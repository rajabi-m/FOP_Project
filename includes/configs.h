// debug
#define __DEBUG_MODE_

#ifdef __DEBUG_MODE_
#define debug(x) printf x
#else
#define debug(x) while(0){}
#endif

// configs related to git itself
#define GIT_FOLDER_NAME ".giga-git"




// configs related to user
#define MAX_USERNAME_LEN 40
#define MAX_EMAIL_LEN 50

// configs related to config command
#define GLOBAL_CONFIG_DIR "/home/mahdi-rajabi/.config/giga-git"
#define LOCAL_CONFIG_DIR "config"
#define USERDATA_CONFIG_FILE "userdata.gigachad"
#define MAX_ALIAS_COUNT 100
#define ALIAS_FILE "alias.sigma"

// technical configs
#define MAX_PATH_LEN 1024
#define MAX_COMMAND_LEN 40

// usage help for commands

#define GIT_USAGE "you dont know how to use giga-git :/"

#define CONFIG_COMMAND_USAGE "just enter the config things from project doc :/"