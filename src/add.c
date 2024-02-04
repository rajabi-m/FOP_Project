#include "include.h"

int stageFile(const char *path, bool bug_fixer);
int syncStagingArea();
void checkStageState(const char *dir_path, uint32_t depth, uint32_t current_depth);
void restageStagedFiles();
void pushStageHistory(const char *line);
bool popStageHistory(char *line);

int GIT_Add(int argc, char **argv){

    srand((unsigned) time(NULL)); // seeding for createObject function

    char mode = '0'; // normal mode

    struct option add_options[] = {
        {"multi", no_argument, NULL, 'f'},
        {"state", no_argument, NULL, 'n'},
        {"redo", no_argument, NULL, 'r'},
        {0, 0 ,0 ,0}
    };

    int option = getopt_long(argc, argv, "fnr", add_options, NULL);

    if(option == '?'){
        printError("wrong option");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; add_options[i].name; i++)
    {
        if(option == add_options[i].val){
            mode = add_options[i].val;
            break;
        }
    }
    
    
    switch(mode){
        case '0':
            stageFile(argv[2], true);
            break;
        case 'f':
            for (int i = 3; i < argc; i++)
            {
                stageFile(argv[i], true);
            }
            break;
        case 'n':
            if (argc != 4){
                printError("we need more argument");
                exit(EXIT_FAILURE);
            }
            checkStageState("." ,strtol(argv[3], NULL, 10), 1);
            return EXIT_SUCCESS; // to avoid syncing staging area pointlessly
        case 'r':
            restageStagedFiles();
            break;
        default:
            printError("unknown mode for add");
            exit(EXIT_FAILURE);
    }

    syncStagingArea();


}



int stageFile(const char *path, bool bug_fixer){
    // bug fixer is here to indicate when ever the stage path should be saved in stage.his file or not

    
    

    char *processed_path = processPath(path);

    struct stat statbuf;
    stat(path, &statbuf);

    if (strstr(processed_path, GIT_DIR_NAME)){
        printfError("you cannot stage %s.", GIT_DIR_NAME);
        exit(EXIT_FAILURE);
    }

    if (bug_fixer){
        pushStageHistory(processed_path);
    }

    if (S_ISDIR(statbuf.st_mode)){
        // try to restage all files that has been staged.
        for (int i = 0; i < GIT_stagedfiles_count; i++)
        {
            if (areStringsEqual(processed_path, ".")){
                char *new_path = gigaStrcat(3, GIT_parent_dir, "/", GIT_staging_area[i].path);
                stageFile(new_path, false);
                free(new_path);
                continue;
            }

            char *dir_path = gigaStrcat(2, processed_path, "/");
            if (strstr(GIT_staging_area[i].path, dir_path) == GIT_staging_area[i].path){ // if file is in this dir
                char *new_path = gigaStrcat(3, GIT_parent_dir, "/", GIT_staging_area[i].path);
                stageFile(new_path, false);
                free(new_path);
            }
            free(dir_path);
        }
        // we need to go through all of its files and directories recursively to see if any new file is in there.
        DIR *directory = opendir(path);
        struct dirent *entry;
        while (entry = readdir(directory))
        {
            if (areStringsEqual(entry->d_name, ".") || areStringsEqual(entry->d_name, "..") || areStringsEqual(entry->d_name, GIT_DIR_NAME)) continue;
            char *new_file_path = gigaStrcat(3, processed_path, "/", entry->d_name);
            bool is_new = true;
            for (int i = 0; i < GIT_stagedfiles_count; i++)
            {
                if (areStringsEqual(new_file_path, GIT_staging_area[i].path)){
                    is_new = false;
                    break;
                }
            }
            
            if (is_new){
                char *full_path = gigaStrcat(3, GIT_parent_dir, "/", new_file_path);
                stageFile(full_path, false);
                free(full_path);
            }
            free(new_file_path);

        }
        

        return EXIT_SUCCESS;
        
    }

    FILE *now = fopen(path, "r");

    
    // checking to see if file is already in the staging area
    

    bool is_untracked = true;
    


    for (int i = 0; i < GIT_stagedfiles_count; i++)
    {
        if (areStringsEqual(processed_path, GIT_staging_area[i].path)){
            is_untracked = false;
            
            if (!now){
                GIT_staging_area[i].access_code = -1;
                break;
            }

            FILE *before = openObject(GIT_staging_area[i].object_hash, "r");
            if (!areFilesEqual(before, now)){
                debug(("file %s has been changed...\n", processed_path));
                char object_hash[HASH_LEN + 1];
                createNewObject(path, object_hash);
                strcpy(GIT_staging_area[i].object_hash, object_hash);
                printfSuccess(("file %s is now staged again.", path));
            }
            if (GIT_staging_area[i].access_code != getFileAccessCode(path)){
                GIT_staging_area[i].access_code = getFileAccessCode(path);
                printfSuccess(("the access code of file %s successfully updated.", path));
            }
            fclose(before);
            break;
        }
    }
    
    if (is_untracked){
        if(!now){
            printfError("%s does not exist", path);
            return EXIT_FAILURE;
        }
        GIT_stagedfiles_count ++;
        GIT_staging_area = realloc(GIT_staging_area, GIT_stagedfiles_count *sizeof(GitFile));
        char object_hash[HASH_LEN + 1];
        createNewObject(path, object_hash);
        GIT_staging_area[GIT_stagedfiles_count - 1].access_code = getFileAccessCode(path);
        strcpy(GIT_staging_area[GIT_stagedfiles_count - 1].path, processed_path);
        strcpy(GIT_staging_area[GIT_stagedfiles_count - 1].object_hash, object_hash);
        printfSuccess(("file %s staged successfully!", path));
    }

    free(processed_path);
    if (now) fclose(now);

    
}


