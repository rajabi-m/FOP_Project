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
#include <sys/types.h>
#include <libgen.h>
#include <errno.h>
#include <stdarg.h>
#include <linux/limits.h>
#define __USE_XOPEN // to unlock more features in time.h
#include <time.h>

// configs
#include "configs.h"

// required files
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
#include "log.h"
#include "revert.h"
#include "tag.h"
#include "diff.h"
#include "merge.h"
#include "grep.h"