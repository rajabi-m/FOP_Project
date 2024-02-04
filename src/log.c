#include "include.h"

bool logFilter_BRANCH(Commit *commit, const char *arg);
bool logFilter_AUTHOR(Commit *commit, const char *arg);
bool logFilter_SINCE(Commit *commit, const char *arg);
bool logFilter_BEFORE(Commit *commit, const char *arg);
bool logFilter_SEARCH(Commit *commit, const char *arg);
void printCommit(Commit *commit);

struct filer_t{
    char code;
    bool enabled;
    char *arg;
    bool (*check_function)(Commit*, const char *arg);
} filters[] = {
    {'b', false, NULL, logFilter_BRANCH},
    {'a', false, NULL, logFilter_AUTHOR},
    {'s', false, NULL, logFilter_SINCE},
    {'f', false, NULL, logFilter_BEFORE},
    {'h', false, NULL, logFilter_SEARCH},
    {0, 0, 0, 0}
};


int GIT_Log(int argc, char **argv){

    struct option log_options[] = {
        {"count", required_argument, NULL, 'n'},
        {"branch", required_argument, NULL, 'b'},
        {"author", required_argument, NULL, 'a'},
        {"since", required_argument, NULL, 's'},
        {"before", required_argument, NULL, 'f'},
        {"search", required_argument, NULL, 'h'},
        {0, 0, 0, 0}
    };

    int count = INT32_MAX;

    int option;

    while ((option = getopt_long(argc, argv, "n:b:a:s:f:h:", log_options, NULL)) != -1)
    {
        if (option == ':') exit(EXIT_FAILURE); // missed argument
        if (option == '?') exit(EXIT_FAILURE); // wrong option

        if(option == 'n'){
            count = strtol(optarg, NULL, 10);
            continue;
        }

        for (int i = 0; filters[i].code; i++)
        {
            if (option == filters[i].code){
                filters[i].enabled = true;
                filters[i].arg = strdup(optarg);
                break;
            }
        }
        
    }

    // printing stuff
    char *command = gigaStrcat(7, "ls -t \"", GIT_parent_dir, "/", GIT_DIR_NAME, "/", COMMITS_DIR, "\"");
    FILE *terminal = popen(command, "r");
    free(command);

    int commit_count = 0;
    char **commit_list = malloc(sizeof(char *));
    commit_list[0] = malloc(HASH_LEN + 1);

    for (commit_count = 0; fscanf(terminal, "%s", commit_list[commit_count]) != EOF; commit_count++)
    {
        commit_list = realloc(commit_list, (commit_count + 2) * sizeof(char *));
        commit_list[commit_count + 1] = malloc(HASH_LEN + 1);
    }
    
    for (int j = 0; j < commit_count && count != 0; j++)
    {

        Commit *commit = openCommit(commit_list[j]);

        bool is_good = true;
        for (int i = 0; filters[i].code; i++)
        {
            if(filters[i].enabled) is_good &= filters[i].check_function(commit, filters[i].arg);
        }

        if (is_good){
            printCommit(commit);
            count --;
        }
        freeCommit(commit);
    }

}

void printCommit(Commit *commit){
    printf(CYN_TEXT"󰜘 Commit %s"RESET_TEXT, commit->meta_data.hash);
    
    // checking if HEAD is here
    if (areStringsEqual(GIT_HEAD_commit_hash,  commit->meta_data.hash)){
        printf(MAG_TEXT" (  HEAD)"RESET_TEXT);
    }
    // checking if any branch is here
    for (int i = 0; i < GIT_branches_count; i++)
    {
        if (areStringsEqual(GIT_branches_list[i].commit_hash, commit->meta_data.hash)){
            printf( BLU_TEXT" ( %s)"RESET_TEXT,  GIT_branches_list[i].name);
        }
    }
    printf("\n");
    printf(WHT_TEXT"     branch: %s\n", commit->meta_data.branch);
    printf("    󰚢 message: %s\n", commit->meta_data.message);
    printf("    󰆙 filecount: %d\n", commit->meta_data.files_count);
    printf("    by  %s<%s> on  %s\n"RESET_TEXT, commit->meta_data.user.username, commit->meta_data.user.email, asctime(localtime(&commit->meta_data.time)));
}

bool logFilter_BRANCH(Commit *commit, const char *arg){

    if (!getBranchByName(arg)){
        printfError("there is no branch called %s", arg);
        exit(EXIT_FAILURE);
    }

    if (areStringsEqual(commit->meta_data.branch, arg)){
        return true;
    }
    return false;
}

bool logFilter_AUTHOR(Commit *commit, const char *arg){
    if (areStringsEqual(commit->meta_data.user.username, arg)){
        return true;
    }
    return false;
}

bool logFilter_SINCE(Commit *commit, const char *arg){
    struct tm tm;
    time_t target_date;
    if ( strptime(arg, "%Y-%m-%d %H:%M:%S", &tm) != NULL)
        target_date = mktime(&tm);
    else{
        printError("please enter time in format of YYYY-mm-dd HH:MM:SS");
        exit(EXIT_FAILURE);
    }

    if (difftime(commit->meta_data.time, target_date) > 0) return true;

    return false;
        
}

bool logFilter_BEFORE(Commit *commit, const char *arg){
    struct tm tm;
    time_t target_date;
    if ( strptime(arg, "%Y-%m-%d %H:%M:%S", &tm) != NULL)
        target_date = mktime(&tm);
    else{
        printError("please enter time in format of YYYY-mm-dd HH:MM:SS");
        exit(EXIT_FAILURE);
    }

    if (difftime(commit->meta_data.time, target_date) < 0) return true;

    return false;
        
}

bool logFilter_SEARCH(Commit *commit, const char *arg){
    TokenArray search_words = gigaStrtok(arg, "-");
    TokenArray msg_words = gigaStrtok(commit->meta_data.message, " ");

    for (int i = 0; i < search_words.count; i++)
    {
        for (int j = 0; j < msg_words.count; j++)
        {
            if(wildcardMatch(search_words.tokens[i], msg_words.tokens[j])){
                freeTokens(&search_words);
                freeTokens(&msg_words);
                return true;
            }
        }
        
    }
    
    freeTokens(&search_words);
    freeTokens(&msg_words);
    return false;

}

