#include "include.h"



bool printError(const char *message) {
    printf(RED_TEXT "Error: %s" RESET_TEXT "\n", message);
    return false;
}

bool areStringsEqual(const char *str1, const char *str2){
    return (strcmp(str1, str2) == 0);
}

bool areArgsValid(int argc, char *argv[]){
    if (argc < 2){
        return printError("you sould enter at least one arg :/");
    }

    
    

    for (int i = 0; GIT_commands_list[i].command_name ; i++) // this for actually means until the end of commands
    {
        if (areStringsEqual(argv[1], GIT_commands_list[i].command_name)){
            if (argc > GIT_commands_list[i].max_argc || argc < GIT_commands_list[i].min_argc){
                printError(GIT_commands_list[i].usage_help);
            }
            return true;
        }
    }

    for (int i = 0;i < GIT_alias_count; i++)
    {
        if (areStringsEqual(argv[1], GIT_alias_list[i].name)){
            if (argc != 2){
                printError("alias command cannot have any arg");
                return false;
            }
            return true;
        }
    }

    printError(GIT_USAGE);
    return false;
    
    
}

char *findGitParentDir(){
    // get current working directory
    char *cwd = getcwd(NULL, 0);
    while (strlen(cwd) > 0)
    {
        DIR *dir = opendir(cwd);
        struct dirent *entry;
        bool found = false;
        while ((entry = readdir(dir)))
        {
            if (areStringsEqual(GIT_FOLDER_NAME, entry->d_name)){
                found = true;
                break;
            }
        }
        
        if (found){
            return cwd;
        }
        char *last_slash_ptr = strrchr(cwd, '/');
        *last_slash_ptr = '\0';
    
    }
    
}




// function to tokenize a string and return an array of tokens
TokenArray gigaStrtok(const char *str, const char *delimiters) {
    TokenArray result;
    result.tokens = NULL;
    result.count = 0;

    char *copy = strdup(str);
    char *token = strtok(copy, delimiters);

    while (token != NULL) {
        result.tokens = realloc(result.tokens, (result.count + 1) * sizeof(char *));
        result.tokens[result.count] = strdup(token);
        result.count++;

        token = strtok(NULL, delimiters);
    }

    free(copy);

    return result;
}

// function to free the memory allocated for tokens
void freeTokens(TokenArray *tokenArray) {
    for (size_t i = 0; i < tokenArray->count; ++i) {
        free(tokenArray->tokens[i]);
    }

    free(tokenArray->tokens);
    tokenArray->count = 0;
    tokenArray->tokens = NULL;
}

// all copyright to Mohammad Mahdi Rjabai Robat (XD just kidding)

// look at this nice parse command function that i made just right now :)

TokenArray parseCommand(const char *command){
    
    TokenArray before = gigaStrtok(command, " ");
    TokenArray after = {NULL, 0};
    int final_index = 0;
    for (int i = 0; i < before.count; i++, final_index ++)
    {
        after.count ++;
        after.tokens = realloc(after.tokens, after.count * sizeof(char *));

        after.tokens[after.count - 1] = strdup(before.tokens[i]);

        if(before.tokens[i][0] == '"'){
            
            if (before.tokens[i][strlen(before.tokens[i]) - 1] != '"'){
                bool reached_end = false;
                for(i++; i < before.count; i++){
                    after.tokens[after.count - 1] = realloc(after.tokens[after.count - 1], strlen(after.tokens[after.count - 1]) + strlen(before.tokens[i]) + 2);
                    strcat(after.tokens[after.count - 1], " ");
                    strcat(after.tokens[after.count - 1], before.tokens[i]);
                    if (before.tokens[i][strlen(before.tokens[i]) - 1] == '"'){
                        reached_end = true;
                        break;
                    }
                }
            

                if (!reached_end){
                    TokenArray error = {NULL, 0};
                    return error;
                }
            }
            
        }
    }

    for (int i = 0; i < after.count; i++) // removing double quots
    {
        if (after.tokens[i][0] == '"' && after.tokens[i][strlen(after.tokens[i] - 1)] == '"'){
            char *new_string = strdup(after.tokens[i] + 1);
            new_string[strlen(new_string) - 1] = '\0';
            free(after.tokens[i]);
            after.tokens[i] = new_string;
        }
    }
    
    
    
    return after;
}