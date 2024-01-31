#include "include.h"

void saveBranchesList();
Branch *getBranchByName(const char *name);

int GIT_Branch(int argc, char **argv){
    if (argc == 2){

        if (GIT_branches_count <= 0){
            printError("no branch found :/");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < GIT_branches_count; i++)
        {
            if (areStringsEqual(GIT_HEAD, GIT_branches_list[i].name)) printf("--> ");
            printf("%s-'%s'\n", GIT_branches_list[i].name, GIT_branches_list[i].commit_hash);
        }
        
        return EXIT_SUCCESS;
    }

    if (strlen(argv[2]) > MAX_BRANCH_NAME_LEN){
        printError("this name is too long for a branch");
        exit(EXIT_FAILURE);
    }

    // check if branch name is used
    if (getBranchByName(argv[2])){
        printError("this branch already exists!!");
        exit(EXIT_FAILURE);
    }

    GIT_branches_count ++;
    GIT_branches_list = realloc(GIT_branches_list, GIT_branches_count * sizeof(Branch));


    strcpy(GIT_branches_list[GIT_branches_count - 1].name, argv[2]);
    strcpy(GIT_branches_list[GIT_branches_count - 1].commit_hash, "headpos"); // TODO: config head

    saveBranchesList();

}

void saveBranchesList(){
    char *branches_file_name = gigaStrcat(5, GIT_parent_dir, "/", GIT_DIR_NAME, "/", BRANCHES_FILE_NAME);

    FILE *branches_file = fopen(branches_file_name, "wb");

    free(branches_file_name);

    if (!branches_file){
        printError("error happened while trying to open branches file!");
        exit(EXIT_FAILURE);
    }

    fwrite(GIT_branches_list, sizeof(Branch), GIT_branches_count, branches_file);

    fclose(branches_file);

}

void loadBranchesList(){
    char *branches_file_name = gigaStrcat(5, GIT_parent_dir, "/", GIT_DIR_NAME, "/", BRANCHES_FILE_NAME);

    FILE *branches_file = fopen(branches_file_name, "rb");

    free(branches_file_name);

    if (!branches_file){
        printError("error happened while trying to open branches file!");
        exit(EXIT_FAILURE);
    }

    GIT_branches_list = malloc(sizeof(Branch));
    GIT_branches_count = 0;

    for (int i = 0; fread(GIT_branches_list + i, sizeof(Branch), 1, branches_file); i++)
    {
        GIT_branches_list = realloc(GIT_branches_list, (i + 2) * sizeof(Branch));
        GIT_branches_count ++;
    }
    


    fclose(branches_file);

}

void loadGitHead(){
    char *head_file_path = gigaStrcat(5, GIT_parent_dir, "/",  GIT_DIR_NAME, "/", HEAD_FILE_NAME);
    FILE *head_file = fopen(head_file_path, "r");
    free(head_file_path);
    if(!head_file){
        printError("could not open git head file");
        exit(EXIT_FAILURE);
    }

    int tmp;

    fscanf(head_file, "%[^,],%d", GIT_HEAD, &tmp);

    GIT_is_head_attached = false;

    if (tmp) GIT_is_head_attached = true;

    fclose(head_file);

}

Branch *getBranchByName(const char *name){
    for (int i = 0; i < GIT_branches_count; i++)
    {
        if (areStringsEqual(name, GIT_branches_list[i].name)){
            return  &GIT_branches_list[i];
        }
    }

    return NULL;
}
