/*
C pointer syntax:
To initialize a pointer use *, ex: *pPointer (using a small p before the variable you are pointing to is the naming convetion)
A pointer points to the memory adress of a certain variable to extract the adress of a variable use & operator
To extract the value at the memory adress the pointer is pointing to derefrence it with * operator.
Example:
int age = 10;
int *pAge = &age; (pAge will point to the memory location of the variable age)
printf(pAge); (will print the address the pointer is pointing to e.g. the adress of variable age. The * operator only needs to be used when creating the pointer)
printf(*pAge); (will print the value stored at the address derefrencing the pointer, in this case the value is 10)
*/
/*
C Array syntax:
2D array = an array, where each element is an entire array useful if you need a matrix, grid, or table of data
int numbers[2][3] = {
                    {1, 2, 3},
                    {4, 5, 6}
                };
[2] = number of rows
[3] = number of columns
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_WORD_LENGTH 22 // Maximum length of a word
#define TOTAL_WORDS 25143   // Total number of words

/**
 * Function to read words from a file into an array.
 * Returns the number of words read or -1 on failure.
 */
int readWordsFromFile(const char *filename, char ***words, int maxWords) {
    FILE *file;
    char buffer[MAX_WORD_LENGTH];
    int count = 0;

    // Open the file in read mode
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    // Allocate memory for the array of word pointers
    *words = malloc(maxWords * sizeof(char *));
    if (*words == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return -1;
    }

    // Read words from the file
    while (count < maxWords && fscanf(file, "%s", buffer) == 1) {
        (*words)[count] = malloc(strlen(buffer) + 1); // Allocate memory for each word
        if ((*words)[count] == NULL) {
            perror("Memory allocation failed");
            fclose(file);
            return -1;
        }
        strcpy((*words)[count], buffer); // Copy the word into the allocated space
        count++;
    }

    fclose(file);
    return count; // Return the number of words read
}

/* Checks if a word is a palindrome */
bool isPalindrome(const char *str) {
    int left = 0;
    int right = strlen(str) - 1;

    while (left < right) {
        if (str[left] != str[right]) {
            return false;
        }
        left++;
        right--;
    }
    return true;
}

bool binarySearch(char **words, int numWords, const char *target) {
    int left = 0;
    int right = numWords - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = strcasecmp(words[mid], target);

        if (cmp == 0) {
            return true; 
        } else if (cmp < 0) {
            left = mid + 1; 
        } else {
            right = mid - 1; 
        }
    }
    return false; 
}

void reverseString(char *str) {
    int start = 0;
    int end = strlen(str) - 1;

    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;

        start++;
        end--;
    }
}

int main() {
    char **words = NULL; // Array of words
    char **resultStrings = NULL; // Dynamic array for palindromes
    int numWords;
    char str[] = "hello"; //temporary word to reverse

    // Call the function to read words
    numWords = readWordsFromFile("words", &words, TOTAL_WORDS);
    if (numWords == -1) {
        fprintf(stderr, "Failed to read words from file.\n");
        return 1;
    }


    int SemordnilapCount = 0;
    const char *testWord = "sussy"; // Replace with the word you want to check
    if (binarySearch(words, numWords, testWord)) {
        printf("The word '%s' is present in the array.\n", testWord);
        SemordnilapCount++;
    } else {
        printf("The word '%s' is NOT present in the array.\n", testWord);
    }

    // reverse word function call
    reverseString(str);
    printf("reversed string: %s\n", str);

    int palindromeCount = 0;
    for (int i = 0; i < numWords; i++) {
        if (isPalindrome(words[i])) {
            resultStrings = realloc(resultStrings, (palindromeCount + 1) * sizeof(char *));
            if (resultStrings == NULL) {
                perror("Memory allocation failed");
                return 1;
            }

            resultStrings[palindromeCount] = malloc(strlen(words[i]) + 1);
            if (resultStrings[palindromeCount] == NULL) {
                perror("Memory allocation failed");
                return 1;
            }
            strcpy(resultStrings[palindromeCount], words[i]);
            palindromeCount++;
        }
    }

    printf("Total number of words: %d\n", numWords);
    printf("Found %d palindrome(s):\n", palindromeCount);
    for (int i = 0; i < palindromeCount; i++) {
        printf("%s\n", resultStrings[i]);
        free(resultStrings[i]); // Free each palindrome string
    }
    free(resultStrings); // Free the array of pointers

     

    // Free memory allocated for words
    for (int i = 0; i < numWords; i++) {
        free(words[i]);
    }
    free(words);

    return 0;
}
