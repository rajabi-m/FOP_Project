#include "include.h"

struct Command GIT_commands_list_array[] = {
    {"config", 4, 5, CONFIG_COMMAND_USAGE, GIT_Config, true},
    {"init", 2, 2, INIT_COMMAND_USAGE, GIT_Init, true},
    {"add", 3, INT32_MAX, ADD_COMMAND_USAGE, GIT_Add, false},
    {"reset", 3, INT32_MAX, RESET_COMMAND_USAGE, GIT_Reset, false},
    {"status", 2, 2, STATUS_COMMAND_USAGE, GIT_Status, false},
    {"commit", 3, 4, COMMIT_COMMAND_USAGE, GIT_Commit, false},
    {"set", 6, 6, SET_COMMAND_USAGE, GIT_Set, false},
    {"replace", 6, 6, REPLACE_COMMAND_USAGE, GIT_Replace, false},
    {"remove", 4, 4, REMOVE_COMMAND_USAGE, GIT_Remove, false},
    {"branch", 2, 3, BRANCH_COMMAND_USAGE, GIT_Branch, false},
    {NULL, 0, 0, NULL, NULL} // just to show that it is the end of the commands list
};

struct Command *GIT_commands_list = GIT_commands_list_array; // just to turn it into a pointer

struct UserData GIT_userdata = {"", ""};

char *GIT_parent_dir = NULL;

struct Alias *GIT_alias_list = NULL;

int GIT_alias_count = 0;

GitFile *GIT_staging_area = NULL;

int GIT_stagedfiles_count = 0;

Branch *GIT_branches_list = NULL;

int GIT_branches_count = 0;

char GIT_HEAD[((HASH_LEN > MAX_BRANCH_NAME_LEN)? HASH_LEN : MAX_BRANCH_NAME_LEN) + 1]; // just to make sure everything fits in it XD

bool GIT_is_head_attached = false;