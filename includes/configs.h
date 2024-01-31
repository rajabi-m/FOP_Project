#include <linux/limits.h>

// debug
#define __DEBUG_MODE_

#ifdef __DEBUG_MODE_
#define debug(x) printf x
#else
#define debug(x) while(0){}
#endif

// configs related to git itself
/**
 * @brief Folder name for the Git repository.
 */
#define GIT_DIR_NAME ".giga-git"

/**
 * @brief Maximum length for user usernames.
 */
#define MAX_USERNAME_LEN 40

/**
 * @brief Maximum length for user email addresses.
 */
#define MAX_EMAIL_LEN 50

/**
 * @brief Global configuration directory path.
 */
#define GLOBAL_CONFIG_DIR "/home/mahdi-rajabi/.config/giga-git"

/**
 * @brief Local configuration directory name.
 */
#define LOCAL_CONFIG_DIR "config"

/**
 * @brief Userdata configuration file name.
 */
#define USERDATA_CONFIG_FILE "userdata.gigachad"

/**
 * @brief Maximum number of command aliases.
 */
#define MAX_ALIAS_COUNT 100

/**
 * @brief Alias file name.
 */
#define ALIAS_FILE "alias.sigma"

/**
 * @brief Maximum length for file paths.
 */
#define MAX_PATH_LEN 1024

#define MAX_LINE_LEN 1024

#define MAX_BRANCH_NAME_LEN 20

/**
 * @brief Maximum length for command strings.
 */
#define MAX_COMMAND_LEN 40


#define HASH_LEN 10 /* Length of hashed password in characters */

/**
 * @brief Usage help message for generic Git commands.
 */
#define GIT_USAGE "you dont know how to use giga-git :/"


/**
 * @brief Usage help message for the config command.
 */
#define CONFIG_COMMAND_USAGE "just enter the config things from project doc :/"

#define INIT_COMMAND_USAGE "just enter the init things from project doc :/"

#define ADD_COMMAND_USAGE "go learn how to use it"

#define RESET_COMMAND_USAGE "go learn reset command baby"

#define STATUS_COMMAND_USAGE "go learn how to use status command"

#define COMMIT_COMMAND_USAGE "ay yo go learn it"
#define SET_COMMAND_USAGE "ay yo go learn it"
#define REPLACE_COMMAND_USAGE "ay yo go learn it"
#define REMOVE_COMMAND_USAGE "ay yo go learn it"
#define BRANCH_COMMAND_USAGE "ay yo go learn it"

#define COMMITS_DIR "commits"

#define OBJECTS_DIR "objects"

#define HEAD_FILE_NAME "saresh.head"

#define STAGE_FILE_NAME "stage.nerd"

#define BRANCHES_FILE_NAME "branches.shakhe"

#define STAGE_HISTORY_FILE_NAME "stage.his"

#define MAIN_BRANCH_NAME "master"