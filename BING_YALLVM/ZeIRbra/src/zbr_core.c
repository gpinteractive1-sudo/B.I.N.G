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

#include "zeirbra_internal.h"
#include <string.h>

ZbrModule* zbr_create(void) {
    ZbrModule *m = (ZbrModule*) calloc(1, sizeof(ZbrModule));
    if(!m) return NULL;
    
    m->label_capacity = 64;
    m->label_count = 0;
    
    
    m->label_pos = (uint32_t*) malloc(m->label_capacity * sizeof(uint32_t));
    m->label_name = (const char**) malloc(m->label_capacity * sizeof(const char*));

    m->capacity = 256;
    m->count = 0;


    m->op = (uint8_t*) malloc(m->capacity * sizeof(uint8_t));
    m->type = (uint8_t*) malloc(m->capacity * sizeof(uint8_t));
    m->arg1 = (uint32_t*) malloc(m->capacity * sizeof(uint32_t));
    m->arg2 = (uint32_t*) malloc(m->capacity * sizeof(uint32_t));
    m->imm = (uint64_t*) malloc(m->capacity * sizeof(uint64_t));

    return m;
}

void zbr_clear(ZbrModule *m) {
    if(!m) return;
    m->count = 0;
    m->label_count = 0;
}

void zbr_destroy(ZbrModule *m) {
    if(!m) return;
    free(m->op);
    free(m->type);
    free(m->arg1);
    free(m->arg2);
    free(m->imm);
    free(m->label_pos);
    free(m->label_name);
    free(m);
}

ZbrInst zbr_const_i(ZbrModule *m, ZbrType type, uint64_t val) {
    return zbr_emit(m, ZBR_OP_CONST, type, 0, 0, val);
}

ZbrInst zbr_const_f(ZbrModule *m, ZbrType type, double val) {
    uint64_t raw_bits = 0;

    if(type == ZBR_F32) {
        float f32 = (float)val;
        memcpy(&raw_bits, &f32, sizeof(float));
    }
    else {
        memcpy(&raw_bits, &val, sizeof(double));
    }

    return zbr_emit(m, ZBR_OP_CONST, type, 0, 0, raw_bits);
}

ZbrInst  zbr_call(ZbrModule *m, ZbrType ret_type, ZbrLabel func_label, uint32_t arg_count) {
    return zbr_emit(m, ZBR_OP_CALL, ret_type, func_label,0, (uint64_t)arg_count);
}

ZbrInst  zbr_allocate(ZbrModule *m, uint32_t size_bytes) {
    return zbr_emit(m, ZBR_OP_ALLOCA, ZBR_PTR, 0, 0, (uint64_t)size_bytes);
}

void zbr_store(ZbrModule *m, ZbrType type, ZbrInst ptr, ZbrInst val) {
    zbr_emit(m, ZBR_OP_STORE, type, ptr, val, 0);
}

ZbrInst zbr_load(ZbrModule *m, ZbrType type, ZbrInst ptr) {
    return zbr_emit(m, ZBR_OP_LOAD, type, ptr, 0, 0);
}

ZbrInst zbr_param(ZbrModule *m, ZbrType type, uint32_t index) {
    return zbr_emit(m, ZBR_OP_PARAM, type, 0, 0, (uint64_t)index);
}

ZbrInst zbr_add(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b) {
    return zbr_emit(m, ZBR_OP_ADD, type, a, b, 0);
}

ZbrInst zbr_sub(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b) {
    return zbr_emit(m, ZBR_OP_SUB, type, a, b, 0);
}

ZbrInst zbr_mul(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b) {
    return zbr_emit(m, ZBR_OP_MUL, type, a, b, 0);
}

ZbrInst zbr_div(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b) {
    return zbr_emit(m, ZBR_OP_DIV, type, a, b, 0);
}

ZbrInst zbr_mod(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b) {
    return zbr_emit(m, ZBR_OP_MOD, type, a, b, 0);
}

ZbrInst zbr_and(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b) {
    return zbr_emit(m, ZBR_OP_AND, type, a, b, 0);
}

ZbrInst zbr_or (ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b) {
    return zbr_emit(m, ZBR_OP_OR, type, a, b, 0);
}

ZbrInst zbr_xor(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b) {
    return zbr_emit(m, ZBR_OP_XOR, type, a, b, 0);
}

ZbrInst zbr_shl(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b) {
    return zbr_emit(m, ZBR_OP_SHL, type, a, b, 0);
}

ZbrInst zbr_shr(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b) {
    return zbr_emit(m, ZBR_OP_SHR, type, a, b, 0);
}

void zbr_ret(ZbrModule *m, ZbrInst val) {
           zbr_emit(m, ZBR_OP_RET, 0, val, 0, 0);
}

