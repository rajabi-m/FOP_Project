#include "include.h"

bool anyNonCommitedChanges();
void loadCommit(const char *hash);
void stepBackward(int count, char *final_hash);

int GIT_Checkout(int argc, char **argv){

    if (anyNonCommitedChanges() && GIT_is_head_attached){
        printError("you have non-commited changes! Commit these changes to  proceed.");
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

bool anyNonCommitedChanges(){
    // this function does not check if any new file is added, it just checks the staged files and if any of them are changed it returns false

    Commit *last_commit = openCommit(GIT_HEAD_commit_hash);

    for (int i = 0; i < last_commit->meta_data.files_count; i++)
    {
        FILE *staged_file = openObject(last_commit->files[i].object_hash, "r");

        if (!staged_file){
            printError("core exploded :(");
            exit(EXIT_FAILURE);
        }

        char *current_file_path = gigaStrcat(3, GIT_parent_dir, "/", last_commit->files[i].path);
        FILE *current_file = fopen(current_file_path, "r");


        if (current_file && (last_commit->files[i].access_code == -1)){
            printfError("%s is added.", last_commit->files[i].path);
            return true;
        }

        if (last_commit->files[i].access_code != -1){
            if (!current_file){
                printfError("%s is deleted.", last_commit->files[i].path);
                return true;
            }
            if ((!areFilesEqual(staged_file, current_file))){
                printfError("%s is changed.", last_commit->files[i].path);
                return true;
            }

            if (last_commit->files[i].access_code != getFileAccessCode(current_file_path)){
                printfError("%s access code is changed.", last_commit->files[i].path);
                return true;
            }
        }

        free(current_file_path);
    }

    freeCommit(last_commit);
    
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

void clearDir();

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
    clearDir();

    // loading new files
    for (int i = 0; i < commit->meta_data.files_count; i++)
    {
        if (commit->files[i].access_code == -1) continue;
        char *dest_path = gigaStrcat(3, GIT_parent_dir, "/", commit->files[i].path);
        char *object_path = gigaStrcat(7, GIT_parent_dir, "/", GIT_DIR_NAME, "/", OBJECTS_DIR, "/", commit->files[i].object_hash);
        if(!copyFile(dest_path, object_path, 10000)){
            printError("error while trying to load commit");
            exit(EXIT_FAILURE);
        }

        chmod(dest_path, commit->files[i].access_code);

        free(dest_path);
        free(object_path);
    }


    freeCommit(commit);

}

void clearDir(){

    Commit *current_commit = openCommit(GIT_HEAD_commit_hash);

    if (!current_commit){
        printError("could not find HEAD in the repository");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < current_commit->meta_data.files_count; i++)
    {
        char *file_path = gigaStrcat(3, GIT_parent_dir, "/", current_commit->files[i].path);
        remove(file_path);
        free(file_path);
    }
    freeCommit(current_commit);

    // removing empty directories
    DIR *directory = opendir(GIT_parent_dir);

    struct  dirent *entry;

    while(entry = readdir(directory)){
        if (areStringsEqual(entry->d_name, ".") || areStringsEqual(entry->d_name, "..") || areStringsEqual(entry->d_name, GIT_DIR_NAME)) continue;
        if(entry->d_type == DT_DIR){
            char *new_path = gigaStrcat(3, GIT_parent_dir, "/", entry->d_name);
            rmdir(new_path);
            free(new_path);
        }
    }
    

    

}