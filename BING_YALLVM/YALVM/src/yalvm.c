#include "yalvm.h"
#include "zeirbra_internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

uint64_t yalvm_exec(const ZbrModule *m, ZbrLabel entry) {
    if(!m || m->count == 0) return 0;

    uint64_t *regs = (uint64_t*)calloc(m->count, sizeof(uint64_t));


    uint32_t pc = 0;

    if(entry < m->label_count && m->label_pos[entry] != (uint32_t)-1) {
        pc = m->label_pos[entry];
    }

    uint8_t memory[65536] = {0};
    uint32_t sp = 0;

    uint64_t arg_buf[32] = {0};  
    uint32_t arg_count = 0;

    uint32_t call_stack[256] = {0}; 
    uint32_t call_depth = 0;      


while (pc < m->count) {
       switch(m->op[pc]) {
        case ZBR_OP_NOP:
        pc++;
        break;

        case ZBR_OP_CONST:
        regs[pc] = m->imm[pc];
        pc++;
        break;

        case ZBR_OP_ADD:
        regs[pc] = regs[m->arg1[pc]] + regs[m->arg2[pc]];
        pc++;
        break;

        case ZBR_OP_SUB:
        regs[pc] =  regs[m->arg1[pc]] - regs[m->arg2[pc]];
        pc++;
        break;

        case ZBR_OP_MUL:
        regs[pc] =  regs[m->arg1[pc]] * regs[m->arg2[pc]];
        pc++;
        break;

        case ZBR_OP_DIV:
        if(regs[m->arg2[pc]] == 0) {
            printf("YALVM error: division by zero at %%%u\n", pc);
            free(regs);
            return 0;
        }

        regs[pc] =  regs[m->arg1[pc]] / regs[m->arg2[pc]];
        pc++;
        break;

        case ZBR_OP_MOD:
         if(regs[m->arg2[pc]] == 0) {
            printf("YALVM error: modulo by zero at %%%u\n", pc);
            free(regs);
            return 0;
        }
        regs[pc] =  (int64_t)regs[m->arg1[pc]] % (int64_t)regs[m->arg2[pc]];
        pc++;
        break;

        case ZBR_OP_CMP_EQ:
        regs[pc] = (regs[m->arg1[pc]]) == (regs[m->arg2[pc]]) ? 1 : 0;
        pc++;
        break;

        case ZBR_OP_CMP_NE:
        regs[pc] = (regs[m->arg1[pc]]) != (regs[m->arg2[pc]]) ? 1 : 0;
        pc++;
        break;

        case ZBR_OP_CMP_LT:
        regs[pc] =  ((int64_t)regs[m->arg1[pc]]  < (int64_t)regs[m->arg2[pc]]) ? 1 : 0;
        pc++;
        break;
       
        case ZBR_OP_CMP_GT:
        regs[pc] =  ((int64_t)regs[m->arg1[pc]]  > (int64_t)regs[m->arg2[pc]]) ? 1 : 0;
        pc++;
        break;

        case ZBR_OP_CMP_LE:
        regs[pc] =  ((int64_t)regs[m->arg1[pc]]  <= (int64_t)regs[m->arg2[pc]]) ? 1 : 0;
        pc++;
        break;

        case ZBR_OP_CMP_GE:
        regs[pc] =  ((int64_t)regs[m->arg1[pc]]  >= (int64_t)regs[m->arg2[pc]]) ? 1 : 0;
        pc++;
        break;

        case ZBR_OP_JMP:
        pc = m->label_pos[m->arg1[pc]];
        break;

        case ZBR_OP_BR:
        if(regs[m->arg1[pc]]) {
            pc = m->label_pos[m->arg2[pc]];
        } else {
            pc = m->label_pos[(ZbrLabel)m->imm[pc]];
        }
        break;

        case ZBR_OP_ALLOCA:
        regs[pc] = sp;
        sp += (uint32_t) m->imm[pc];
        pc++;
        break;

        case ZBR_OP_STORE: {
        uint32_t addr = (uint32_t)regs[m->arg1[pc]];
        uint64_t val  = regs[m->arg2[pc]];
        uint8_t  t    = m->type[pc];
        size_t size = (t == ZBR_PTR || t == ZBR_F64) ? 8 : (t == ZBR_F32 ? 4 : t);
        memcpy(&memory[addr], &val, size);
        pc++;
        break;
        }


        case ZBR_OP_LOAD: {
        uint32_t addr = (uint32_t)regs[m->arg1[pc]];
        uint8_t  t    = m->type[pc];
        size_t size = (t == ZBR_PTR || t == ZBR_F64) ? 8 : (t == ZBR_F32 ? 4 : t);
        regs[pc] = 0; 
        memcpy(&regs[pc], &memory[addr], size);
        pc++;
        break;
        }

        case ZBR_OP_ARG:
        arg_buf[arg_count++] = regs[m->arg1[pc]];
        pc++;
        break;

        case ZBR_OP_CALL:        
        call_stack[call_depth++] = pc;
        arg_count = 0; 
        pc = m->label_pos[m->arg1[pc]];
        break;

        case ZBR_OP_PARAM:
        regs[pc] = arg_buf[(uint32_t)m->imm[pc]];
        pc++;
        break;

        case ZBR_OP_RET: {
        uint64_t result = regs[m->arg1[pc]];
        if (call_depth > 0) {
        uint32_t caller_pc = call_stack[--call_depth];
        regs[caller_pc] = result;
        pc = caller_pc + 1;  
        break;
        }

    
            free(regs);
            return result;
        }

        case ZBR_OP_ZEXT:
        case ZBR_OP_PTRTOINT:
        case ZBR_OP_INTTOPTR:
        case ZBR_OP_BITCAST:
        regs[pc] = regs[m->arg1[pc]];
        pc++;
        break;

        case ZBR_OP_TRUNC:
        if (m->type[pc] == ZBR_I8)       regs[pc] = regs[m->arg1[pc]] & 0xFF;
        else if (m->type[pc] == ZBR_I16) regs[pc] = regs[m->arg1[pc]] & 0xFFFF;
        else if (m->type[pc] == ZBR_I32) regs[pc] = regs[m->arg1[pc]] & 0xFFFFFFFF;
        pc++;
        break;

        case ZBR_OP_SEXT:
        if (m->type[pc] == ZBR_I64)      regs[pc] = (uint64_t)(int64_t)(int32_t)regs[m->arg1[pc]];
        else if (m->type[pc] == ZBR_I32) regs[pc] = (uint64_t)(int32_t)(int16_t)regs[m->arg1[pc]];
        else                             regs[pc] = (uint64_t)(int16_t)(int8_t)regs[m->arg1[pc]];
        pc++;
        break;

        case ZBR_OP_SITOFP: {
        if (m->type[pc] == ZBR_F64) {
        double d = (double)(int64_t)regs[m->arg1[pc]];
        memcpy(&regs[pc], &d, sizeof(double));
        } else {
        float f = (float)(int64_t)regs[m->arg1[pc]];
        memcpy(&regs[pc], &f, sizeof(float));
        }
        pc++;
        break;
    }

        case ZBR_OP_FPTOSI: {
        double d = 0;
        memcpy(&d, &regs[m->arg1[pc]], sizeof(double));
        regs[pc] = (uint64_t)(int64_t)d;
        pc++;
        break;
    }
        case ZBR_OP_FPEXT: {
            float f = 0;
            memcpy(&f, &regs[m->arg1[pc]], sizeof(float));
            double d = (double)f;
            memcpy(&regs[pc], &d, sizeof(double));
            pc++;
            break;
        }

        case ZBR_OP_FPTRUNC: {
            double d = 0;
            memcpy(&d, &regs[m->arg1[pc]], sizeof(double));
            float f = (float)d;
            regs[pc] = 0;
            memcpy(&regs[pc], &f, sizeof(float));
            pc++;
            break;
        }

        case ZBR_OP_UITOFP: {
            if (m->type[pc] == ZBR_F64) {
                double d = (double)regs[m->arg1[pc]];
                memcpy(&regs[pc], &d, sizeof(double));
            } else {
                float f = (float)regs[m->arg1[pc]];
                regs[pc] = 0;
                memcpy(&regs[pc], &f, sizeof(float));
            }
            pc++;
            break;
        }

        case ZBR_OP_FPTOUI: {
            double d = 0;
            memcpy(&d, &regs[m->arg1[pc]], sizeof(double));
            regs[pc] = (uint64_t)d;
            pc++;
            break;
        }

        case ZBR_OP_AND:
        regs[pc] = regs[m->arg1[pc]] & regs[m->arg2[pc]];
        pc++;
        break;

        case ZBR_OP_OR:
        regs[pc] = regs[m->arg1[pc]] | regs[m->arg2[pc]];
        pc++;
        break;

        case ZBR_OP_XOR:
        regs[pc] = regs[m->arg1[pc]] ^ regs[m->arg2[pc]];
        pc++;
        break;

        case ZBR_OP_SHL:
        regs[pc] = regs[m->arg1[pc]] << (regs[m->arg2[pc]] & 63);
        pc++;
        break;

        case ZBR_OP_SHR:
        regs[pc] = regs[m->arg1[pc]] >> regs[m->arg2[pc]];
        pc++;
        break;

        case ZBR_OP_SAR:
        regs[pc] = (uint64_t)((int64_t)regs[m->arg1[pc]] >> regs[m->arg2[pc]]);
        pc++;
        break;

        


        default:
        printf("YALVM: unknown instruction %u\n", m->op[pc]);
        free(regs);
        return 0;

    }

 }

free(regs);
return 0;

}