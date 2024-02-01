#ifndef __CHECKOUT_H_
#define __CHECKOUT_H_

int GIT_Checkout(int argc, char **argv);

void stepBackward(int count, char *final_hash);

void loadCommit(const char *hash);

#endif