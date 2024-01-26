#ifndef __USEFUL_FUNCTIONS_H_
#define __USEFUL_FUNCTIONS_H_

bool areStringsEqual(const char *str1, const char *str2);

bool printError(const char *error_msg);

bool areArgsValid(int argc, char *argv[]);

char *findGitParentDir();

#define RED_TEXT "\033[1;31m"
#define RESET_TEXT "\033[0m"
#define printfError(x, y) printf(RED_TEXT "Error: "x" " RESET_TEXT "\n", y)

// my new string functions
typedef struct {
    char **tokens;
    size_t count;
} TokenArray;

TokenArray gigaStrtok(const char *str, const char *delimiters);
void freeTokens(TokenArray *tokenArray);


#endif