ZbrLabel zbr_label_new(ZbrModule *m, const char *name) {
    if(m->label_count >= m->label_capacity) {
        m->label_capacity *= 2;
        m->label_pos = (uint32_t*) realloc(m->label_pos, m->label_capacity * sizeof(uint32_t));
        m->label_name = (const char**) realloc(m->label_name, m->label_capacity * sizeof(const char*));
        
    }

    uint32_t id = m->label_count++;
    m->label_pos[id] = (uint32_t)-1;
    m->label_name[id] = name;
    return id;
}

ZbrInst zbr_cmp_eq(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b) {
      return zbr_emit(m, ZBR_OP_CMP_EQ, type, a, b, 0);
}

ZbrInst zbr_cmp_lt(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b) {
   return zbr_emit(m, ZBR_OP_CMP_LT, type, a, b, 0);
}

void zbr_jmp(ZbrModule *m, ZbrLabel target) {
    zbr_emit(m, ZBR_OP_JMP, 0, target, 0, 0);
}

void zbr_br(ZbrModule *m, ZbrInst cond, ZbrLabel if_true, ZbrLabel if_false) {
    zbr_emit(m, ZBR_OP_BR, 0, cond, if_true, (uint64_t)(if_false));
}

void zbr_arg(ZbrModule *m, ZbrType type, ZbrInst val) {
    zbr_emit(m,ZBR_OP_ARG, type, val, 0, 0);
}

ZbrInst zbr_cmp_ne(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b) {
    return zbr_emit(m, ZBR_OP_CMP_NE, type, a, b, 0);
}

ZbrInst zbr_cmp_gt(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b) {
    return zbr_emit(m, ZBR_OP_CMP_GT, type, a, b, 0);
}

ZbrInst zbr_cmp_le(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b) {
    return zbr_emit(m, ZBR_OP_CMP_LE, type, a, b, 0);
}

ZbrInst zbr_cmp_ge(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b) {
    return zbr_emit(m, ZBR_OP_CMP_GE, type, a, b, 0);
}

ZbrInst zbr_sar(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b) {
    return zbr_emit(m, ZBR_OP_SAR, type, a, b, 0);
}

ZbrInst zbr_zext(ZbrModule *m, ZbrType to_type, ZbrInst val) {
    return zbr_emit(m, ZBR_OP_ZEXT, to_type, val, 0, 0);
}

ZbrInst zbr_sext(ZbrModule *m, ZbrType to_type, ZbrInst val) {
    return zbr_emit(m, ZBR_OP_SEXT, to_type, val, 0, 0);
}

ZbrInst zbr_trunc(ZbrModule *m, ZbrType to_type, ZbrInst val) {
    return zbr_emit(m, ZBR_OP_TRUNC, to_type, val, 0, 0);
}

ZbrInst zbr_fpext(ZbrModule *m, ZbrType to_type, ZbrInst val) {
    return zbr_emit(m, ZBR_OP_FPEXT, to_type, val, 0, 0);
}
ZbrInst zbr_fptrunc(ZbrModule *m, ZbrType to_type, ZbrInst val) {
    return zbr_emit(m, ZBR_OP_FPTRUNC, to_type, val, 0, 0);
}


ZbrInst zbr_sitofp(ZbrModule *m, ZbrType to_type, ZbrInst val) {
    return zbr_emit(m, ZBR_OP_SITOFP, to_type, val, 0, 0);
}

ZbrInst zbr_uitofp(ZbrModule *m, ZbrType to_type, ZbrInst val) {
    return zbr_emit(m, ZBR_OP_UITOFP, to_type, val, 0, 0);
}

ZbrInst zbr_fptosi(ZbrModule *m, ZbrType to_type, ZbrInst val) {
    return zbr_emit(m, ZBR_OP_FPTOSI, to_type, val, 0, 0);
}

ZbrInst zbr_fptoui(ZbrModule *m, ZbrType to_type, ZbrInst val) {
    return zbr_emit(m, ZBR_OP_FPTOUI, to_type, val, 0, 0);
}


ZbrInst zbr_ptrtoint(ZbrModule *m, ZbrType to_type, ZbrInst val) {
    return zbr_emit(m, ZBR_OP_PTRTOINT, to_type, val, 0, 0);
}

ZbrInst zbr_inttoptr(ZbrModule *m, ZbrType to_type, ZbrInst val) {
    return zbr_emit(m, ZBR_OP_INTTOPTR, to_type, val, 0, 0);
}

ZbrInst zbr_bitcast(ZbrModule *m, ZbrType to_type, ZbrInst val) {
    return zbr_emit(m, ZBR_OP_BITCAST, to_type, val, 0, 0);
}


void zbr_label_bind(ZbrModule *m, ZbrLabel label) {
    if(!m || label >= m->label_count) return;
    m->label_pos[label] = m->count;
}
