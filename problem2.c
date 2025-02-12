/*
 * Problem 2: Square a Number
 *
 * Instructions:
 *   Complete the program so that it:
 *     1. Prompts the user to enter an integer.
 *     2. Reads the integer from standard input.
 *     3. Computes the square of the integer.
 *     4. Prints the square to the terminal.
 *
 * Expected Output Example:
 *   If the user enters 5, the program should output:
 *     25
 *
 * Hint:
 *   Use scanf to read an integer and printf to display the result.
 *   The square of a number is calculated as: square = num * num;
 */

#include <stdio.h>

int main(void) {
    int num;
    int square;
    
    // TODO: Prompt the user for input
    // Example: printf("Enter an integer: ");
    printf("Enter an integer: ");    
    // TODO: Read the integer from the user
    // Example: scanf("%d", &num);
    scanf("%d", &num);
    // TODO: Compute the square of the number
    // Example: square = num * num;
    square = num * num;
    // TODO: Print the result
    // Example: printf("%d\n", square);
    printf("%d\n", square);
    return 0;
}
