#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_LENGTH 100 // Maximum length of a word
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

int main() {
    char **words = NULL; // Array of words
    int numWords;

    // Call the function to read words
    numWords = readWordsFromFile("words", &words, TOTAL_WORDS);
    if (numWords == -1) {
        fprintf(stderr, "Failed to read words from file.\n");
        return 1;
    }

    // Print the total number of words
    printf("Total number of words: %d\n", numWords);

    // Print the 20th word if available
    if (numWords >= 20) {
        printf("The 20th word is: %s\n", words[25139]); // Index 19 is the 20th word
    } else {
        printf("The file contains fewer than 20 words.\n");
    }

    // Free memory
    for (int i = 0; i < numWords; i++) {
        free(words[i]); // Free each allocated word
    }
    free(words); // Free the array of pointers

    return 0;
}
