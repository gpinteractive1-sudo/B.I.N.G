#include "yalvm.h"
#include "zeirbra_internal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


bool yalvm_save_bc(const ZbrModule *m, const char *filename) {
    if (!m || !filename) return false;
    FILE *f = fopen(filename, "wb");
    if(!f) return false;

    fwrite("YLVMBC", 1, 6 , f);

    fwrite(&m->count, sizeof(uint32_t), 1 , f);
    fwrite(&m->label_count, sizeof(uint32_t), 1, f);

    fwrite(m->op, sizeof(uint8_t), m->count, f);
    fwrite(m->type, sizeof(uint8_t), m->count, f);
    fwrite(m->arg1, sizeof(uint32_t), m->count, f);
    fwrite(m->arg2, sizeof(uint32_t), m->count, f);
    fwrite(m->imm, sizeof(uint64_t), m->count, f);

    fwrite(m->label_pos, sizeof(uint32_t), m->label_count, f);

    for(uint32_t i = 0; i < m->label_count; i++) {
        uint32_t len = m->label_name[i] ? (uint32_t)strlen(m->label_name[i]) : 0;
        fwrite(&len, sizeof(uint32_t), 1, f);
        if(len > 0) {
            fwrite(m->label_name[i], 1, len, f);
        }
    }

    fclose(f);
    return true;
}

ZbrModule* yalvm_load_bc(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if(!f) return NULL;

    char signature[6];
    if(fread(signature, 1, 6, f) != 6 || memcmp(signature, "YLVMBC", 6) != 0) {
        fclose(f);
        return NULL;
    }

    uint32_t count = 0, label_count = 0;
    fread(&count, sizeof(uint32_t), 1, f);
    fread(&label_count, sizeof(uint32_t), 1, f);

    ZbrModule *m = zbr_create();
    if(count > m->capacity) {
        m->capacity = count;
        m->op   = realloc(m->op,   count * sizeof(uint8_t));
        m->type = realloc(m->type, count * sizeof(uint8_t));
        m->arg1 = realloc(m->arg1, count * sizeof(uint32_t));
        m->arg2 = realloc(m->arg2, count * sizeof(uint32_t));
        m->imm  = realloc(m->imm,  count * sizeof(uint64_t));

    }
    m->count = count;

    fread(m->op,   sizeof(uint8_t),  count, f);
    fread(m->type, sizeof(uint8_t),  count, f);
    fread(m->arg1, sizeof(uint32_t), count, f);
    fread(m->arg2, sizeof(uint32_t), count, f);
    fread(m->imm,  sizeof(uint64_t), count, f);

    if(label_count > m->label_capacity) {
        m->label_capacity = label_count;
        m->label_pos  = realloc(m->label_pos,  label_count * sizeof(uint32_t));
        m->label_name = realloc(m->label_name, label_count * sizeof(const char*));
    }
    m->label_count = label_count;

    fread(m->label_pos, sizeof(uint32_t), label_count, f);
    
    for(uint32_t i = 0; i < label_count; i++) {
        uint32_t len = 0;
        fread(&len, sizeof(uint32_t), 1, f);
        if(len > 0) {
            char *str = (char*)malloc(len + 1);
            fread(str, 1, len, f);
            str[len] = '\0';
            m->label_name[i] = str;

        } else {
            m->label_name[i] = NULL;
        }
    }

    fclose(f);
    return m;


}