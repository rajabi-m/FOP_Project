#include "include.h"

typedef enum{
    file_is_added = 0,
    content_is_changed = 1,
    acsscode_is_changed = 2,
    file_is_removed = 3
} FileStatus;


void printStatus(const char *dir, bool *checklist);
void printStatusLine(const char *file, bool staged, FileStatus file_status);

int GIT_Status(int argc, char **argv){

    bool *checklist = calloc(GIT_stagedfiles_count, sizeof(bool));
    printStatus(GIT_parent_dir, checklist);

    
    // handling check list
    for (int i = 0; i < GIT_stagedfiles_count; i++)
    {
        if (!checklist[i]){
            if (GIT_staging_area[i].access_code != -1)
                printStatusLine(GIT_staging_area[i].path, true, file_is_removed);
        }
    }
    


    free(checklist);

    return EXIT_SUCCESS;
}


void printStatus(const char *dir_path, bool *checklist){
    
    DIR *directory = opendir(dir_path);

    if (!directory){
        printfError("wrong path : %s", dir_path);
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;

    while (entry = readdir(directory))
    {
        if (areStringsEqual(entry->d_name, ".") || areStringsEqual(entry->d_name, "..") || areStringsEqual(entry->d_name, GIT_DIR_NAME)) continue;

        if (entry->d_type == DT_DIR){
            char *new_dir = gigaStrcat(3, dir_path, "/", entry->d_name);
            printStatus(new_dir, checklist);
            free(new_dir);
        }else{
            char *file_path = gigaStrcat(3, dir_path, "/", entry->d_name);
            char *processed_path = processPath(file_path);
            
            bool is_staged = false;
            for (int i = 0; i < GIT_stagedfiles_count; i++)
            {
                if (areStringsEqual(processed_path, GIT_staging_area[i].path)){ // file is staged
                    checklist[i] = true;
                    FILE *object = openObject(GIT_staging_area[i].object_hash, "r");
                    FILE *file = fopen(file_path, "r");
                    if (GIT_staging_area[i].access_code == -1){
                        printStatusLine(processed_path, true, file_is_added);
                    }else if (!areFilesEqual(file, object)){
                        printStatusLine(processed_path, true, content_is_changed);
                    }else if (getFileAccessCode(file_path) != GIT_staging_area[i].access_code){
                        printStatusLine(processed_path, true, acsscode_is_changed);
                    }
                    is_staged = true;
                    fclose(object);
                    fclose(file);
                    break;
                }
            }

            if (!is_staged) printStatusLine(processed_path, false, file_is_added);
             
            free(file_path);
            free(processed_path);
        }

    }
        

}

void printStatusLine(const char *file, bool staged, FileStatus file_status){
    char mode_char;
    char stage_char[4];

    if (staged) {strcpy(stage_char, ""); printf(YEL_TEXT);}
    else        {strcpy(stage_char, ""); printf(RED_TEXT);}

    switch (file_status){
        case content_is_changed:{
            mode_char = 'M';
            break;
        }
        case file_is_added:{
            mode_char = 'A';
            break;
        }
        case file_is_removed:{
            mode_char = 'D';
            break;
        }
        case acsscode_is_changed:{
            mode_char = 'T';
            break;
        }
    }
    


    printf("%s %c  %s\n"RESET_TEXT, stage_char, mode_char, file);

}
