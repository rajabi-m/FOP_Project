#include "include.h"

struct Command GIT_commands_list[COMMANDS_COUNT] = {
    {"add", 3},
    {"config", 5}
};

struct UserData GIT_userdata = {"", ""};

char *GIT_parent_dir = NULL;

struct Alias GIT_alias_list[MAX_ALIAS_COUNT];
int GIT_alias_count = 0;