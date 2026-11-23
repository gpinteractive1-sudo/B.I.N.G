#include "zeirbra.h"
#include "yalvm.h"
#include <stdio.h>

int main (int argc, char **argv) {
    if(argc < 2) {
        fprintf(stderr, "Usage: %s <file.ylvmbc>\n", argv[0]);
        return 1;
    }

    const char *path = argv[1];
    ZbrModule *m = yalvm_load_bc(path);

    if(!m) {
        fprintf(stderr,"Error: failed to load '%s'\n", path);
        return 1;
    }

    zbr_dump(m);
    zbr_destroy(m);
    return 0;
}