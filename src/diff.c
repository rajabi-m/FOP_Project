#include "include.h"

typedef enum {
    not_specified = 0,
    compare_files = 1,
    compare_commits = 2
} DiffMode;

int printCommitDiffs(const char *first_commit_hash, const char *second_commit_hash);

int GIT_Diff(int argc, char **argv){
    int option;

    DiffMode diff_mode = not_specified;

    // stuff required for compare_files mode
    char *file1_path = NULL;
    char *file2_path = NULL;
    int file1_start = 1, file1_end = INT32_MAX;
    int file2_start = 1, file2_end = INT32_MAX;

    // stuff required for compare_commits mode
    char *first_commit_hash = NULL, *second_commit_hash = NULL;

    struct option diff_options[] = {
        {"commit", required_argument, NULL, 'c'},
        {"file", required_argument, NULL, 'f'},
        {"line1", required_argument, NULL, 'l'},
        {"line2", required_argument, NULL, 'I'},
        {0 ,0, 0, 0}
    };
    
    while ((option = getopt_long(argc, argv, "c:f:l:I:", diff_options, NULL)) != -1)
    {
        if (option == ':' || option == '?') return EXIT_FAILURE;

        // options for compare_files mode
        if (option == 'f'){
            if (diff_mode == compare_commits){
                printError(DIFF_COMMAND_USAGE);
                return EXIT_FAILURE;
            }
            diff_mode = compare_files;
            
            if (!file1_path){
                file1_path = strdup(optarg);
            }else{
                file2_path = strdup(optarg);
            }
        }

        if (option == 'l'){
            if (sscanf(optarg, "%d-%d", &file1_start, &file1_end) < 2){
                printError("bad format for line numbers.");
                return EXIT_FAILURE;
            }
        }

        if (option == 'I'){
            if (sscanf(optarg, "%d-%d", &file2_start, &file2_end) < 2){
                printError("bad format for line numbers.");
                return EXIT_FAILURE;
            }
        }

        // options for compare_commits mode
        if (option == 'c'){
            if (diff_mode == compare_files){
                printError(DIFF_COMMAND_USAGE);
                return EXIT_FAILURE;
            }
            diff_mode = compare_commits;

            if (!first_commit_hash){
                first_commit_hash = strdup(optarg);
            }else{
                second_commit_hash = strdup(optarg);
            }
        }
    }

    if (diff_mode == not_specified){
        printError(DIFF_COMMAND_USAGE);
        return EXIT_FAILURE;
    }else if (diff_mode == compare_files){

        FILE *file1 = fopen(file1_path, "r");
        FILE *file2 = fopen(file2_path, "r");

        if (!(file1 && file2)){
            printError("file paths are wrong.");
            return EXIT_FAILURE;
        }
        
        int count = 0;
        Difference *difference = compareFilesLineByLineV2(file1, file2, file1_start, file1_end, file2_start, file2_end, &count);

        for (int i = 0; i < count; i++)
        {
            // printf(BLU_TEXT"󰈔 %s 󰞷 %d\n"WHT_TEXT"󰇘󰇘󰇘󰇘 "CYN_TEXT"%s\n"RED_TEXT"󰈔 %s 󰞷 %d\n"WHT_TEXT"󰇘󰇘󰇘󰇘 "YEL_TEXT"%s\n"RESET_TEXT, file1_path, difference[i].first_line_n, difference[i].first, file2_path, difference[i].second_line_n, difference[i].second);
            printDifferences(difference + i, file1_path, file2_path, "󰈔");
            printf("------------------------------------------\n");

        }
    }else if (diff_mode == compare_commits){
        if (!(first_commit_hash && second_commit_hash)){
            printError(DIFF_COMMAND_USAGE);
            return EXIT_FAILURE;
        }

        return printCommitDiffs(first_commit_hash, second_commit_hash);        

    }

    return EXIT_SUCCESS;

}

int printCommitDiffs(const char *first_commit_hash, const char *second_commit_hash){
    CommitDiff *commit_diff = compareTwoCommits(first_commit_hash, second_commit_hash);

    if (!commit_diff){
        printError("wrong commit hash!");
        return EXIT_FAILURE;
    }

    if (commit_diff->first_commit_files_n) printf(BLU_TEXT"󰈔 Files that only exist in commit %s:\n", first_commit_hash);
    for (int i = 0; i < commit_diff->first_commit_files_n; i++)
    {
        printf(CYN_TEXT"󰇘󰇘󰇘󰇘 %s\n"RESET_TEXT, commit_diff->first_commit_files[i]);
    }
    if (commit_diff->second_commit_files_n) printf(RED_TEXT"󰈔 Files that only exist in commit %s:\n", second_commit_hash);
    for (int i = 0; i < commit_diff->second_commit_files_n; i++)
    {
        printf(YEL_TEXT"󰇘󰇘󰇘󰇘 %s\n"RESET_TEXT, commit_diff->second_commit_files[i]);
    }

    printf(GRN_TEXT"󰈔 Shared files:\n");
    for (int i = 0; i < commit_diff->commons_n; i++)
    {
        printf(MAG_TEXT"󰇘󰇘󰇘󰇘 󰈔 %s\n", commit_diff->commons[i].file_path);
        for (int j = 0; j < commit_diff->commons[i].diffs.diff_count; j++)
        {
            // printf(WHT_TEXT"󰇘󰇘󰇘󰇘󰇘󰇘󰇘󰇘 "BLU_TEXT"󰜘 %s 󰞷 %d\n", first_commit_hash, commit_diff->commons[i].diffs.diff[j].first_line_n);
            // printf(WHT_TEXT"󰇘󰇘󰇘󰇘󰇘󰇘󰇘󰇘󰇘󰇘󰇘󰇘 "CYN_TEXT"%s\n", commit_diff->commons[i].diffs.diff[j].first);
            // printf(WHT_TEXT"󰇘󰇘󰇘󰇘󰇘󰇘󰇘󰇘 "RED_TEXT"󰜘 %s 󰞷 %d\n", second_commit_hash, commit_diff->commons[i].diffs.diff[j].second_line_n);
            // printf(WHT_TEXT"󰇘󰇘󰇘󰇘󰇘󰇘󰇘󰇘󰇘󰇘󰇘󰇘 "YEL_TEXT"%s\n"RESET_TEXT, commit_diff->commons[i].diffs.diff[j].second);

            printDifferences(&commit_diff->commons[i].diffs.diff[j], first_commit_hash, second_commit_hash, "󰜘");
        }
        
    }
    
    freeCommitDiff(commit_diff);

    return EXIT_SUCCESS;
}