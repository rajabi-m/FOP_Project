#include "configs.h"

#ifndef __GLOBALS_H_
#define __GLOBALS_H_


// struct to save all valid commands
struct Command{
    char command_name[MAX_COMMAND_LEN];
    int max_arg;
};

extern struct Command GIT_commands_list[COMMANDS_COUNT];

// struct to save user data
struct UserData{
    char username[MAX_USERNAME_LEN];
    char email[MAX_EMAIL_LEN];
};

extern struct UserData GIT_userdata;

// struct to save all aliases
struct Alias{
    char name[MAX_USERNAME_LEN];
    int argc;
    char argv[10][MAX_COMMAND_LEN];
};

extern struct Alias GIT_alias_list[MAX_ALIAS_COUNT];


// global variables

extern char *GIT_parent_dir;
extern int GIT_alias_count;

#endif