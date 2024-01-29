#include <stdlib.h>

#ifndef __OBJECTS_H_
#define __OBJECTS_H_

bool createNewObject(const char *path, char *object_hash);

FILE *openObject(const char *object_hash, const char *mode);

#endif