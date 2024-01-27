#ifndef __USEFUL_FUNCTIONS_H_
#define __USEFUL_FUNCTIONS_H_

/**
 * @brief Prints an error message to the standard error stream.
 *
 * This function formats and prints an error message to the standard error stream
 * with a distinctive color. It is commonly used to display error messages in the
 * context of a command-line application.
 *
 * @param message The error message to be displayed.
 * @return bool Always returns false, indicating that an error occurred.
 */
bool printError(const char *message);

/**
 * @brief Compares two strings for equality.
 *
 * This function compares two strings and returns true if they are equal; otherwise,
 * it returns false. The comparison is case-sensitive.
 *
 * @param str1 The first string for comparison.
 * @param str2 The second string for comparison.
 * @return bool Returns true if the strings are equal; otherwise, false.
 */
bool areStringsEqual(const char *str1, const char *str2);


/**
 * @brief Validates the command-line arguments for a Git-like application.
 *
 * This function checks if the provided command-line arguments are valid based on
 * predefined commands, their aliases, and associated usage rules. It ensures that
 * the correct number of arguments is provided for the specified command or alias.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments.
 * @return bool Returns true if the arguments are valid; otherwise, false. If
 * invalid, an error message is printed to the standard error stream.
 */
bool areArgsValid(int argc, char *argv[]);


/**
 * @brief Finds the parent directory containing a Git repository.
 *
 * This function iteratively searches for the nearest parent directory
 * containing a folder named "`.git`" (Git repository). It starts the search
 * from the current working directory and moves upwards until a Git repository
 * is found or the root directory is reached.
 *
 * @return char* A dynamically allocated string representing the path of the
 * found Git repository's parent directory. The caller is responsible for
 * freeing the memory allocated for the returned string. If no Git repository
 * is found, returns NULL.
 */
char *findGitParentDir();


#define RED_TEXT "\033[1;31m"
#define RESET_TEXT "\033[0m"
/**
 * @brief Prints a formatted error message to the standard output.
 *
 * This macro is designed to print a formatted error message to the standard
 * output with a distinctive color. It uses the provided format string (x) and
 * the corresponding argument (y) to generate the error message. The color macros
 * (e.g., RED_TEXT and RESET_TEXT) are expected to be defined in the code.
 *
 * @param x The format string for the error message.
 * @param y The argument(s) to be included in the formatted error message.
 */
#define printfError(x, y) printf(RED_TEXT "Error: " x " " RESET_TEXT "\n", y)


/**
 * @brief Represents an array of tokens.
 *
 * TokenArray is a structure that holds an array of strings (tokens) along with
 * the count of tokens in the array. It is commonly used for storing parsed
 * command-line arguments or similar tokenized data. The memory for both the
 * array of tokens and the TokenArray structure itself is expected to be managed
 * dynamically.
 */
typedef struct {
    char **tokens;  /**< Array of strings (tokens). */
    size_t count;   /**< Number of tokens in the array. */
} TokenArray;



/**
 * @brief Tokenizes a string based on specified delimiters.
 *
 * This function takes a string and breaks it into tokens using the provided
 * delimiters. The resulting tokens are stored in a dynamically allocated array,
 * and the total count of tokens is recorded in the returned TokenArray.
 *
 * @param str The input string to be tokenized.
 * @param delimiters A string containing the delimiters used for tokenization.
 * @return TokenArray An array of tokens extracted from the input string.
 * The caller is responsible for freeing the memory used by the returned TokenArray.
 */
TokenArray gigaStrtok(const char *str, const char *delimiters);



/**
 * @brief Frees the memory allocated for tokens in a TokenArray.
 *
 * This function releases the memory allocated for each token in the TokenArray
 * and then frees the array itself. After calling this function, the TokenArray
 * becomes empty, and its count is set to zero.
 *
 * @param tokenArray Pointer to the TokenArray whose tokens need to be freed.
 * It should be allocated using dynamic memory.
 */
void freeTokens(TokenArray *tokenArray);

/**
 * @brief Parses a command string into an array of tokens.
 *
 * This function takes a command string and breaks it into individual tokens,
 * considering space as the delimiter. It supports handling double-quoted
 * substrings as single tokens. The resulting array of tokens is dynamically
 * allocated and should be freed by the caller when no longer needed.
 *
 * @param command The input command string to be parsed.
 * @return TokenArray An array of tokens extracted from the command. If parsing
 * encounters an error (e.g., unmatched double quotes), an empty TokenArray is
 * returned.
 */
TokenArray parseCommand(const char *command);


#endif