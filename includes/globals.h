#include "configs.h"

#ifndef __GLOBALS_H_
#define __GLOBALS_H_


// struct to save all valid commands
struct Command{
    const char *command_name;
    int min_argc;
    int max_argc;
    const char *usage_help;
    int (*function)(int argc, char *argv[]);
};

extern struct Command *GIT_commands_list;

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