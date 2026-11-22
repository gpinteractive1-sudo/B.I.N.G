#ifndef ZEIRBRA_INTERNAL_H
#define ZEIRBRA_INTERNAL_H


#include "../include/zeirbra.h"
#include <stdlib.h>

enum {
    ZBR_OP_NOP = 0,
    ZBR_OP_ADD,
    ZBR_OP_SUB,
    ZBR_OP_MUL,
    ZBR_OP_DIV,
    ZBR_OP_MOD,
    ZBR_OP_AND,
    ZBR_OP_OR,
    ZBR_OP_XOR,
    ZBR_OP_SHL,
    ZBR_OP_SHR,
    ZBR_OP_CONST,
    ZBR_OP_RET,
    ZBR_OP_CMP_EQ,
    ZBR_OP_CMP_LT,
    ZBR_OP_CMP_NE,
    ZBR_OP_CMP_GT,
    ZBR_OP_CMP_LE,
    ZBR_OP_CMP_GE,
    ZBR_OP_JMP,
    ZBR_OP_BR,
    ZBR_OP_ARG,
    ZBR_OP_CALL,
    ZBR_OP_ALLOCA,
    ZBR_OP_STORE,
    ZBR_OP_LOAD,
    ZBR_OP_PARAM,
    ZBR_OP_SAR

};

struct ZbrModule {
    uint8_t* type;
    uint8_t* op;
    uint32_t* arg1;
    uint32_t* arg2;
    uint64_t* imm;

    uint32_t capacity;
    uint32_t count;

    const char** label_name;
    uint32_t* label_pos;
    uint32_t label_capacity;
    uint32_t label_count;
};

static inline ZbrInst zbr_emit(ZbrModule *m, uint8_t op_code, uint8_t t, uint32_t arg1, uint32_t arg2, uint64_t val) {
    if(m->count >= m->capacity) {
        m->capacity *= 2;
        m->op = (uint8_t*) realloc(m->op, m->capacity * sizeof(uint8_t));
        m->type = (uint8_t*) realloc(m->type, m->capacity * sizeof(uint8_t));
        m->arg1 = (uint32_t*) realloc(m->arg1, m->capacity * sizeof(uint32_t));
        m->arg2 = (uint32_t*) realloc(m->arg2, m->capacity * sizeof(uint32_t));
        m->imm = (uint64_t*) realloc(m->imm, m->capacity * sizeof(uint64_t));


    }

    uint32_t i = m->count;

    m->op[i] = op_code;
    m->type[i] = t;
    m->arg1[i] = arg1;
    m->arg1[i] = arg2;
    m->imm[i] = val;

    m->count++;
    return i;
}
#endif /* ZEIRBRA_INTERNAL_H */