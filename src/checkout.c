#include "include.h"

bool anyNonStagedGhanges();
void loadCommit(const char *hash);
void stepBackward(int count, char *final_hash);

int GIT_Checkout(int argc, char **argv){

    if (anyNonStagedGhanges()){
        printError("you have non-staged changes! please fix this using add command.");
        exit(EXIT_FAILURE);
    }

    // we are sure that argc is 3 because of areArgsValid Functions

    char name[((MAX_BRANCH_NAME_LEN > HASH_LEN) ? MAX_BRANCH_NAME_LEN : HASH_LEN) + 1]; // i know its long but its very good
    int step;

    int count = sscanf(argv[2], "%[^-]-%d", name, &step);

    if (areStringsEqual(name, "HEAD")){
        if (count == 1){

            // check if head is attached
            if (GIT_is_head_attached){
                printError("you are already here!");
                exit(EXIT_FAILURE);
            }

            step = 0;
        }

        char final_commit_hash[HASH_LEN + 1];
        stepBackward(step, final_commit_hash);
        loadCommit(final_commit_hash);
        saveGitHead(GIT_HEAD_branch->name, false, final_commit_hash);
        printfSuccess(("we are now on Commit %s", final_commit_hash));
        return EXIT_SUCCESS;
    }

    Branch *dest_branch = getBranchByName(name);

    if (dest_branch){
        if (areStringsEqual(dest_branch->commit_hash, "")){
            printfError("%s is not attached to any commit right now", name);
            return EXIT_FAILURE;
        }

        loadCommit(dest_branch->commit_hash);

        saveGitHead(dest_branch->name, true, dest_branch->commit_hash);
        printfSuccess(("we are now on Branch %s", dest_branch->name));
        return EXIT_SUCCESS;
    }


    // so name is a commit hash by itself
    loadCommit(name);
    saveGitHead(GIT_HEAD_branch->name, false, name);
    printfSuccess(("we are now on Commit %s", name));
    return EXIT_SUCCESS;
}

bool anyNonStagedGhanges(){
    // this function does not check if any new file is added, it just checks the stagged files and if any of them are changed it returns false

    for (int i = 0; i < GIT_stagedfiles_count; i++)
    {
        FILE *staged_file = openObject(GIT_staging_area[i].object_hash, "r");

        if (!staged_file){
            printError("core exploded :(");
            exit(EXIT_FAILURE);
        }

        char *current_file_path = gigaStrcat(3, GIT_parent_dir, "/", GIT_staging_area[i].path);
        FILE *current_file = fopen(current_file_path, "r");

        if (!current_file){
            printfError("%s is deleted.", GIT_staging_area[i].path);
            return true;
        }

        if (!areFilesEqual(staged_file, current_file)){
            printfError("%s is changed.", GIT_staging_area[i].path);
            return true;
        }

        if (GIT_staging_area[i].access_code != getFileAccessCode(current_file_path)){
            printfError("%s access code is changed.", GIT_staging_area[i].path);
            return true;
        }

        free(current_file_path);
    }
    
    return false;

}

void stepBackward(int count, char *final_hash){
    char *current_commit_hash;


    current_commit_hash = strdup(GIT_HEAD_commit_hash);

    while (count > 0)
    {
        Commit *commit = openCommit(current_commit_hash);
        if (!commit){
            printError("step count is too much :/");
            exit(EXIT_FAILURE);
        }
        free(current_commit_hash);

        current_commit_hash = strdup(commit->meta_data.parents_hash[0]);
        freeCommit(commit);

        count --;
    }

    strcpy(final_hash, current_commit_hash);

}

void clearDir(const char *dir_path);

void loadCommit(const char *hash){


    Commit *commit = openCommit(hash);

    if (!commit){
        printfError("commit %s does not exist at all", hash);
        exit(EXIT_FAILURE);
    }

    // copy commit to staging area
    char *staging_area_file_path = gigaStrcat(5, GIT_parent_dir, "/", GIT_DIR_NAME, "/", STAGE_FILE_NAME);
    FILE *staging_area_file = fopen(staging_area_file_path, "wb");
    fwrite(commit->files, sizeof(GitFile), commit->meta_data.files_count, staging_area_file);
    fclose(staging_area_file);
    free(staging_area_file_path);
    // clear working dir
    clearDir(GIT_parent_dir);

    // loading new files
    for (int i = 0; i < commit->meta_data.files_count; i++)
    {
        char *dest_path = gigaStrcat(3, GIT_parent_dir, "/", commit->files[i].path);
        char *object_path = gigaStrcat(7, GIT_parent_dir, "/", GIT_DIR_NAME, "/", OBJECTS_DIR, "/", commit->files[i].object_hash);
        if(!copyFile(dest_path, object_path, 10000)){
            printError("error while trying to load commit");
            exit(EXIT_FAILURE);
        }
        free(dest_path);
        free(object_path);
    }


    freeCommit(commit);

}

void clearDir(const char *dir_path){
    DIR *directory = opendir(dir_path);

    if (!directory){
        printfError("error while trying to delete dir %s", dir_path);
        exit(EXIT_FAILURE);
    }

    struct  dirent *entry;

    while(entry = readdir(directory)){
        if (areStringsEqual(entry->d_name, ".") || areStringsEqual(entry->d_name, "..") || areStringsEqual(entry->d_name, GIT_DIR_NAME)) continue;
        char *new_path = gigaStrcat(3, dir_path, "/", entry->d_name);
        if(entry->d_type == DT_DIR){
            clearDir(new_path);
        }else{ // so it is a file
            remove(new_path);
        }
        free(new_path);
    }

}