#include "include.h"

int doCommand(struct Alias *alias);


int main(){

    char command[] = "giga-git init";
    TokenArray tokenized = parseCommand(command);
    

    int argc = tokenized.count;
    char **argv = tokenized.tokens;
    
// int main(int argc, char *argv[]){

    GIT_parent_dir = findGitParentDir();
    debug(("git path = %s\n", GIT_parent_dir));

    loadUserData();
    loadAliasList();

    if (!areArgsValid(argc, argv)){
        exit(EXIT_FAILURE);
    }

    for (int i = 0; GIT_commands_list[i].command_name; i++)
    {
        if (areStringsEqual(argv[1], GIT_commands_list[i].command_name)){
            return GIT_commands_list[i].function(argc, argv);
        }
    }
    

    for (int i = 0; i < GIT_alias_count; i++)
    {
        if (areStringsEqual(argv[1], GIT_alias_list[i].name)){
            return doCommand(&GIT_alias_list[i]);
            debug(("alias found : %s\n", GIT_alias_list[i].name));
            return 0;
        }
    }
    
    
    return 0;
}

// temperory things to fix alias

char **fixedAliasArgv(struct Alias *alias){
    char **res = malloc(alias->argc * sizeof(char *));
    for (int i = 0; i < alias->argc; i++)
    {
        res[i] = strdup(alias->argv[i]);
    }
    
    return res;
}

int doCommand(struct Alias *alias){
    

    for (int i = 0; GIT_commands_list[i].command_name; i++)
    {
        if (areStringsEqual(alias->argv[1], GIT_commands_list[i].command_name)){
            return GIT_commands_list[i].function(alias->argc, fixedAliasArgv(alias));
        }
    }
}