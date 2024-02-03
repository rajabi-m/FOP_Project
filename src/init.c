#include "include.h"

int createGitSubDir(const char *dir_name);
int createGitSubFile(const char *file_name, const char *default_value);

int GIT_Init(int argc, char **argv){
    if (GIT_parent_dir){
        printError("you are already in a git dir :/ ");
        exit(EXIT_FAILURE);
    }

    

    // creating git dir
    mkdir(GIT_DIR_NAME, 0755);
    mkdir(GIT_DIR_NAME "/" LOCAL_CONFIG_DIR, 0755);
    mkdir(GIT_DIR_NAME "/" COMMITS_DIR, 0755);
    mkdir(GIT_DIR_NAME "/" OBJECTS_DIR, 0755);

    // creating git files
    // mkfile(GIT_DIR_NAME "/" HEAD_FILE_NAME);
    mkfile(GIT_DIR_NAME "/" STAGE_FILE_NAME);
    mkfile(GIT_DIR_NAME "/" STAGE_HISTORY_FILE_NAME);
    mkfile(GIT_DIR_NAME "/" MSG_SHORTCUTS_FILE_NAME);
    mkfile(GIT_DIR_NAME "/" TAGS_FILE_NAME);
    
    // creating Branches file and adding master branch
    Branch master_branch = {MAIN_BRANCH_NAME, ""};
    FILE * branches_file = fopen(GIT_DIR_NAME "/" BRANCHES_FILE_NAME, "wb");
    fwrite(&master_branch, sizeof(master_branch), 1, branches_file);
    fclose(branches_file);

    // creating git head file and save master branch on it
    FILE *head_file = fopen(GIT_DIR_NAME "/" HEAD_FILE_NAME, "w");
    fputs(MAIN_BRANCH_NAME",1,", head_file); // this 1 indicates that head is attached
    fclose(head_file);


    printfSuccess(("git folder created successfully!."));
    return EXIT_SUCCESS;
}


// int createGitSubDir(const char *dir_name){
//     char *sub_dir = gigaStrcat(3, GIT_DIR_NAME,"/", dir_name);
//     int res = mkdir(sub_dir, 0755);
//     free(sub_dir);
//     return res;
// }

// int createGitSubFile(const char *file_name, const char *default_value){
//     char *file_path = gigaStrcat(3, GIT_DIR_NAME, "/", file_name);
//     FILE *file = fopen(file_path, "w");
//     if (file == NULL){
//         return EXIT_FAILURE;
//     }
//     if (default_value){
//         fputs(default_value, file);
//     }
//     fclose(file);
//     free(file_path);
//     return EXIT_SUCCESS;
// }