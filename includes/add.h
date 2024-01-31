#ifndef __ADD_H_
#define __ADD_H_

int GIT_Add(int argc, char **argv);
int GIT_Reset(int argc, char **argv);

int stageFile(const char *path, bool bug_fixer);

int loadStagingArea();

#endif