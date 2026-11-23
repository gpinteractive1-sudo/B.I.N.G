#ifndef YALVM_H
#define YALVM_H

#include "zeirbra.h"
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

uint64_t yalvm_exec( const ZbrModule *m, ZbrLabel entry);
bool       yalvm_save_bc(const ZbrModule *m, const char *filename);
ZbrModule* yalvm_load_bc(const char *filename);


#ifdef __cplusplus
}
#endif
#endif