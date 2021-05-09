#ifndef LAB7_HELPER_H
#define LAB7_HELPER_H

#include <stdbool.h>
#include "defs.h"

#ifdef LAB7_SYSTEMV
    #include "helper_systemv.h"
#else
    #include "helper_posix.h"
#endif

const char* helper_get_error();


#endif //LAB7_HELPER_H