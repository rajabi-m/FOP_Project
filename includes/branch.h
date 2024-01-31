#ifndef __BRANCH_H_
#define __BRANCH_H_

void loadBranchesList();
void saveBranchesList();

int GIT_Branch(int argc, char **argv);

void loadGitHead();
void saveGitHead(const char *branch_name, bool is_attached, const char *commit_hash);

Branch *getBranchByName(const char *name);

#endif