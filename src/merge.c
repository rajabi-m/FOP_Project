#include "include.h"

int GIT_Merge(int argc, char **argv){
    int option;
    char *branch1_name = GIT_HEAD_branch->name;
    char *branch2_name = NULL;

    struct option merge_options[] = {
        {"branch", required_argument, NULL, 'b'},
        {0, 0, 0, 0}
    };

    while ((option = getopt_long(argc, argv, "b:", merge_options, NULL)) != -1)
    {
        if (option == ':' || option == '?') return EXIT_FAILURE;

        if (option == 'b'){
            branch2_name = strdup(optarg);
        }

    }


    if (!(branch1_name && branch2_name)){
        // printError(MERGE_COMMAND_USAGE);
        return EXIT_FAILURE;
    }

    Branch *branch1 = getBranchByName(branch1_name);    
    Branch *branch2 = getBranchByName(branch2_name);    

    if (!(branch1 && branch2)){
        printError("wrong branch name!");
        return EXIT_FAILURE;
    }

    CommitDiff *branch_diff = compareTwoCommits(branch1->commit_hash, branch2->commit_hash);

    if (!branch_diff){
        printError("error happened while trying to compare two branches!!");
        return EXIT_FAILURE;
    }

    // new commit metadata
    time_t current_time;
    time(&current_time);
    srand((unsigned)(current_time + GIT_stagedfiles_count));
    char *hash = generateRandomString(HASH_LEN);
    Commit new_commit;
    strcpy(new_commit.meta_data.branch, branch1_name);
    new_commit.meta_data.files_count = branch_diff->commons_n + branch_diff->first_commit_files_n + branch_diff->second_commit_files_n;
    strcpy(new_commit.meta_data.hash, hash);
    sprintf(new_commit.meta_data.message, "merging branch %s with branch %s.", branch1_name, branch2_name);
    strcpy(new_commit.meta_data.parents_hash[0], branch1->commit_hash);
    strcpy(new_commit.meta_data.parents_hash[1], branch2->commit_hash);
    strcpy(new_commit.meta_data.user.username, GIT_userdata.username);
    strcpy(new_commit.meta_data.user.email, GIT_userdata.email);
    new_commit.meta_data.time = current_time;

    // new commit files
    new_commit.files = malloc(new_commit.meta_data.files_count * sizeof(GitFile));

    Commit *branch1_commit = openCommit(branch1->commit_hash);
    Commit *branch2_commit = openCommit(branch2->commit_hash);

    int index = 0;
    for (int j = 0; j < branch1_commit->meta_data.files_count; j++)
    {
        for (int i; i < branch_diff->first_commit_files_n; i++)
        {
            if (areStringsEqual(branch_diff->first_commit_files[i], branch1_commit->files[j].path)){
                memcpy(&new_commit.files[index], &branch1_commit->files[j], sizeof(GitFile));
                index ++;
            }
        }
        
    }

    for (int i = 0; i < branch2_commit->meta_data.files_count; i++)
    {
        for (int j = 0; j < branch_diff->second_commit_files_n; j++)
        {
            if (areStringsEqual(branch_diff->second_commit_files[j], branch2_commit->files[i].path)){
                memcpy(&new_commit.files[index], &branch2_commit->files[i], sizeof(GitFile));
                index ++;
            }
        }
        
    }
    
    // handling common files
    for (int i = 0; i < branch_diff->commons_n; i++)
    {
        srand((unsigned)(time(NULL) + i));
        char *hash = generateRandomString(HASH_LEN);

        GitFile merged_file;
        merged_file.access_code = 0755; // TODO:
        strcpy(merged_file.path, branch_diff->commons[i].file_path);
        strcpy(merged_file.object_hash, hash);
        memcpy(&new_commit.files[index], &merged_file, sizeof(GitFile));
        index ++;

        char *new_object_path = gigaStrcat(7, GIT_parent_dir, "/", GIT_DIR_NAME, "/", OBJECTS_DIR, "/", hash);
        FILE *new_object = fopen(new_object_path, "w");

        if (!new_object){
            printError("error happened while trying to make new object in merging process!");
            return EXIT_FAILURE;
        }

        // writing data on new object file
        
        // finding the object that is on the current branch last commit
        FILE *current_object = NULL;
        for (int j = 0; j < branch1_commit->meta_data.files_count; j++)
        {
            if (areStringsEqual(branch1_commit->files[j].path, branch_diff->commons[i].file_path)){
                current_object = openObject(branch1_commit->files[j].object_hash, "r");
            }
        }
        
        if (!current_object){
            printError("core exploded! a file that excepted to exist, does not.");
            exit(EXIT_FAILURE);
        }

        char line[MAX_LINE_LEN];
        int line_n = 0;
        int diff_index = 0;
        char *res = (char *) 1; // just a temp value
        while (true)
        {
            line_n ++;

            if (res != NULL) res = fgets(line, MAX_LINE_LEN, current_object);
            
            if (branch_diff->commons[i].diffs.diff[diff_index].first_line_n == line_n){
                if (branch_diff->commons[i].diffs.diff[diff_index].second_line_n == 0){
                    // if the line only exists in file 1.
                    fputs(branch_diff->commons[i].diffs.diff[diff_index].first, new_object);
                    fputc('\n', new_object);
                }
                else{
                    // the hole merge conflict resolve process is here
                    while (true)
                    {
                        char input;
                        printf("------------<%s>>>\n", branch_diff->commons[i].file_path);
                        printf("<%s><%d>\n", branch1_name, branch_diff->commons[i].diffs.diff[diff_index].first_line_n);
                        printf("--- %s\n", branch_diff->commons[i].diffs.diff[diff_index].first);
                        printf("<%s><%d>\n", branch2_name, branch_diff->commons[i].diffs.diff[diff_index].second_line_n);
                        printf("--- %s\n", branch_diff->commons[i].diffs.diff[diff_index].second);
                        printf("<<<%s>------------\n", branch_diff->commons[i].file_path);
                        scanf("%c", &input);
                        printf("what do you want to do? ->");
                        switch(input){
                            case '1':{
                                fputs(branch_diff->commons[i].diffs.diff[diff_index].first, new_object);
                                fputc('\n', new_object);
                                break;
                            }
                            case '2':{
                                fputs(branch_diff->commons[i].diffs.diff[diff_index].second, new_object);
                                fputc('\n', new_object);
                                break;
                            }
                            case 'q':{
                                fclose(new_object);
                                fclose(current_object);
                                remove(new_object_path);
                                printf("stopped merging process");
                                exit(EXIT_SUCCESS);
                                break;
                            }
                            default:
                                continue;
                        }

                        break;
                    }
                    

                }


            }else{
                if (res != NULL){
                    fputs(line, new_object);
                }else{
                    if (diff_index < branch_diff->commons[i].diffs.diff_count){
                        // if there is still some lines in file 2
                        fputs(branch_diff->commons[i].diffs.diff[diff_index].second, new_object);
                        fputc('\n', new_object);
                        diff_index ++;
                    }else{
                        break;
                    }
                }
            }

        }

        fclose(current_object);
        fclose(new_object);



        
        

    }
    
    freeCommit(branch1_commit);
    freeCommit(branch2_commit);

    char *new_commit_path = gigaStrcat(7, GIT_parent_dir, "/", GIT_DIR_NAME, "/", COMMITS_DIR, "/", hash);
    
    FILE *new_commit_file = fopen(new_commit_path, "wb");

    fwrite(&new_commit.meta_data, sizeof(new_commit.meta_data), 1, new_commit_file);
    fwrite(new_commit.files, sizeof(GitFile), new_commit.meta_data.files_count, new_commit_file);

    fclose(new_commit_file);
    
    loadCommit(hash);

    return EXIT_SUCCESS;

}