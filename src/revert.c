#include "include.h"

int GIT_Revert(int argc, char **argv){
    int option;
    bool create_new_commit = true;
    char *commit_msg = NULL;
    char *input_commit = NULL;


    while ((option = getopt(argc, argv, "nm:c:")) != -1)
    {
        if (option == '?' || option == ':') return EXIT_FAILURE;

        if (option == 'n') create_new_commit = false;

        if (option == 'm')  commit_msg = strdup(optarg);

        if (option == 'c') input_commit = strdup(optarg);

    }

    if (!input_commit){
        printError("you should enter the commit with -c option.");
        return EXIT_FAILURE;
    }

    if (!GIT_is_head_attached){
        printError("you cannot do this while your HEAD is not attached!");
        return EXIT_FAILURE;
    }

    char commit_hash[HASH_LEN + 1];
    int step = 0; 

    int count = sscanf(input_commit, "%s-%d", commit_hash, step);

    if (count < 1){
        printError("bad formatted input.");
        return EXIT_FAILURE;
    }

    if (areStringsEqual(commit_hash, "HEAD")){
        if (count == 1){
            step = 0;
        }
        stepBackward(count, commit_hash);       
    }


    if (create_new_commit){
        
        time_t current_time;
        time(&current_time);
        srand(current_time);
        char *new_commit_hash = generateRandomString(HASH_LEN);

        char *new_commit_path = gigaStrcat(7, GIT_parent_dir, "/", GIT_DIR_NAME, "/", COMMITS_DIR, "/", new_commit_hash);

        Commit *old_commit = openCommit(commit_hash);

        Commit new_commit;

        if (!commit_msg){
            commit_msg = old_commit->meta_data.message;
        }

        if (strlen(commit_msg) > MAX_COMMIT_MSG_LEN){
            printError("you picked the wrong len foo.");
            return EXIT_FAILURE;
        }

        strcpy(new_commit.meta_data.message, commit_msg);
        strcpy(new_commit.meta_data.branch, GIT_HEAD_branch->name);
        strcpy(new_commit.meta_data.hash, new_commit_hash);
        strcpy(new_commit.meta_data.user.username, GIT_userdata.username);
        strcpy(new_commit.meta_data.user.email, GIT_userdata.email);
        strcpy(new_commit.meta_data.parents_hash[0], GIT_HEAD_commit_hash);
        strcpy(new_commit.meta_data.parents_hash[1], "");
        new_commit.meta_data.files_count = old_commit->meta_data.files_count;
        new_commit.meta_data.time = current_time;
        
        FILE *new_commit_file = fopen(new_commit_path, "wb");

        if (!new_commit_file){
            printError("could not create the new commit file.");
            exit(EXIT_FAILURE);
        }

        fwrite(&new_commit.meta_data, sizeof(new_commit.meta_data), 1, new_commit_file);

        fwrite(old_commit->files, sizeof(GitFile), old_commit->meta_data.files_count, new_commit_file);

        fclose(new_commit_file);

        loadCommit(new_commit_hash);

        saveGitHead(GIT_HEAD_branch->name, true,  new_commit_hash);

        printfSuccess(("new commit created with hash %s based on commit %s", new_commit_hash, commit_hash));
        free(new_commit_hash);
        free(new_commit_path);

    }else{
        loadCommit(commit_hash);
        printfSuccess(("we are all the way back to commit %s", commit_hash));
    }

    return EXIT_SUCCESS;
    
}