int syncStagingArea(){
    char *staging_area_file_path = gigaStrcat(5, GIT_parent_dir, "/", GIT_DIR_NAME, "/", STAGE_FILE_NAME);
    FILE *staging_area_file = fopen(staging_area_file_path, "wb");
    free(staging_area_file_path);

    for (int i = 0; i < GIT_stagedfiles_count; i++)
    {
        char *file_path = gigaStrcat(3, GIT_parent_dir, "/", GIT_staging_area[i].path);
        if (GIT_staging_area[i].access_code != -2){ // if file still exists (-1 access code means that it's removed.)
            fwrite(GIT_staging_area + i, sizeof(GitFile), 1, staging_area_file);
        }else{
            debug(("file %s is removed\n", GIT_staging_area[i].path));
        }
        free(file_path);
    }
    


    fclose(staging_area_file);
    return EXIT_SUCCESS;
    
}

int loadStagingArea(){ 
    // Load the staging area into memory for further processing
    char *staging_area_file_path = gigaStrcat(5, GIT_parent_dir, "/", GIT_DIR_NAME, "/", STAGE_FILE_NAME);
    FILE *staging_area_file = fopen(staging_area_file_path, "rb");
    free(staging_area_file_path);

    if (staging_area_file == NULL) {
        printError("404 no staging area file found...");
        exit(EXIT_FAILURE);
    }

    int index;


    
    GIT_staging_area = malloc(sizeof(GitFile));


    for (index = 0;fread(GIT_staging_area + index, sizeof(GitFile), 1, staging_area_file); index++)
    {
        GIT_staging_area = realloc(GIT_staging_area, (index + 2) * sizeof(GitFile));
    }

    GIT_stagedfiles_count = index;    
    
    fclose(staging_area_file);


    for (int i = 0; i < GIT_stagedfiles_count; i++)
    {
        debug(("path = %s\naccess = %d\nhash = %s\n--------\n", GIT_staging_area[i].path, GIT_staging_area[i].access_code, GIT_staging_area[i].object_hash));
    }
    


    return EXIT_SUCCESS;
}



