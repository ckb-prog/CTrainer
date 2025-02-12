#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROBLEMS 100
#define MAX_LINE 2048
#define PROBLEMS_DIR "Problems/"
#define MANIFEST_FILE PROBLEMS_DIR "manifest.txt"
#define TEMP_BIN "temp_bin"

// ANSI escape sequences for yellow text and reset.
#define YELLOW "\033[1;33m"
#define RESET  "\033[0m"

typedef struct {
    char filename[256];        // e.g., "problem1.c"
    char title[256];           // e.g., "Problem 1: Hello, World!"
    char description[1024];    // Detailed instructions for the problem
    char hint[256];            // Hint text
    char expected_output[256]; // Expected output (should be provided without newline)
} Problem;

/* Loads problems from the manifest file.
   Each line in manifest.txt must be in the format:
     filename|title|description|hint|expected_output
*/
int load_problems(Problem problems[], int *numProblems) {
    FILE *fp = fopen(MANIFEST_FILE, "r");
    if (!fp) {
        fprintf(stderr, "Failed to open manifest file: %s\n", MANIFEST_FILE);
        return -1;
    }
    char line[MAX_LINE];
    int count = 0;
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0'; // remove newline
        if (strlen(line) == 0)
            continue;  // skip empty lines

        char *token = strtok(line, "|");
        if (!token)
            continue;
        strncpy(problems[count].filename, token, sizeof(problems[count].filename) - 1);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(problems[count].title, token, sizeof(problems[count].title) - 1);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(problems[count].description, token, sizeof(problems[count].description) - 1);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(problems[count].hint, token, sizeof(problems[count].hint) - 1);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(problems[count].expected_output, token, sizeof(problems[count].expected_output) - 1);

        count++;
        if (count >= MAX_PROBLEMS)
            break;
    }
    fclose(fp);
    *numProblems = count;
    return 0;
}

/* Compiles the given problem source file (from the Problems folder)
   into a temporary binary (named TEMP_BIN).
   Returns the exit status of the gcc command.
*/
int compile_problem(const char *problemFilename) {
    char command[1024];
    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%s%s", PROBLEMS_DIR, problemFilename);
    snprintf(command, sizeof(command), "gcc %s -o %s", fullPath, TEMP_BIN);
    printf("Compiling your solution...\n");
    int ret = system(command);
    return ret;
}

/* Runs the compiled binary and captures its output.
   This function reads all output lines and then extracts the last line that
   is considered "valid" (i.e. not just a prompt). If a line contains
   "Enter an integer:", then it is assumed that any text following that substring
   is the computed result.
   The result is stored in 'output' (after trimming any newline).
   Returns 0 on success, -1 if no valid output is found.
*/
int capture_output(const char *command, char *output, size_t size) {
    FILE *fp = popen(command, "r");
    if (fp == NULL)
        return -1;

    char line[512];
    char valid[512] = "";
    while (fgets(line, sizeof(line), fp) != NULL) {
        // Remove trailing newline.
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0)
            continue;
        char *pos = strstr(line, "Enter an integer:");
        if (pos != NULL) {
            // Move pointer after the prompt text.
            pos += strlen("Enter an integer:");
            // Skip any leading spaces.
            while (*pos == ' ')
                pos++;
            if (strlen(pos) > 0) {
                strncpy(valid, pos, sizeof(valid)-1);
                valid[sizeof(valid)-1] = '\0';
            }
        } else {
            // If line does not contain the prompt, assume it's the valid output.
            strncpy(valid, line, sizeof(valid)-1);
            valid[sizeof(valid)-1] = '\0';
        }
    }
    pclose(fp);
    if (strlen(valid) == 0) {
        output[0] = '\0';
        return -1;
    } else {
        strncpy(output, valid, size-1);
        output[size-1] = '\0';
        return 0;
    }
}

int main(void) {
    Problem problems[MAX_PROBLEMS];
    int numProblems = 0;

    if (load_problems(problems, &numProblems) != 0)
        return 1;

    printf("Welcome to the Interactive C Tutorial!\n");
    printf("This tutorial will guide you from beginner to advanced C programming.\n");
    printf("Each problem must compile and produce the expected output to proceed.\n");
    printf("If you need a hint, type 'h' followed by Enter before editing your solution.\n\n");

    char input[16];
    char command[1024];
    char output[512];

    for (int i = 0; i < numProblems; i++) {
        int solved = 0;
        while (!solved) {
            printf("--------------------------------------------------\n");
            printf("%s\n", problems[i].title);
            printf("%s\n\n", problems[i].description);
            printf("Type 'h' (then Enter) for a hint, or just press Enter to open Vim for editing: ");
            fgets(input, sizeof(input), stdin);
            if (input[0] == 'h' || input[0] == 'H') {
                printf("Hint: %s\n", problems[i].hint);
                printf("Press Enter to open Vim for editing: ");
                fgets(input, sizeof(input), stdin);
            }
            
            // Open Vim for the user to edit the problem file.
            snprintf(command, sizeof(command), "vim %s%s", PROBLEMS_DIR, problems[i].filename);
            system(command);

            // Attempt to compile the solution.
            if (compile_problem(problems[i].filename) != 0) {
                printf("Compilation failed. Please fix your code in Vim.\n");
                printf("Press Enter to try editing again: ");
                fgets(input, sizeof(input), stdin);
                continue;
            }

            // Run the compiled binary and capture the output.
            if (capture_output("./" TEMP_BIN, output, sizeof(output)) != 0) {
                printf("Error running your solution. Please try editing again.\n");
                printf("Press Enter to re-open Vim for editing: ");
                fgets(input, sizeof(input), stdin);
                continue;
            }

            // Compare the captured output with the expected output.
            if (strcmp(output, problems[i].expected_output) == 0) {
                printf(YELLOW "Success!" RESET " Your program produced the expected output.\n\n");
                solved = 1;
            } else {
                printf("The output did not match the expected result.\n");
                printf("Expected: %s\n", problems[i].expected_output);
                printf("Got: %s\n", output);
                printf("Press Enter to re-open Vim for editing: ");
                fgets(input, sizeof(input), stdin);
            }
        }
    }

    // Clean up the temporary binary.
    remove(TEMP_BIN);
    printf("Congratulations! You have completed all problems.\n");
    return 0;
}

