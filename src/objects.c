#include "include.h"

//
bool createNewObject(const char *path, char *object_hash){ // file should be opened in "r" mode
    char *hash = generateRandomString(HASH_LEN);
    strcpy(object_hash, hash);
    free(hash);   
    char *object_path = gigaStrcat(7, GIT_parent_dir, "/", GIT_DIR_NAME, "/", OBJECTS_DIR, "/", object_hash);
    debug(("hash = %s\n", object_hash));
    
    if (copyFile(object_path, path, 1000)){
        free(object_path);

        return true;
    }


    free(object_path);
    return false;
    
}


FILE *openObject(const char *object_hash, const char *mode){
    char *object_path = gigaStrcat(7, GIT_parent_dir, "/", GIT_DIR_NAME, "/", OBJECTS_DIR, "/", object_hash);

    FILE *object = fopen(object_path, mode);

    free(object_path);

    return object;
}