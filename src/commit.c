#include "include.h"


typedef struct {
    char shortcut[MAX_USERNAME_LEN + 1];
    char message[MAX_COMMIT_MSG_LEN + 1];
} MsgShortcut;

MsgShortcut *msg_shortcuts = NULL;
int msg_shortcuts_count = 0;

char *getShortcutMsg(const char *shortcut);
void saveMsgShortcuts();
void loadMsgShortcuts();

int GIT_Commit(int argc, char **argv){

    if (GIT_stagedfiles_count < 1){
        printError("at least stage 1 file bro ://////////////");
        exit(EXIT_FAILURE);
    }


    int option = getopt(argc, argv, "m:s:");

    if (option == -1 || option == '?' || option == ':'){
        printf(COMMIT_COMMAND_USAGE"\n");
        exit(EXIT_FAILURE);
    }

    char *msg;
    if (option == 'm'){
        if (strlen(optarg) > MAX_COMMIT_MSG_LEN){
            printError("you picked the wrong len, foo!");
            return(EXIT_FAILURE);
        }

        msg = optarg;
    }    

    if (option == 's'){
        msg = getShortcutMsg(optarg);
        if (!msg){
            printfError("there is no shortcut called %s", optarg);
            return EXIT_FAILURE;
        }
    }

    

    if (!GIT_is_head_attached){
        printError("bro thinks he has a timetravel machine XD");
        exit(EXIT_FAILURE);
    }

    Branch *current_branch = getBranchByName(GIT_HEAD_branch->name);

    if (!current_branch){
        printfError("branch %s does not exist at all!!", GIT_HEAD_branch->name);
        exit(EXIT_FAILURE);
    }

    time_t current_time;
    time(&current_time);
    srand((unsigned)current_time);
    char *hash = generateRandomString(HASH_LEN);

    Commit commit;
    strcpy(commit.meta_data.message, msg);
    strcpy(commit.meta_data.branch, GIT_HEAD_branch->name); // TODO:
    strcpy(commit.meta_data.hash, hash);
    strcpy(commit.meta_data.user.username, GIT_userdata.username);
    strcpy(commit.meta_data.user.email, GIT_userdata.email);
    strcpy(commit.meta_data.parents_hash[0], current_branch->commit_hash); // git head branch commit name
    strcpy(commit.meta_data.parents_hash[1], "");
    commit.meta_data.time = current_time;
    commit.meta_data.files_count = GIT_stagedfiles_count;

    strcpy(current_branch->commit_hash, hash);

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
    saveGitHead(GIT_HEAD_branch->name, true, hash);
    free(hash);

    printfSuccess(("%s commit created at %s", commit.meta_data.hash, asctime(localtime(&current_time))));

    return EXIT_SUCCESS;

}

Commit *openCommit(const char *hash){
    char *commits_dir_path = gigaStrcat(5, GIT_parent_dir, "/",GIT_DIR_NAME, "/", COMMITS_DIR);
    DIR *commits_dir = opendir(commits_dir_path);

    struct dirent *entry;

    while (entry = readdir(commits_dir))
    {
        if (areStringsEqual(entry->d_name, hash)){
            
            char *commit_path = gigaStrcat(3, commits_dir_path, "/", hash);
            FILE *commit_file = fopen(commit_path, "rb");

            if (!commit_file){
                printError("core exploded!");
                exit(EXIT_FAILURE);
            }

            Commit *commit = malloc(sizeof(Commit));

            if (fread(&(commit->meta_data), sizeof(commit->meta_data), 1, commit_file) < 1){
                printfError("error while loading commit %s: commit file is empty !!!!!", hash);
                exit(EXIT_FAILURE);
            }

            commit->files = malloc(commit->meta_data.files_count * sizeof(GitFile));
            if (fread(commit->files, sizeof(GitFile), commit->meta_data.files_count,  commit_file) < commit->meta_data.files_count){
                printError("commit files are missed ://////?");
                exit(EXIT_FAILURE);
            }

            free(commit_path);           
            closedir(commits_dir);
            return commit;
        }
    }
    
    closedir(commits_dir);
    free(commits_dir_path);

    return NULL;
}

void freeCommit(Commit *commit){
    free(commit->files);
    free(commit);
}


