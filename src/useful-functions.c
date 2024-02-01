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
                return false;
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
            if (areStringsEqual(GIT_DIR_NAME, entry->d_name)){
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


char *gigaStrcat(int count, ...){

    if (count < 2){
        return NULL;
    }

    va_list args;
    va_start(args, count);


    char *res = strdup(va_arg(args, const char *));

    for (int i = 1; i < count; i++)
    {
        
        char *next_string = strdup(va_arg(args, const char *));
        res = realloc(res, strlen(res) + strlen(next_string) + 1);
        strcat(res, next_string);
        free(next_string);
    }
    
    return res;
}


// my giga mkdir function

// void gigaMkdir(const char *path) {
//     char *path_copy = strdup(path);
//     char *dir_name = strtok(path_copy, "/");
//     char *dir_path = dir_name;
//     while (dir_name != NULL) {
//         mkdir(dir_path, 0777); // just tries to create it  if it exists, it will just fail :) 
//         dir_name = strtok(NULL, "/");
//         if (dir_name != NULL) {
//             dir_path = strcat(strcat(dir_path, "/"), dir_name);
//         }
//     }
//     free(path_copy);
// }


// file related functions

int mkfile(const char *path){
    FILE *file = fopen(path, "w");
    if (file == NULL){
        return EXIT_FAILURE;
    }
    fclose(file);
    return EXIT_SUCCESS;
}

bool getNextNonBlankLine(FILE *file, char *output, int *new_line_n){
    int n = *new_line_n;
    while (true) {
        n ++;
        if (fgets(output, MAX_LINE_LEN, file) == NULL){
            return false;
        }
        bool is_blank = true;
        for (int i = 0; output[i]; i++)
        {
            if (output[i] != ' ' && output[i] != '\n'){
                output[strcspn(output, "\n")] = '\0'; // removing \n
                is_blank = false;
                break;
            }
        }
        if (!is_blank) break;
            
    }
    *new_line_n = n;
    return true;
}

Diffrence *compareFilesLineByLine(FILE *file1, FILE *file2, int *count_feedback){ // these files should be opened at read mode
    if (!(file1 && file2)){
        return NULL;
    }

    rewind(file1);
    rewind(file2);

    char  line1[MAX_LINE_LEN], line2[MAX_LINE_LEN];
    Diffrence *res = NULL;

    int count = 0;
    int file1_line_n = 0, file2_line_n = 0;
    while (true)
    {

        int file1_found = getNextNonBlankLine(file1, line1, &file1_line_n);
        int file2_found = getNextNonBlankLine(file2, line2, &file2_line_n);

        
        


        if ((file1_found != file2_found || strcmp(line1, line2)) && (file1_found || file2_found)){
            count ++;
            res = realloc(res , count * sizeof(Diffrence));
            if (file1_found) {res[count - 1].first = strdup(line1); res[count - 1].first_line_n = file1_line_n;}
            else             {res[count - 1].first = strdup(""); res[count - 1].first_line_n = 0;}
            if (file2_found) {res[count - 1].second = strdup(line2); res[count - 1].second_line_n = file2_line_n;}
            else             {res[count - 1].second = strdup(""); res[count - 1].second_line_n = 0;}
        }

        if ( (!file1_found) && (!file2_found)){
            break;
        }
    }

    *count_feedback = count;
    
    return res;

}

void freeDiffrence(Diffrence *diffrence, int count){
    for (int i = 0; i < count; i++)
    {
        free(diffrence[i].first);
        free(diffrence[i].second);
    }

    free(diffrence);
}

bool areFilesEqual(FILE *file1, FILE *file2){
    int diff_count;
    Diffrence *diffrence = compareFilesLineByLine(file1, file2, &diff_count);
    free(diffrence);
    
    return (diff_count == 0);
}


bool copyFile(const char *dest_path, const char *src_path, size_t buffer_size){
    FILE *dest = fopen(dest_path, "wb");
    FILE *src = fopen(src_path, "rb");

    if (!(dest && src)){
        return false;
    }

    void *tmp = malloc(buffer_size);

    while (true)
    {
        int count = fread(tmp, 1, buffer_size, src);
        fwrite(tmp, 1, count, dest);

        if (count < buffer_size){

            fclose(src);
            fclose(dest);

            return true;
        }
    }
    
}


// random functions

char *processPath(const char *relative_path){

    if (!GIT_parent_dir){
        return NULL;
    }

    char absolute_path[MAX_PATH_LEN];

    realpath(relative_path, absolute_path);
    if (strlen(absolute_path) < strlen(GIT_parent_dir)){
        return NULL;
    }
    char *res;
    if(strlen(absolute_path) == strlen(GIT_parent_dir)){
        res = strdup(".");
        return res; // paths are equal
    }

    res = strdup(absolute_path + strlen(GIT_parent_dir) + 1);

    return res;
}

int32_t getFileAccessCode(const char *path){
    struct stat file_stat;
    int code = stat(path, &file_stat);
    if (-1 == code){
        printfError("error while trying to get access code of %s", path);
        exit(EXIT_FAILURE); // because it is in a dangrous place
    }

    return (file_stat.st_mode % 01000);
}

char *generateRandomString(size_t length) { // const size_t length, supra

    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#?!"; // could be const
    char *randomString;

    if (length) {
        randomString = malloc(length +1); // sizeof(char) == 1, cf. C99

        if (randomString) {
            int l = (int) (sizeof(charset) -1); // (static/global, could be const or #define SZ, would be even better)
            int key;  // one-time instantiation (static/global would be even better)
            for (int n = 0;n < length;n++) {        
                key = rand() % l;   // no instantiation, just assignment, no overhead from sizeof
                randomString[n] = charset[key];
            }

            randomString[length] = '\0';
        }
    }

    return randomString;
}

bool wildcardMatch(char *wild, char *word) { 
    // If we reach at the end of both strings, we are done 
    if (*wild == '\0' && *word == '\0') 
        return true; 
  
    // Make sure to eliminate consecutive '*' 
    if (*wild == '*') { 
        while (*(wild + 1) == '*') 
            wild++; 
    } 
  
    // Make sure that the characters after '*' are present 
    // in word string. This function assumes that the 
    // wild string will not contain two consecutive '*' 
    if (*wild == '*' && *(wild + 1) != '\0'
        && *word == '\0') 
        return false; 
  
    // If the wild string contains '?', or current 
    // characters of both strings match 
    if (*wild == '?' || *wild == *word) 
        return wildcardMatch(wild + 1, word + 1); 
  
    // If there is *, then there are two possibilities 
    // a) We consider current character of word string 
    // b) We ignore current character of word string. 
    if (*wild == '*') 
        return wildcardMatch(wild + 1, word) 
               || wildcardMatch(wild, word + 1); 
    return false; 
} 