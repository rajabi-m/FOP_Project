#include "include.h"

int changeUserData(const char *variable, const char *value, bool is_global);
int createAlias(const char *name, const char *command, const bool is_global);


int GIT_Config(int argc, char *argv[]){
    bool global = false;
    int correct_arg_count = 4;

    // option handling --> https://linux.die.net/man/3/getopt_long
    struct option config_options[] = {
        {"global", no_argument, NULL, 'g'},
        {0, 0 ,0 ,0}
    };

    int option = getopt_long(argc, argv, "g", config_options, NULL);

    if (option != -1){
        correct_arg_count ++;
        if (option == 'g'){
            global = true;
        }else{
            return EXIT_FAILURE;
        }
    }

    // validation
    if (argc != correct_arg_count){
        printError("what the hell is this amount of args");
        return EXIT_FAILURE;
    }

    if ((!global) && (!GIT_parent_dir)){
        printError("you should be in a giga git dir to be able to change stuff locally!");
        return EXIT_FAILURE;
    }

    // getting cat and val
    char category[10];
    char variable[10];
    sscanf(argv[correct_arg_count - 2], "%[^.].%s", category, variable);

    if (areStringsEqual(category, "user")){
        return changeUserData(variable, argv[correct_arg_count - 1], global);
    }else if (areStringsEqual(category, "alias")){
        return createAlias(variable, argv[correct_arg_count - 1], global);
    }else{
        printError("there is no such a category in configs :/");
        exit(EXIT_FAILURE);
    }

    

    return EXIT_SUCCESS;
}


int changeUserData(const char *variable, const char *value, const bool is_global){
    // finding user data file path and oppenning it
    char user_data_file_path[MAX_PATH_LEN];
    if (is_global){
        sprintf(user_data_file_path, "%s/%s", GLOBAL_CONFIG_DIR, USERDATA_CONFIG_FILE);
    }else{
        sprintf(user_data_file_path, "%s/%s/%s/%s", GIT_parent_dir, GIT_FOLDER_NAME, LOCAL_CONFIG_DIR, USERDATA_CONFIG_FILE);
    }

    struct UserData user_data;
    // trying to open it
    FILE *user_data_file = fopen(user_data_file_path, "rb");
    if (!user_data_file){
        // trying to create file and set some default values
        debug(("could not open %s trying to create it \n", user_data_file_path));
        user_data_file = fopen(user_data_file_path, "w");

        if (!user_data_file){
            debug(("faild to create it too\n"));
            exit(EXIT_FAILURE);
        }

        strcpy(user_data.username, "");
        strcpy(user_data.email, "");

        fclose(user_data_file);

    }else {
        // just loading data
        if (fread(&user_data, sizeof(user_data), 1 ,user_data_file) <= 0){
            printError("could not read data from user data file!");
            fclose(user_data_file);
            exit(EXIT_FAILURE);
        }
        fclose(user_data_file);
    }


    // set new data
    if(areStringsEqual(variable,"name")){
        strcpy(user_data.username, value);
    }else if (areStringsEqual(variable, "email")){
        strcpy(user_data.email, value);
    }else{
        printfError("wrong variable for user : %s", variable);
        exit(EXIT_FAILURE);
    }

    user_data_file = fopen(user_data_file_path, "wb");
    if(fwrite(&user_data, sizeof(user_data), 1, user_data_file) <= 0){
        printError("could not save data on user data file!");
        fclose(user_data_file);
        exit(EXIT_FAILURE);
    }
    fclose(user_data_file);

}

