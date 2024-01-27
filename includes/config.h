#ifndef __CONFIG_H_
#define __CONFIG_H_

/**
 * @brief Handles the 'config' command for Git-like application.
 *
 * This function processes the 'config' command, allowing users to configure
 * various settings. It supports options like '--global' to set global configurations.
 * The command syntax follows the pattern: 'giga-git config [--global] category.variable value'.
 * It validates the command line arguments, handles options, and delegates to specific
 * functions based on the provided category.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments.
 * @return int Returns EXIT_SUCCESS if the 'config' command is executed successfully;
 * otherwise, returns EXIT_FAILURE, and an error message is printed to the standard error stream.
 */
int GIT_Config(int argc, char *argv[]);

/**
 * @brief Loads user data from global and local configuration files.
 *
 * This function reads user data from global and local configuration files,
 * populating the global `GIT_userdata` struct. It first attempts to load global
 * user data from the specified file path (`GLOBAL_CONFIG_DIR/USERDATA_CONFIG_FILE`).
 * If no global user data is found, it exits with an error message. Next, it
 * attempts to load local user data from the Git parent directory, if available.
 * If no local user data is found, it exits with a debug message.
 *
 * @return int Returns EXIT_SUCCESS if user data is loaded successfully;
 * otherwise, exits the program with an error message.
 */
int loadUserData();

/**
 * @brief Loads the Git alias list from global and local configuration files.
 *
 * This function reads Git alias definitions from global and local configuration
 * files, populating the global `GIT_alias_list` array and updating the count of
 * aliases in `GIT_alias_count`. It first attempts to load global aliases from
 * the specified file path (`GLOBAL_CONFIG_DIR/ALIAS_FILE`). If no global alias
 * file is found, it returns with a debug message. Next, it attempts to load local
 * aliases from the Git parent directory, if available. If no local alias file is
 * found, it returns with a debug message.
 *
 * @return int Returns EXIT_SUCCESS if the alias list is loaded successfully;
 * otherwise, exits the program with an error message.
 */
int loadAliasList();


#endif