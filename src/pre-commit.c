#include "include.h"

bool HOOK_CharacterLimit(GitFile *git_file);
bool HOOK_FileSizeCheck(GitFile *git_file);
bool HOOK_FormatCheck(GitFile *git_file);
bool HOOK_TimeLimit(GitFile *git_file);

struct{
    const char *name;
    bool enabled;
    bool (*function)(GitFile *);
} hooks[] = {
    {"format-check", false, HOOK_FormatCheck},
    {"file-size-check", false, HOOK_FileSizeCheck},
    {"character-limit", false, HOOK_CharacterLimit},
    {"time-limit", false, HOOK_TimeLimit},
    {0, 0, 0}
};


bool checkHooks(GitFile *git_file, bool print_result);
void loadHooks();
void saveHooks();

int GIT_PreCommit(int argc, char **argv){
    struct option pre_commit_options[] = {
        {"list", no_argument, NULL, 'l'},
        {"applied", no_argument, NULL, 'p'},
        {"add", required_argument, NULL, 'a'},
        {"remove", required_argument, NULL, 'r'},
        {"file", required_argument, NULL, 'f'},
        {0 ,0, 0, 0}
    };

    char mode = (char)getopt_long(argc, argv, "lpaa:r:f:",  pre_commit_options, NULL);

    if (mode == ':' || mode == '?') return EXIT_FAILURE;

    loadHooks();

    switch(mode){
        case -1: // normal mode
            for (int i = 0; i < GIT_stagedfiles_count; i++)
            {
                if (GIT_staging_area[i].access_code == -1) continue;
                checkHooks(&GIT_staging_area[i], true);
            }
            break;
        case 'l':
            for (int i = 0; hooks[i].name; i++)
            {
                printf("%s\n", hooks[i].name);
            }
            break;
        case 'p':
            for (int i = 0; hooks[i].name; i++)
            {
                if (hooks[i].enabled) printf("%s\n", hooks[i].name);
            }
            break;
        case 'a':
            bool found = false;
            for (int i = 0; hooks[i].name; i++)
            {
                if (areStringsEqual(hooks[i].name, optarg)){
                    hooks[i].enabled = true;
                    saveHooks();
                    found = true;
                    break;
                }
            }
            if (!found) {
                printfError("there is no hook called %s.", optarg);
            }
            break;
        case 'r':
            found = false;
            for (int i = 0; hooks[i].name; i++)
            {
                if (areStringsEqual(hooks[i].name, optarg)){
                    hooks[i].enabled = false;
                    saveHooks();
                    found = true;
                    break;
                }
            }
            if (!found) {
                printfError("there is no hook called %s.", optarg);
            }
            break;
        case 'f':
            for (int i = 3; i < argc; i++)
            {
                bool found = false;
                char *processed_path = processPath(argv[i]);
                for (int i = 0; i < GIT_stagedfiles_count; i++)
                {
                    if (areStringsEqual(processed_path, GIT_staging_area[i].path)){
                        checkHooks(&GIT_staging_area[i], true);
                        found = true;
                        break;
                    }
                }
                
                if (!found) printfError("there is no file called %s in staging area.", argv[i]);

            }
            break;
            
            
    }    

}

bool checkHooks(GitFile *git_file, bool print_result){
    bool res = true;
    for (int i = 0; hooks[i].name; i++)
    {
        if (!hooks[i].enabled) continue;
        bool result = hooks[i].function(git_file);
        res &= result;

        if (print_result){
            printf("%s  -  %s - %d\n", git_file->path, hooks[i].name, result);
        }

    }
    
    return res;
}

bool HOOK_CharacterLimit(GitFile *git_file){

    char *file_ext = strrchr(git_file->path, '.');

    if (!file_ext) return true;

    file_ext ++;

    if(!(areStringsEqual(file_ext, "txt") || areStringsEqual(file_ext, "c") || areStringsEqual(file_ext, "cpp"))) return true;

    FILE *file = openObject(git_file->object_hash, "r");
    if (!file){
        printError("core exploded! no such object.");
        return false;
    }

    int count = 0;
    while (fgetc(file) != EOF)
    {
        count ++;
    }
    fclose(file);
    

    return count <= 20000;

}

bool HOOK_FileSizeCheck(GitFile *git_file){
    char *object_path = gigaStrcat(7, GIT_parent_dir, "/", GIT_DIR_NAME, "/", OBJECTS_DIR, "/", git_file->object_hash);
    struct stat st;
    if (stat(object_path, &st) == -1){
        printError("wrong object hash!");
        return false;
    }

    return st.st_size <= 5000000;
}

bool HOOK_FormatCheck(GitFile *git_file){
    char *file_ext = strrchr(git_file->path, '.');
    if (!file_ext) return false;
    file_ext ++;

    char *valid_exts[] = {"c", "cpp", "txt", "mp4", "wav", "mp3", "py", 0};

    for (int i = 0; valid_exts[i]; i++)
    {
        if (areStringsEqual(file_ext, valid_exts[i])) return true;
    }
    
    return false;
}

bool HOOK_TimeLimit(GitFile *git_file){
    return true; // TODO:
}

void loadHooks(){
    char *hooks_file_path = gigaStrcat(5, GIT_parent_dir, "/", GIT_DIR_NAME, "/", HOOKS_FILE_NAME);
    FILE *hooks_file = fopen(hooks_file_path, "r");
    free(hooks_file_path);
    if (!hooks_file){
        printError("no hook file!!");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; hooks[i].name; i++)
    {
        fscanf(hooks_file, "%u,", &hooks[i].enabled);
    }
    
    fclose(hooks_file);
}

void saveHooks(){
    char *hooks_file_path = gigaStrcat(5, GIT_parent_dir, "/", GIT_DIR_NAME, "/", HOOKS_FILE_NAME);
    FILE *hooks_file = fopen(hooks_file_path, "w");
    free(hooks_file_path);
    if (!hooks_file){
        printError("no hook file!!");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; hooks[i].name; i++)
    {
        fprintf(hooks_file, "%u,", hooks[i].enabled);
    }
    
    fclose(hooks_file);

}