#include "include.h"



bool printError(const char *message) {
    printf(RED_TEXT "  %s" RESET_TEXT "\n", message);
    return false;
}

bool areStringsEqual(const char *str1, const char *str2){
    return (strcmp(str1, str2) == 0);
}

bool areArgsValid(int argc, char *argv[]){
    if (argc < 2){
        return printError(GIT_USAGE);
    }

    
    

    for (int i = 0; GIT_commands_list[i].command_name ; i++) // this for actually means until the end of commands
    {
        if (areStringsEqual(argv[1], GIT_commands_list[i].command_name)){
            if (argc > GIT_commands_list[i].max_argc || argc < GIT_commands_list[i].min_argc){

                printError("wrong command usage.");
                printf(GRN_TEXT);
                printf(GIT_commands_list[i].usage_help);
                printf("\n"RESET_TEXT);

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

    for (int i = 0; i < after.count; i++) // removing double quotes
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

Difference *compareFilesLineByLine(FILE *file1, FILE *file2, int *count_feedback){ // these files should be opened at read mode
    if (!(file1 && file2)){
        return NULL;
    }

    rewind(file1);
    rewind(file2);

    char  line1[MAX_LINE_LEN], line2[MAX_LINE_LEN];
    Difference *res = NULL;

    int count = 0;
    int file1_line_n = 0, file2_line_n = 0;
    while (true)
    {

        int file1_found = getNextNonBlankLine(file1, line1, &file1_line_n);
        int file2_found = getNextNonBlankLine(file2, line2, &file2_line_n);

        
        


        if ((file1_found != file2_found || strcmp(line1, line2)) && (file1_found || file2_found)){
            count ++;
            res = realloc(res , count * sizeof(Difference));
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



Difference *compareFilesLineByLineV2(FILE *file1, FILE *file2, int file1_start, int file1_end, int file2_start, int file2_end, int *count_feedback){ // these files should be opened at read mode
    if (!(file1 && file2)){
        return NULL;
    }

    rewind(file1);
    rewind(file2);

    // moving files to the specified line.
    char tmp_line[MAX_LINE_LEN];
    for (int i = 0; i < file1_start - 1; i++)
    {
        fgets(tmp_line, MAX_LINE_LEN, file1);
    }
    for (int i = 0; i < file2_start - 1; i++)
    {
        fgets(tmp_line, MAX_LINE_LEN, file2);
    }


    char  line1[MAX_LINE_LEN], line2[MAX_LINE_LEN];
    Difference *res = NULL;

    int count = 0;
    int file1_line_n = file1_start - 1, file2_line_n = file2_start - 1;
    while (true)
    {

        int file1_found = getNextNonBlankLine(file1, line1, &file1_line_n);
        int file2_found = getNextNonBlankLine(file2, line2, &file2_line_n);

        if(file1_line_n > file1_end) file1_found = false;
        if(file2_line_n > file2_end) file2_found = false;
        


        if ((file1_found != file2_found || strcmp(line1, line2)) && (file1_found || file2_found)){
            count ++;
            res = realloc(res , count * sizeof(Difference));
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

void freeDifference(Difference *difference, int count){
    for (int i = 0; i < count; i++)
    {
        free(difference[i].first);
        free(difference[i].second);
    }

    free(difference);
}


void printDifferences(Difference *difference, const char *first_label, const char *second_label, const char *icon){

    TokenArray first_words = gigaStrtok(difference->first, " \n");
    TokenArray second_words = gigaStrtok(difference->second, " \n");
    int word_index = -1;

    if (first_words.count == second_words.count){
        int diff_count = 0;
        for (size_t i = 0; i < first_words.count; i++)
        {
            if (!areStringsEqual(first_words.tokens[i], second_words.tokens[i])) {
                diff_count ++;
                word_index = i;
            };
        }

        if (diff_count != 1){
            word_index = -1;
        }

    }
    
    if (difference->first_line_n != 0){
        printf(WHT_TEXT"󰇘󰇘󰇘󰇘 "BLU_TEXT"%s %s 󰞷 %d\n", icon, first_label, difference->first_line_n);
        printf(WHT_TEXT"󰇘󰇘󰇘󰇘󰇘󰇘󰇘󰇘 "CYN_TEXT);
        for (int i = 0; i < first_words.count; i++)
        {
            if (i == word_index) printf(RED_TEXT"»"CYN_TEXT"%s"RED_TEXT"« "CYN_TEXT, first_words.tokens[i]);
            else printf("%s ", first_words.tokens[i]);
        }
    }else{
        printf(WHT_TEXT"󰇘󰇘󰇘󰇘 "BLU_TEXT"%s %s\n", icon, first_label);
        printf(WHT_TEXT"󰇘󰇘󰇘󰇘󰇘󰇘󰇘󰇘 FINISHED!!"CYN_TEXT);
    }
    printf("\n");

    if (difference->second_line_n != 0){
        printf(WHT_TEXT"󰇘󰇘󰇘󰇘 "RED_TEXT"%s %s 󰞷 %d\n", icon, second_label, difference->second_line_n);
        printf(WHT_TEXT"󰇘󰇘󰇘󰇘󰇘󰇘󰇘󰇘 "YEL_TEXT);
        for (int i = 0; i < second_words.count; i++)
        {
            if (i == word_index) printf(RED_TEXT"»"YEL_TEXT"%s"RED_TEXT"« "YEL_TEXT, second_words.tokens[i]);
            else printf("%s ", second_words.tokens[i]);
        }
    }else{
        printf(WHT_TEXT"󰇘󰇘󰇘󰇘 "RED_TEXT"%s %s\n", icon, second_label);
        printf(WHT_TEXT"󰇘󰇘󰇘󰇘󰇘󰇘󰇘󰇘 FINISHED!!"YEL_TEXT);
    }
    printf("\n"RESET_TEXT);

}


CommitDiff *compareTwoCommits(const char *first_commit_hash, const char *second_commit_hash){
    CommitDiff *result = malloc(sizeof(CommitDiff));
    result->commons = NULL;
    result->commons_n = 0;
    result->first_commit_files = NULL;
    result->first_commit_files_n = 0;
    result->second_commit_files = NULL;
    result->second_commit_files_n = 0;
    result->deleted_files = NULL;
    result->deleted_files_n = 0;

    Commit *first_commit = openCommit(first_commit_hash);
    Commit *second_commit = openCommit(second_commit_hash);

    if (!(first_commit && second_commit)) return NULL;

    if ((first_commit->meta_data.files_count < 0) || (second_commit->meta_data.files_count < 0)){
        printError("Core Exploded!!: in function compareTwoCommits\none of commits has a negative file count");
        return NULL;
    }

    bool checklist[second_commit->meta_data.files_count];

    for (int i = 0; i < second_commit->meta_data.files_count; i++)
    {
        checklist[i] = false;
    }
    
    

    for (int i = 0; i < first_commit->meta_data.files_count; i++)
    {
        if (first_commit->files[i].access_code == -1) continue;
        bool is_common = false;
        for (int j = 0; j < second_commit->meta_data.files_count; j++)
        {
            if (second_commit->files[j].access_code == -1) continue;
            if (areStringsEqual(first_commit->files[i].path, second_commit->files[j].path)){
                is_common = true;
                checklist[j] = true;
                result->commons_n ++;
                result->commons = realloc(result->commons, sizeof(*result->commons) * (result->commons_n));
                

                int index = result->commons_n - 1;
                result->commons[index].file_path =  strdup(first_commit->files[i].path);

                FILE *first_object = openObject(first_commit->files[i].object_hash, "r");
                FILE *second_object = openObject(second_commit->files[j].object_hash, "r");
                result->commons[index].diffs.diff = compareFilesLineByLine(first_object, second_object, &result->commons[index].diffs.diff_count);
            }
        }

        if (!is_common){
            result->first_commit_files_n ++;
            result->first_commit_files = realloc(result->first_commit_files, sizeof(*result->first_commit_files) * result->first_commit_files_n);

            int index = result->first_commit_files_n - 1;
            result->first_commit_files[index] = strdup(first_commit->files[i].path);
        }
        
    }
    
    for (int i = 0; i < second_commit->meta_data.files_count; i++)
    {
        if(second_commit->files[i].access_code == -1) continue;
        if (!checklist[i]){
            result->second_commit_files_n ++;
            result->second_commit_files = realloc(result->second_commit_files, sizeof(*result->second_commit_files) * result->second_commit_files_n);

            int index = result->second_commit_files_n - 1;
            result->second_commit_files[index] = strdup(second_commit->files[i].path);
        }
    }


    for (int i = 0; i < first_commit->meta_data.files_count; i++)
    {
        if (first_commit->files[i].access_code == -1){ // a deleted file
            bool unique = true;

            for (int j = 0; j < result->deleted_files_n; j++)
            {
                if (areStringsEqual(result->deleted_files[j], first_commit->files[i].path)){
                    unique = false;
                    break;
                }
            }
            if (!unique) continue;
            for (int j = 0; j < result->commons_n; j++)
            {
                if (areStringsEqual(result->commons[j].file_path, first_commit->files[i].path)){
                    unique = false;
                    break;
                }
            }
            if (!unique) continue;
            for (int j = 0; j < result->first_commit_files_n; j++)
            {
                if (areStringsEqual(result->first_commit_files[j], first_commit->files[i].path)){
                    unique = false;
                    break;
                }
            }
            if (!unique) continue;
            for (int j = 0; j < result->second_commit_files_n; j++)
            {
                if (areStringsEqual(result->second_commit_files[j], first_commit->files[i].path)){
                    unique = false;
                    break;
                }
            }
            if (!unique) continue;
    
            result->deleted_files_n ++;
            result->deleted_files = realloc(result->deleted_files, result->deleted_files_n * sizeof(char *));
            result->deleted_files[result->deleted_files_n - 1] = strdup(first_commit->files[i].path);
        }
    }
    
    for (int i = 0; i < second_commit->meta_data.files_count; i++)
    {
        if (second_commit->files[i].access_code == -1){ // a deleted file
            bool unique = true;

            for (int j = 0; j < result->deleted_files_n; j++)
            {
                if (areStringsEqual(result->deleted_files[j], second_commit->files[i].path)){
                    unique = false;
                    break;
                }
            }
            if (!unique) continue;
            for (int j = 0; j < result->commons_n; j++)
            {
                if (areStringsEqual(result->commons[j].file_path, second_commit->files[i].path)){
                    unique = false;
                    break;
                }
            }
            if (!unique) continue;
            for (int j = 0; j < result->first_commit_files_n; j++)
            {
                if (areStringsEqual(result->first_commit_files[j], second_commit->files[i].path)){
                    unique = false;
                    break;
                }
            }
            if (!unique) continue;
            for (int j = 0; j < result->second_commit_files_n; j++)
            {
                if (areStringsEqual(result->second_commit_files[j], second_commit->files[i].path)){
                    unique = false;
                    break;
                }
            }
            if (!unique) continue;

            if (unique){
                result->deleted_files_n ++;
                result->deleted_files = realloc(result->deleted_files, result->deleted_files_n * sizeof(char *));
                result->deleted_files[result->deleted_files_n - 1] = strdup(second_commit->files[i].path);
            }
        }
    }

    
    freeCommit(first_commit);
    freeCommit(second_commit);

    return result;

}

void freeCommitDiff(CommitDiff *commit_diff){
    for (int i = 0; i < commit_diff->commons_n; i++)
    {
        free(commit_diff->commons[i].file_path);
        freeDifference(commit_diff->commons[i].diffs.diff, commit_diff->commons[i].diffs.diff_count);
    }
    
    for (int i = 0; i < commit_diff->first_commit_files_n; i++)
    {
        free(commit_diff->first_commit_files[i]);
    }
    
    for (int i = 0; i < commit_diff->second_commit_files_n; i++)
    {
        free(commit_diff->second_commit_files[i]);
    }

    for (int i = 0; i < commit_diff->deleted_files_n; i++)
    {
        free(commit_diff->deleted_files[i]);
    }
    
    if (commit_diff->deleted_files) free(commit_diff->deleted_files);
    if (commit_diff->commons) free(commit_diff->commons);
    if (commit_diff->first_commit_files) free(commit_diff->first_commit_files);
    if (commit_diff->second_commit_files) free(commit_diff->second_commit_files);
    free(commit_diff);
}


bool areFilesEqual(FILE *file1, FILE *file2){
    int diff_count;
    Difference *difference = compareFilesLineByLine(file1, file2, &diff_count);
    free(difference);
    
    return (diff_count == 0);
}

int createDirectories(const char *path) {
    // Create directories recursively
    char *what = strdup(path);
    char *command = gigaStrcat(3, "mkdir -p \"", dirname(what), "\"");
    int result = system(command);
    free(command);

    return result;
}


bool copyFile(const char *dest_path, const char *src_path, size_t buffer_size){
    if (createDirectories(dest_path) == -1){
        printError("could not create directories.");
        return false;
    }
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

void realpathCommandLine(const char *path, char *res){
    char *command = gigaStrcat(3, "realpath -m \"", path, "\"");
    FILE *terminal = popen(command, "r");
    fscanf(terminal, "%[^\n]", res);
    pclose(terminal);
    free(command);
}

char *processPath(const char *relative_path){

    if (!GIT_parent_dir){
        return NULL;
    }

    char absolute_path[MAX_PATH_LEN];

    realpathCommandLine(relative_path, absolute_path);

    if (strlen(absolute_path) < strlen(GIT_parent_dir)){
        return NULL;
    }
    char *res;

    if (strstr(absolute_path, GIT_parent_dir) != absolute_path){
        printError("this path is out of your giga git directory.");
        exit(EXIT_FAILURE);
    }

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
        exit(EXIT_FAILURE); // because it is in a dangerous place
    }

    return (file_stat.st_mode % 01000);
}

char *generateRandomString(size_t length) { 

    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; 
    char *randomString;

    if (length) {
        randomString = malloc(length +1);

        if (randomString) {
            int l = (int) (sizeof(charset) -1); 
            int key; 
            for (int n = 0;n < length;n++) {        
                key = rand() % l; 
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
  

    if (*wild == '*') { 
        while (*(wild + 1) == '*') 
            wild++; 
    } 
 
    if (*wild == '*' && *(wild + 1) != '\0'
        && *word == '\0') 
        return false; 
  

    if (*wild == '?' || *wild == *word) 
        return wildcardMatch(wild + 1, word + 1); 
  

    if (*wild == '*') 
        return wildcardMatch(wild + 1, word) 
               || wildcardMatch(wild, word + 1); 
    return false; 
} 
