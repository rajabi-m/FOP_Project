#include "include.h"


int GIT_Commit(int argc, char **argv){

    if (GIT_stagedfiles_count < 1){
        printError("at least stage 1 file bro ://////////////");
        exit(EXIT_FAILURE);
    }

    struct option commit_options[] = {
        {"message", required_argument, NULL, 'm'},
        {0, 0 ,0 ,0}
    };

    int option = getopt_long(argc, argv, "m:", commit_options, NULL);

    if (option == -1 || option == '?' || option == ':'){
        printf(COMMIT_COMMAND_USAGE"\n");
        exit(EXIT_FAILURE);
    }

    if (strlen(optarg) > 72){
        printError("you picked the wrong len, foo!");
        exit(EXIT_FAILURE);
    }

    if (!GIT_is_head_attached){
        printError("bro thinks he has a timetravel machine XD");
        exit(EXIT_FAILURE);
    }

    Branch *current_branch = getBranchByName(GIT_HEAD);

    if (!current_branch){
        printfError("branch %s does not exist at all!!", GIT_HEAD);
        exit(EXIT_FAILURE);
    }

    time_t current_time;
    time(&current_time);
    srand((unsigned)current_time);
    char *hash = generateRandomString(HASH_LEN);

    Commit commit;
    strcpy(commit.meta_data.message, optarg);
    strcpy(commit.meta_data.branch, GIT_HEAD); // TODO:
    strcpy(commit.meta_data.hash, hash);
    strcpy(commit.meta_data.user.username, GIT_userdata.username);
    strcpy(commit.meta_data.user.email, GIT_userdata.email);
    strcpy(commit.meta_data.parents_hash[0], current_branch->commit_hash); // git head branch commit name
    strcpy(commit.meta_data.parents_hash[1], "");
    commit.meta_data.time = current_time;
    
    strcpy(current_branch->commit_hash, hash);

    free(hash);
    // dont need to store files in commit struct because we have them is staging area

    char *commit_file_path = gigaStrcat(7, GIT_parent_dir, "/", GIT_DIR_NAME, "/", COMMITS_DIR, "/", commit.meta_data.hash);

    FILE *commit_file = fopen(commit_file_path, "wb");

    if (!commit_file){
        printfError("could not create file %s", commit_file_path);
        exit(EXIT_FAILURE);
    }

    fwrite(&commit.meta_data, sizeof(commit.meta_data), 1, commit_file); // saving metadata

    fwrite(GIT_staging_area, sizeof(GitFile), GIT_stagedfiles_count,  commit_file); // saving files

    fclose(commit_file);
    free(commit_file_path);

    saveBranchesList();

    printfSuccess(("%s commit created at %s", commit.meta_data.hash, asctime(localtime(&current_time))));

    return EXIT_SUCCESS;

}


int GIT_Set(int argc, char **argv){

}


int GIT_Replace(int argc, char **argv){

}


int GIT_Remove(int argc, char **argv){

}