#ifndef __BRANCH_H_
#define __BRANCH_H_

void loadBranchesList();
void saveBranchesList();

int GIT_Branch(int argc, char **argv);

void loadGitHead();

Branch *getBranchByName(const char *name);

#endif