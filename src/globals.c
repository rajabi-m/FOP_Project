#include "include.h"

struct Command GIT_commands_list_array[] = {
    {"config", 4, 5, CONFIG_COMMAND_USAGE, GIT_Config, true},
    {"init", 2, 2, INIT_COMMAND_USAGE, GIT_Init, true},
    {"add", 3, INT32_MAX, ADD_COMMAND_USAGE, GIT_Add, false},
    {NULL, 0, 0, NULL, NULL} // just to show that it is the end of the commands list
};

struct Command *GIT_commands_list = GIT_commands_list_array; // just to turn it into a pointer

struct UserData GIT_userdata = {"", ""};

char *GIT_parent_dir = NULL;

struct Alias *GIT_alias_list = NULL;

int GIT_alias_count = 0;

GitFile *GIT_staging_area = NULL;

int GIT_stagedfiles_count = 0;