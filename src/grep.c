#include "include.h"


int GIT_Grep(int argc, char **argv){
    char *path = NULL;
    char *processed_path = NULL;
    char *word = NULL;
    char *commit_hash = NULL;
    bool show_lines_n = false;
    FILE *file = NULL;

    int option;

    while ((option = getopt(argc, argv, "f:p:c:n")) != -1)
    {
        if (option == ':' || option == '?') return EXIT_FAILURE;

        if (option == 'f'){
            path = strdup(optarg);
            processed_path = processPath(optarg);
        }

        if (option == 'p'){
            word = strdup(optarg);
        }

        if (option == 'c'){
            commit_hash = strdup(optarg);
        }

        if (option == 'n'){
            show_lines_n = true;
        }
    }

    if (path == NULL){
        printError("You should specify a file to search in.");
        return EXIT_FAILURE;
    }

    if (word == NULL){
        printError("You should specify the word you want to find.");
        return  EXIT_FAILURE;
    }

    if (commit_hash){
        Commit *commit = openCommit(commit_hash);
        if (!commit){
            printError("Commit hash is wrong.");
            return EXIT_FAILURE;
        }

        for (int i = 0; i < commit->meta_data.files_count; i++)
        {
            if (commit->files[i].access_code == -1) continue;
            if (areStringsEqual(commit->files[i].path, processed_path)){
                file = openObject(commit->files[i].object_hash, "r");
                break;
            }
        }
    }else {
        file = fopen(path, "r");
    }

    if (!file){
        printError("Could not open specified file.");
        return EXIT_FAILURE;
    }

    char line[MAX_LINE_LEN];
    int line_counter = 0;
    while (fgets(line, MAX_LINE_LEN, file) != NULL)
    {
        line_counter ++;
        TokenArray words = gigaStrtok(line, " \n");
        bool found = false;
        for (int i = 0; i < words.count; i++)
        {
            if (wildcardMatch(word, words.tokens[i])){
                found = true;
                break;
            }
        }
        
        if (!found){
            freeTokens(&words);
            continue;
        }

        if (show_lines_n) printf(CYN_TEXT"󰞷 %d "RESET_TEXT, line_counter);
        for (int i = 0; i < words.count; i++)
        {
            if (wildcardMatch(word, words.tokens[i])){
                printf(GRN_TEXT);
            }
            printf("%s "RESET_TEXT, words.tokens[i]);
        }
        printf("\n");
        freeTokens(&words);

    }
    
    return EXIT_SUCCESS;

}