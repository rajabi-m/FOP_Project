#include "include.h"


int main(){


    int argc;
    char *argv[] = {"giga-git","abbas", "0-0"};

    for (int i = 0; ; i++)
    {
        if (areStringsEqual(argv[i], "0-0")){
            argc = i;
            break;
        }
    }
    
// int main(int argc, char const *argv[]){

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
            // return main(GIT_alias_list[i].argc, GIT_alias_list[i].argv); does not work while debugging
            debug(("alias found : %s\n", GIT_alias_list[i].name));
            return 0;
        }
    }
    
    
    return 0;
}
