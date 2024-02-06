#ifndef __PRE_COMMIT_H_
#define __PRE_COMMIT_H_

int GIT_PreCommit(int argc, char **argv);

bool checkHooks(GitFile *git_file, bool print_result);

#endif