// function to load all user data
int loadUserData(){
    // loading global user data

    char user_data_file_path[MAX_PATH_LEN];
    sprintf(user_data_file_path, "%s/%s", GLOBAL_CONFIG_DIR, USERDATA_CONFIG_FILE);
    
    FILE *user_data_file = fopen(user_data_file_path, "rb");

    if (!user_data_file){
        printError("no global user data found! please use 'config --global user.*' to create one");
        exit(EXIT_FAILURE);
    }

    if (fread(&GIT_userdata, sizeof(GIT_userdata), 1, user_data_file) < 1){
        printError("no data on global user data file! please use 'config --global user.*'");
        fclose(user_data_file);
        exit(EXIT_FAILURE);
    }

    fclose(user_data_file);
    


    // loading local user data

    if (!GIT_parent_dir){ // exit if no git dir found
        return EXIT_SUCCESS;
    }

    sprintf(user_data_file_path, "%s/%s/%s/%s", GIT_parent_dir, GIT_FOLDER_NAME, LOCAL_CONFIG_DIR, USERDATA_CONFIG_FILE);
    user_data_file = fopen(user_data_file_path, "rb");

    if (!user_data_file){
        debug(("no local user data found! please use 'config user.*' to create one\n"));
        exit(EXIT_FAILURE);
    }

    if (fread(&GIT_userdata, sizeof(&GIT_userdata), 1, user_data_file) < 1){
        printError("no data on local user data file! please use 'config user.*'\n");
        fclose(user_data_file);
        exit(EXIT_FAILURE);
    }

    fclose(user_data_file);
}


int createAlias(const char *name, const char *command, const bool is_global){
    // this function still does not have the ability to create folders by itself (but will be soon :) )
    struct Alias new_alias;
    strcpy(new_alias.name, name); // setting alias name

    TokenArray args_array = parseCommand(command);
    if (!areArgsValid(args_array.count, args_array.tokens)){
        freeTokens(&args_array);
        exit(EXIT_FAILURE);
    }

    // copying alias args and argc
    for (int i = 0; i < args_array.count; i++)
    {
        strcpy(new_alias.argv[i], args_array.tokens[i]);
    }
    
    new_alias.argc = args_array.count;
    
    freeTokens(&args_array);

    char alias_file_path[MAX_PATH_LEN];
    if (is_global){
        sprintf(alias_file_path, "%s/%s", GLOBAL_CONFIG_DIR, ALIAS_FILE);
    }else{
        sprintf(alias_file_path, "%s/%s/%s/%s", GIT_parent_dir, GIT_FOLDER_NAME, LOCAL_CONFIG_DIR, ALIAS_FILE);
    }

    FILE *alias_file = fopen(alias_file_path, "ab");
    if(!alias_file){
        printError("could not open or create the alias file :/ ");
        exit(EXIT_FAILURE);
    }

    fwrite(&new_alias, sizeof(new_alias), 1, alias_file);

    fclose(alias_file);

}


// function to load GIT_alias_list
int loadAliasList(){
    // loading global alias

    char alias_file_path[MAX_PATH_LEN];
    sprintf(alias_file_path, "%s/%s", GLOBAL_CONFIG_DIR, ALIAS_FILE);
    
    FILE *alias_file = fopen(alias_file_path, "rb");

    int loaded_alias_count = 0;
    if (!alias_file){
        debug(("no global alias file found! please use 'config --global alias.*' to create one\n"));
        return EXIT_FAILURE;
    }

    for (loaded_alias_count = 0;fread(&GIT_alias_list[loaded_alias_count], sizeof(struct Alias), 1, alias_file); loaded_alias_count++)
    {
        if (loaded_alias_count >= MAX_ALIAS_COUNT){
            printError("maximum alias count exeeded");
            exit(EXIT_FAILURE);
        }
    }
    
    GIT_alias_count = loaded_alias_count;

    fclose(alias_file);
    


    // loading local alias

    if (!GIT_parent_dir){ // exit if no git dir found
        return EXIT_SUCCESS;
    }

    sprintf(alias_file_path, "%s/%s/%s/%s", GIT_parent_dir, GIT_FOLDER_NAME, LOCAL_CONFIG_DIR, ALIAS_FILE);
    alias_file = fopen(alias_file_path, "rb");

    if (!alias_file){
        debug(("no local alias file found! please use 'config alias.*' to create one\n"));
        exit(EXIT_FAILURE);
    }

    for (loaded_alias_count = 0;fread(&GIT_alias_list[loaded_alias_count], sizeof(struct Alias), 1, alias_file); loaded_alias_count++)
    {
        if (loaded_alias_count >= MAX_ALIAS_COUNT){
            printError("maximum alias count exeeded");
            exit(EXIT_FAILURE);
        }
    }
    GIT_alias_count = loaded_alias_count;

    fclose(alias_file);
}