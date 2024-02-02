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

// some useful colors

#define RED_TEXT   "\x1B[31m"
#define GRN_TEXT   "\x1B[32m"
#define YEL_TEXT   "\x1B[33m"
#define BLU_TEXT   "\x1B[34m"
#define MAG_TEXT   "\x1B[35m"
#define CYN_TEXT   "\x1B[36m"
#define WHT_TEXT   "\x1B[37m"
#define RESET_TEXT "\x1B[0m"

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


#define printfSuccess(x) printf(BLU_TEXT "Success: ");\
                        printf x;\
                        printf("\n" RESET_TEXT)

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


// void gigaMkdir(const char *path);

char *gigaStrcat(int count, ...);

int mkfile(const char *path);

char *processPath(const char *relative_path);

int32_t getFileAccessCode(const char *path);

typedef struct {
    char *first;
    int first_line_n;
    char *second;
    int second_line_n;
} Difference;

Difference *compareFilesLineByLine(FILE *file1, FILE *file2, int *count_feedback);
Difference *compareFilesLineByLineV2(FILE *file1, FILE *file2, int file1_start, int file1_end, int file2_start, int file2_end, int *count_feedback);

void freeDifference(Difference *difference, int count);

typedef struct {
    char **first_commit_files;
    int first_commit_files_n;
    char **second_commit_files;
    int second_commit_files_n;
    struct {
        char *file_path;
        struct {
            Difference *diff;
            int diff_count;
        } diffs;
    } *commons;
    int commons_n;
} CommitDiff;

CommitDiff *compareTwoCommits(const char *first_commit_hash, const char *second_commit_hash);

void freeCommitDiff(CommitDiff *commit_diff);

char *generateRandomString(size_t length);

bool areFilesEqual(FILE *file1, FILE *file2);

bool copyFile(const char *dest_path, const char *src_path, size_t buffer_size);

bool wildcardMatch(char *wild, char *word);

#endif