void checkStageState(const char *dir_path, uint32_t depth, uint32_t current_depth){

    if (current_depth > depth) return;

    DIR *directory = opendir(dir_path);

    if (!directory){
        printfError("wrong path : %s", dir_path);
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while (entry = readdir(directory))
    {
        if (areStringsEqual(entry->d_name, ".") || areStringsEqual(entry->d_name, "..") || areStringsEqual(entry->d_name, GIT_DIR_NAME)) continue;
        
        if(entry->d_type == DT_DIR){ // if it is a dir
            char *new_path = gigaStrcat(3, dir_path, "/", entry->d_name);
            checkStageState(new_path, depth, current_depth + 1);
            free(new_path);
        }else{ // if it is a file
            char *file_path = gigaStrcat(3, dir_path, "/", entry->d_name);
            char *processed_path = processPath(file_path);
            free(file_path);

            bool staged = false;
            for (int i = 0; i < GIT_stagedfiles_count; i++)
            {
                if(areStringsEqual(processed_path, GIT_staging_area[i].path)){
                    printf(GRN_TEXT " TRACKED : " RESET_TEXT "%s\n", processed_path);
                    staged = true;
                    break;
                }
            }

            if (!staged){
                printf(YEL_TEXT " UNTRACKED : " RESET_TEXT "%s\n", processed_path);
            }
        

        }
    }
    

}

void restageStagedFiles(){

    for (int i = 0; i < GIT_stagedfiles_count; i++)
    {
        char *file_path = gigaStrcat(3, GIT_parent_dir, "/", GIT_staging_area[i].path);
        stageFile(file_path, false);
        free(file_path);
    }
    
}


// RESET COMMAND --------------------------------------------------------------------------------------
int unstageFile(const char *file_path);
void redoLastStage();


int GIT_Reset(int argc, char **argv){
    char mode = '0'; // normal mode

    struct option reset_options[] = {
        {"multi", no_argument, NULL, 'f'},
        {"undo", no_argument, NULL, 'r'},
        {0, 0 ,0 ,0}
    };

    int option = getopt_long(argc, argv, "fr", reset_options, NULL);

    if(option == '?'){
        printError("wrong option");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; reset_options[i].name; i++)
    {
        if(option == reset_options[i].val){
            mode = reset_options[i].val;
            break;
        }
    }

    switch (mode){
        case '0':
            unstageFile(argv[2]);
            break;
        case 'f':
            for (int i = 3; i < argc; i++)
            {
                unstageFile(argv[i]);
            }
            break;
        case 'r':
            redoLastStage();
            break;
        default:
            printError("unknown mode for reset command");
            exit(EXIT_FAILURE);
    }

    syncStagingArea();

    return EXIT_SUCCESS;

}


int unstageFile(const char *file_path){
    
    struct stat statbuf;
    stat(file_path, &statbuf);

    char *processed_path = processPath(file_path);
    if (S_ISDIR(statbuf.st_mode)){

        for (int i = 0; i < GIT_stagedfiles_count; i++)
        {
            char *dir_path = gigaStrcat(2, file_path, "/");
            if (strstr(GIT_staging_area[i].path, dir_path) == GIT_staging_area[i].path){
                char *full_path = gigaStrcat(3, GIT_parent_dir, "/", GIT_staging_area[i].path);
                unstageFile(full_path);
                free(full_path);
            }
            free(dir_path);
        }
        

        DIR *directory = opendir(file_path);
        struct dirent *entry;
        while (entry = readdir(directory))
        {
            if (areStringsEqual(entry->d_name, ".") || areStringsEqual(entry->d_name, "..") || areStringsEqual(entry->d_name, GIT_DIR_NAME)) continue;
            char *full_path = gigaStrcat(3, processed_path, "/",  entry->d_name);
            bool is_new = true;
            for (int i = 0; i < GIT_stagedfiles_count; i++)
            {
                if (areStringsEqual(GIT_staging_area[i].path, full_path)){
                    is_new = false;
                    break;
                }
            }

            if (is_new){
                char *file_path = gigaStrcat(3, GIT_parent_dir, "/", full_path);
                unstageFile(file_path);
                free(file_path);
            }
            
            free(full_path);
        }
        return EXIT_SUCCESS;
        
    }

    Commit *last_commit = openCommit(GIT_HEAD_commit_hash);

    for (int i = 0; i < GIT_stagedfiles_count; i++)
    {
        if (areStringsEqual(processed_path, GIT_staging_area[i].path)){
            if(!last_commit){ // so we are at the start
                GIT_staging_area[i].access_code = -2; // means it should be removed
            }else{
                bool found = false;
                for (int j = 0; j < last_commit->meta_data.files_count; j++)
                {
                    if (areStringsEqual(last_commit->files[j].path, processed_path)){
                        memcpy(&GIT_staging_area[i], &last_commit->files[j] , sizeof(GitFile));
                        found = true;
                        break;
                    }
                }

                if (!found){
                    GIT_staging_area[i].access_code = -2; // means it should be removed
                }

                freeCommit(last_commit);
                
            }
            free(processed_path);
            printfSuccess(("file %s is not staged anymore.", file_path));
            return EXIT_SUCCESS;
        }
    }
    
    printfError("file %s is not staged at all", file_path);
    free(processed_path);
    return EXIT_FAILURE;
}

void redoLastStage(){
    char line[MAX_PATH_LEN];
    line[0] = '\0';

    if (!popStageHistory(line)){
        printError("stage history is empty.");
        return;
    }

    char *full_path = gigaStrcat(3, GIT_parent_dir, "/", line);

    unstageFile(full_path);

    free(full_path);

}

void pushStageHistory(const char *line){
    char *stage_history_file_path = gigaStrcat(5, GIT_parent_dir, "/", GIT_DIR_NAME, "/", STAGE_HISTORY_FILE_NAME);
    FILE *stage_history_file = fopen(stage_history_file_path, "a");
    free(stage_history_file_path);

    char *full_line = gigaStrcat(2, line, "\n");
    fputs(full_line, stage_history_file);
    free(full_line);
    fclose(stage_history_file);

}

bool popStageHistory(char *line){
    char *stage_history_file_path = gigaStrcat(5, GIT_parent_dir, "/", GIT_DIR_NAME, "/", STAGE_HISTORY_FILE_NAME);

    char *command = gigaStrcat(3, "tail -n 1 \"", stage_history_file_path, "\"");
    FILE *bash_stream = popen(command, "r");
    free(command);
    char tmp_line[MAX_PATH_LEN];
    if (fgets(tmp_line, MAX_PATH_LEN, bash_stream) == NULL){
        pclose(bash_stream);
        return false;
    }
    pclose(bash_stream);

    tmp_line[strcspn(tmp_line, "\n")] = '\0'; // removing \n

    strcpy(line, tmp_line);

    // removing last line from stage history

    command = gigaStrcat(3, "sed -i '$ d' \"", stage_history_file_path, "\"");
    system(command);
    free(command);

    free(stage_history_file_path);

    return true;
}