#include "include.h"



int stageFile(const char *path);
int syncStagingArea();
void checkStageState(const char *dir_path, uint32_t depth, uint32_t current_depth);

int GIT_Add(int argc, char **argv){
    if (!GIT_parent_dir){
        printError("you sould be in a giga-git dir t obe able to do this !");
        exit(EXIT_FAILURE);
    }

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
            stageFile(argv[2]);
            break;
        case 'f':
            for (int i = 3; i < argc; i++)
            {
                stageFile(argv[i]);
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
            // restageStagedFiles(); // TODO:
            break;
        default:
            printError("unknown mode for add");
            exit(EXIT_FAILURE);
    }

    syncStagingArea();


}



int stageFile(const char *path){

    // add capability to not be able to stage .giga-git TODO:

    struct stat statbuf;
    if (stat(path, &statbuf) == -1){
        printfError("path %s is probebly wrong.", path);
        return EXIT_FAILURE;
    }

    if (S_ISDIR(statbuf.st_mode)){
        DIR *directory = opendir(path);
        struct dirent *entry;
        while (entry = readdir(directory))
        {
            if (areStringsEqual(entry->d_name, ".") || areStringsEqual(entry->d_name, "..") || areStringsEqual(entry->d_name, GIT_DIR_NAME)) continue;
            char *new_file_name = gigaStrcat(3, path, "/", entry->d_name);
            stageFile(new_file_name);
            free(new_file_name);
        }
        return EXIT_SUCCESS;
        
    }

    FILE *now = fopen(path, "r");
    if(!now){
        printfError("file %s does not exist", path);
        return EXIT_FAILURE;
    }

    char *proccesed_path = processPath(path);
    
    // checking to see if file is already in the staging area
    

    bool is_untracked = true;

    for (int i = 0; i < GIT_stagedfiles_count; i++)
    {
        if (areStringsEqual(proccesed_path, GIT_staging_area[i].path)){
            
            FILE *before = openObject(GIT_staging_area[i].object_hash, "r");
            if (!areFilesEqual(before, now)){
                debug(("file %s has been changed...\n", proccesed_path));
                char object_hash[HASH_LEN + 1];
                createNewObject(path, object_hash);
                strcpy(GIT_staging_area[i].object_hash, object_hash);
            }
            GIT_staging_area[i].access_code = getFileAccessCode(path);
            fclose(before);
            is_untracked = false;
            break;
        }
    }
    
    if (is_untracked){
        GIT_stagedfiles_count ++;
        GIT_staging_area = realloc(GIT_staging_area, GIT_stagedfiles_count *sizeof(GitFile));
        char object_hash[HASH_LEN + 1];
        createNewObject(path, object_hash);
        GIT_staging_area[GIT_stagedfiles_count - 1].access_code = getFileAccessCode(path);
        strcpy(GIT_staging_area[GIT_stagedfiles_count - 1].path, proccesed_path);
        strcpy(GIT_staging_area[GIT_stagedfiles_count - 1].object_hash, object_hash);
    }

    printfSuccess(("file %s staged successfuly!", path));
    free(proccesed_path);
    fclose(now);

    
}


int syncStagingArea(){
    char *staging_area_file_path = gigaStrcat(5, GIT_parent_dir, "/", GIT_DIR_NAME, "/", STAGE_FILE_NAME);
    FILE *staging_area_file = fopen(staging_area_file_path, "wb");
    free(staging_area_file_path);


    for (int i = 0; i < GIT_stagedfiles_count; i++)
    {
        char *file_path = gigaStrcat(3, GIT_parent_dir, "/", GIT_staging_area[i].path);
        if (access(file_path, F_OK) == 0){ // if file still exists
            fwrite(GIT_staging_area + i, sizeof(GitFile), 1, staging_area_file);
        }else{
            debug(("file %s is removed\n", GIT_staging_area[i].path));
        }
        free(file_path);
    }
    

    // fwrite(GIT_staging_area, sizeof(GitFile), GIT_stagedfiles_count, staging_area_file);

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
                    printf(GRN_TEXT "STAGED : " RESET_TEXT "%s\n", processed_path);
                    staged = true;
                    break;
                }
            }

            if (!staged){
                printf(YEL_TEXT "UNSTAGED : " RESET_TEXT "%s\n", processed_path);
            }
        

        }
    }
    

}