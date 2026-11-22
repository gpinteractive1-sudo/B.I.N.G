/*
 * Copyright (c) 2026, GP Interactive (GamePlanet Interactive)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
    m->arg2[i] = arg2;
    m->imm[i] = val;

    m->count++;
    return i;
}
#endif /* ZEIRBRA_INTERNAL_H */