int GIT_Set(int argc, char **argv){

    loadMsgShortcuts();

    char *shortcut = NULL, *message = NULL;

    int option;
    while ((option = getopt(argc, argv, "s:m:")) != -1)
    {
        if (option == '?' || option == ':') return(EXIT_FAILURE);

        if (option == 's') {
            if (strlen(optarg) > MAX_USERNAME_LEN){
                printError("you picked the wrong len foo.");
                exit(EXIT_FAILURE);
            }
            shortcut = strdup(optarg);
        }
        if (option == 'm') {
            if (strlen(optarg) > MAX_COMMIT_MSG_LEN){
                printError("you picked the wrong len foo.");
                exit(EXIT_FAILURE);
            }
            message = strdup(optarg);
        }
    }

    if (!(shortcut && message)){
        printError("one arg is missing :/");
        return(EXIT_FAILURE);
    }

    // check if shortcut exists
    if (getShortcutMsg(shortcut)){
        printfError("there is a shortcut called %s", shortcut);
        return(EXIT_FAILURE);
    }

    // adding shortcut
    msg_shortcuts_count ++;
    msg_shortcuts = realloc(msg_shortcuts, msg_shortcuts_count * sizeof(MsgShortcut));
    strcpy(msg_shortcuts[msg_shortcuts_count - 1].shortcut, shortcut);
    strcpy(msg_shortcuts[msg_shortcuts_count - 1].message, message);

    saveMsgShortcuts();
}


int GIT_Replace(int argc, char **argv){
    loadMsgShortcuts();

    char *shortcut = NULL, *message = NULL;

    int option;
    while ((option = getopt(argc, argv, "s:m:")) != -1)
    {
        if (option == '?' || option == ':') return(EXIT_FAILURE);

        if (option == 's') {
            if (strlen(optarg) > MAX_USERNAME_LEN){
                printError("you picked the wrong len foo.");
                return(EXIT_FAILURE);
            }
            shortcut = strdup(optarg);
        }
        if (option == 'm') {
            if (strlen(optarg) > MAX_COMMIT_MSG_LEN){
                printError("you picked the wrong len foo.");
                return(EXIT_FAILURE);
            }
            message = strdup(optarg);
        }
    }

    if (!(shortcut && message)){
        printError("one arg is missing :/");
        return(EXIT_FAILURE);
    }

    // check if shortcut exists
    char *old_msg = getShortcutMsg(shortcut);
    if (!old_msg){
        printfError("there is no shortcut called %s", shortcut);
        return(EXIT_FAILURE);
    }

    strcpy(old_msg, message);

    saveMsgShortcuts();
}


int GIT_Remove(int argc, char **argv){

    loadMsgShortcuts();

    int option = getopt(argc, argv, "s:");

    if (option == -1 || option == '?' || option == ':'){
        printf(COMMIT_COMMAND_USAGE"\n");
        exit(EXIT_FAILURE);
    }


    for (int i = 0; i < msg_shortcuts_count; i++)
    {
        if (areStringsEqual(optarg, msg_shortcuts[i].shortcut)){
            msg_shortcuts[i].shortcut[0] = -1; // setting a flag to remove it in savemsgshortcut
            saveMsgShortcuts();
            return EXIT_SUCCESS;
        }
    }

    printfError("no msg shortcut called %s", optarg);
    return EXIT_FAILURE;
    

}

void loadMsgShortcuts(){
    char *file_path = gigaStrcat(5, GIT_parent_dir, "/", GIT_DIR_NAME, "/", MSG_SHORTCUTS_FILE_NAME);
    FILE *file = fopen(file_path, "rb");
    free(file_path);

    if (!file){
        printError("could not open msg shortcuts file !!!");
        exit(EXIT_FAILURE);
    }

    msg_shortcuts = malloc(sizeof(MsgShortcut));
    msg_shortcuts_count = 0;

    while (fread(msg_shortcuts + msg_shortcuts_count, sizeof(MsgShortcut), 1, file)  == 1)
    {
        msg_shortcuts_count ++;
        msg_shortcuts = realloc(msg_shortcuts, (msg_shortcuts_count + 1) * sizeof(MsgShortcut));
    }
    
    fclose(file);

}

void saveMsgShortcuts(){
    char *file_path = gigaStrcat(5, GIT_parent_dir, "/", GIT_DIR_NAME, "/", MSG_SHORTCUTS_FILE_NAME);
    FILE *file = fopen(file_path, "wb");
    free(file_path);
    if (!file){
        printError("could not open msg shortcuts file !!!");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < msg_shortcuts_count; i++)
    {
        if (msg_shortcuts[i].shortcut[0] == -1) continue; // my flag
        fwrite(msg_shortcuts + i, sizeof(MsgShortcut), 1, file);
    }
    

    fclose(file);

}

char *getShortcutMsg(const char *shortcut){
    for (int i = 0; i < msg_shortcuts_count; i++)
    {
        if (areStringsEqual(shortcut, msg_shortcuts[i].shortcut)){
            return (char *)msg_shortcuts[i].message;
        }
    }


    return NULL;
    
}