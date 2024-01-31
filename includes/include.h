// libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <unistd.h>
#ifndef __USE_MISC
#define __USE_MISC // to enable some dirent.h features
#endif
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>
#include <linux/limits.h>
#include <time.h>

// configs
#include "configs.h"

// requiered files
#include "globals.h"
#include "useful-functions.h"


// commands
#include "config.h"
#include "init.h"
#include "objects.h"
#include "add.h"
#include "status.h"
#include "commit.h"
#include "branch.h"
#include "checkout.h"