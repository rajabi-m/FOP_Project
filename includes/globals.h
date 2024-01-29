#include "configs.h"

#ifndef __GLOBALS_H_
#define __GLOBALS_H_


// struct to save all valid commands
/**
 * @brief Represents a command in a command-line application.
 *
 * Command is a structure that defines the properties of a command in a
 * command-line application. It includes the command name, minimum and maximum
 * expected number of arguments, usage help information, and a function pointer
 * to the implementation of the command. This structure facilitates the
 * organization and execution of various commands within the application.
 */
struct Command {
    const char *command_name;    /**< Name of the command. */
    int min_argc;                /**< Minimum number of expected arguments. */
    int max_argc;                /**< Maximum number of expected arguments. */
    const char *usage_help;      /**< Usage help information for the command. */
    int (*function)(int argc, char *argv[]); /**< Function pointer to the command implementation. */
    bool is_global;
};


/**
 * @brief External declaration of an array of Command structures for Git commands.
 *
 * External declaration for a global array of Command structures named
 * `GIT_commands_list`. This array holds definitions for various Git
 * commands, including details such as the command name, expected argument range,
 * usage help information, and the associated function pointer. The array is
 * initialized with specific command entries, and a sentinel entry is used to
 * indicate the end of the commands list.
 *
 */
extern struct Command *GIT_commands_list;

/**
 * @brief Represents user data, including username and email.
 *
 * UserData is a structure that holds information about a user, including their
 * username and email. The structure defines character arrays for storing the
 * username and email, with maximum lengths specified by MAX_USERNAME_LEN and
 * MAX_EMAIL_LEN, respectively.
 */
struct UserData {
    char username[MAX_USERNAME_LEN]; /**< User's username. */
    char email[MAX_EMAIL_LEN];       /**< User's email address. */
};


/**
 * @brief External declaration of a global UserData structure for Git user information.
 *
 * External declaration for a global UserData structure named `GIT_userdata`. This
 * structure is designed to hold information about a Git user, including their
 * username and email address. The actual definition and initialization of this
 * structure are expected to be provided in another source file or module.
 */
extern struct UserData GIT_userdata;


/**
 * @brief Represents an alias for a command with parameters.
 *
 * Alias is a structure that defines an alias for a command with parameters.
 * It includes the name of the alias, the number of arguments (argc), and an
 * array (argv) to store the command parameters. The maximum length of the name
 * and each parameter is specified by MAX_USERNAME_LEN and MAX_COMMAND_LEN,
 * respectively. This struct is commonly used to store and manage command
 * aliases in applications. // NEEDS AN UPDATE
 */
struct Alias {
    char name[MAX_USERNAME_LEN];         /**< Name of the alias. */
    int argc;                            /**< Number of command arguments. */
    char argv[10][MAX_COMMAND_LEN];      /**< Array to store command parameters. */
};


/**
 * @brief External declaration of an array of Alias structures for Git command aliases.
 *
 * External declaration for a global array of Alias structures named
 * `GIT_alias_list`. This array is designed to store definitions for various Git
 * command aliases, including details such as the alias name, argument count, and
 * an array to store command parameters. The array has a maximum capacity
 * specified by MAX_ALIAS_COUNT. The actual definition and initialization of this
 * array are expected to be provided in another source file or module. // NEEDS TO UPDATE
 */
extern struct Alias *GIT_alias_list;


/**
 * @brief External declaration of a global variable for the Git parent directory path.
 *
 * External declaration for a global character pointer named `GIT_parent_dir`. This
 * pointer is expected to store the path of the Git parent directory. The actual
 * definition and initialization of this variable are expected to be provided
 * in another source file or module.
 */
extern char *GIT_parent_dir;

/**
 * @brief External declaration of a global variable for the count of Git command aliases.
 *
 * External declaration for a global integer variable named `GIT_alias_count`.
 * This variable is intended to store the count of Git command aliases. The actual
 * definition and initialization of this variable are expected to be provided
 * in another source file or module.
 */
extern int GIT_alias_count;

typedef struct {
    char path[MAX_PATH_LEN];
    char object_hash[HASH_LEN + 1];
    int32_t access_code;
} GitFile;

extern GitFile *GIT_staging_area;

extern int GIT_stagedfiles_count;


typedef struct{
    struct MetaData{
        struct UserData user;
        char message[73];
        char hash[HASH_LEN + 1];
        char branch[MAX_BRANCH_NAME_LEN];
        time_t time;
        int files_count;
        char parents_hash[2][HASH_LEN + 1]; // a commit can have two parents if it the result of a merge (فاز ۲ حواسم بهت هست XD)
    } meta_data;
    GitFile *files;
} Commit;

extern char *GIT_HEAD; // to save head pos

#endif