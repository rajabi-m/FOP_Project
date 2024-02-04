#include "include.h"

void loadTags();
void printAllTags();
void printTag(const char *tag_name);

int GIT_Tag(int argc, char **argv){

    loadTags();    


    int option;

    char *tag_name = NULL;
    char *tag_message = NULL;
    char *commit_hash = NULL;
    bool can_override = false;
    bool create_mode = false;


    while ((option = getopt(argc, argv, "a:c:m:f")) != -1)
    {
        if (option == ':' || option == '?') return EXIT_FAILURE;

        create_mode = true; // if we get any options, it means we are on create mode

        if (option == 'a') tag_name = strdup(optarg);

        if (option == 'm') tag_message = strdup(optarg);

        if (option == 'c') commit_hash = strdup(optarg);

        if (option == 'f') can_override = true;

    }
    
    if (!create_mode){
        if (argc == 2){
            printAllTags();
        }else if (argc == 4){
            
            if (!areStringsEqual(argv[2], "show")){
                printError(TAG_COMMAND_USAGE);
                return EXIT_FAILURE;
            }

            printTag(argv[3]);
        }else{
            printError(TAG_COMMAND_USAGE);
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    if (!tag_name){
        printError("you should enter the tag name using -a option.");
        return EXIT_FAILURE;
    }

    if (commit_hash){
        Commit *commit = openCommit(commit_hash);

        if (!commit){
            printfError("there is no commit called %s.", commit_hash);
            return EXIT_FAILURE;
        }
    }else{
        commit_hash = GIT_HEAD_commit_hash;
    }

    bool is_unique = true;
    int index;
    for (index = 0; index < GIT_tags_count; index++)
    {
        if (areStringsEqual(GIT_tags_list[index].name, tag_name)){
            is_unique = false;
            break;
        }
    }
    
    if ((!is_unique) && (!can_override)){
        printError("This tag already exists. use -f flag to override it.");
        return EXIT_FAILURE;
    }

    if (is_unique){
        GIT_tags_count ++;
        GIT_tags_list = realloc(GIT_tags_list, (GIT_tags_count) * sizeof(Tag));
        index = GIT_tags_count - 1;
    }// if it is not unique the index is set at top.


    strcpy(GIT_tags_list[index].name, tag_name);
    strcpy(GIT_tags_list[index].commit_hash, commit_hash);
    strcpy(GIT_tags_list[index].user.username, GIT_userdata.username);
    strcpy(GIT_tags_list[index].user.email, GIT_userdata.email);
    GIT_tags_list[index].time = time(NULL);
    
    // set message if available
    if (tag_message)
        strcpy(GIT_tags_list[index].message, tag_message);
    else
        strcpy(GIT_tags_list[index].message, "");

    // finally saving all tags

    char *tags_file_path = gigaStrcat(5 ,GIT_parent_dir, "/", GIT_DIR_NAME, "/", TAGS_FILE_NAME);
    FILE *tags_file = fopen(tags_file_path, "wb");
    free(tags_file_path);

    if (!tags_file){
        printError("could not open tags file.");
        exit(EXIT_FAILURE);
    }

    fwrite(GIT_tags_list, sizeof(Tag), GIT_tags_count, tags_file);

    printfSuccess(("tag created."));

    return EXIT_SUCCESS;

}

void loadTags(){

    char *tags_file_path = gigaStrcat(5 ,GIT_parent_dir, "/", GIT_DIR_NAME, "/", TAGS_FILE_NAME);
    FILE *tags_file = fopen(tags_file_path, "rb");
    free(tags_file_path);

    if (!tags_file){
        printError("could not open tags file.");
        exit(EXIT_FAILURE);
    }

    GIT_tags_list = malloc(sizeof(Tag));
    GIT_tags_count = 0;

    for (int i = 0; fread(GIT_tags_list + i, sizeof(Tag), 1, tags_file); i++)
    {
        GIT_tags_count ++;
        GIT_tags_list = realloc(GIT_tags_list, (GIT_tags_count + 1) * sizeof(Tag));
    }

    fclose(tags_file);
    

}

void printAllTags(){
    for (int i = 0; i < GIT_tags_count; i++)
    {
        printf("󰓹 %s\n", GIT_tags_list[i].name);
    }
    
}

void printTag(const char *tag_name){
    Tag *tag = NULL;

    for (int i = 0; i < GIT_tags_count; i++)
    {
        if (areStringsEqual(tag_name, GIT_tags_list[i].name)){
            tag = GIT_tags_list + i;
            break;
        }
    }
    
    if (!tag){
        printfError("there is no tag called %s.", tag_name);
    }

    printf("󰓹 Tag name: %s\n󰜘 Commit: %s\n Author: %s<%s>\n Date: %s",tag->name, tag->commit_hash, tag->user.username
                                                                   ,tag->user.email, asctime(localtime(&tag->time)));

    if (!areStringsEqual(tag->message, "")){
        printf("󰚢 Message: %s\n", tag->message);
    }

}