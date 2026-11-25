#include <math.h>
#include "yalvm.h"
#include "zeirbra_internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define REG(idx) regs[reg_base + (idx)]

#define MAX_FRAMES 256
#define MEM_SIZE   65536

typedef struct {
    uint32_t caller_pc;
    uint32_t saved_sp;
    uint32_t reg_base;   
} CallFrame;

static inline float u32_to_f32(uint64_t r) {
    uint32_t bits = (uint32_t)r;
    float f;
    memcpy(&f, &bits, sizeof(float));
    return f;
}

static inline uint64_t f32_to_u64(float f) {
    uint32_t bits = 0;
    uint64_t out = 0;
    memcpy(&bits, &f, sizeof(float));
    memcpy(&out, &bits, sizeof(float)); 
    return out;
}

static inline double u64_to_f64(uint64_t r) {
    double d;
    memcpy(&d, &r, sizeof(double));
    return d;
}

static inline uint64_t f64_to_u64(double d) {
    uint64_t out = 0;
    memcpy(&out, &d, sizeof(double));
    return out;
}

uint64_t yalvm_exec(const ZbrModule *m, ZbrLabel entry) {
    if(!m || m->count == 0) return 0;

    uint64_t *regs = (uint64_t*)calloc((size_t)m->count * MAX_FRAMES, sizeof(uint64_t));
    
    if (!regs) return 0;
    uint32_t reg_base = 0;


    uint32_t pc = 0;

    if(entry < m->label_count && m->label_pos[entry] != (uint32_t)-1) {
        pc = m->label_pos[entry];
    }

    uint8_t *memory = (uint8_t*)calloc(MEM_SIZE, sizeof(uint8_t));
    if (!memory) {
    free(regs);
    return 0;
    }

    uint32_t sp = 0;

    uint64_t arg_buf[32] = {0};  
    uint32_t arg_count = 0;

    CallFrame call_stack[MAX_FRAMES] = {0}; 
    uint32_t call_depth = 0;      


while (pc < m->count) {
       switch(m->op[pc]) {
        case ZBR_OP_NOP:
        pc++;
        break;

        case ZBR_OP_CONST:
        REG(pc) = m->imm[pc];
        pc++;
        break;

        case ZBR_OP_ADD:
        if (m->type[pc] == ZBR_F64) {
            REG(pc) = f64_to_u64(u64_to_f64(REG(m->arg1[pc])) + u64_to_f64(REG(m->arg2[pc])));
        } else if (m->type[pc] == ZBR_F32) {
            REG(pc) = f32_to_u64(u32_to_f32(REG(m->arg1[pc])) + u32_to_f32(REG(m->arg2[pc])));
        } else {
            REG(pc) = REG(m->arg1[pc]) + REG(m->arg2[pc]);
        }
        pc++;
        break;

        case ZBR_OP_SUB:
        if (m->type[pc] == ZBR_F64) {
            REG(pc) = f64_to_u64(u64_to_f64(REG(m->arg1[pc])) - u64_to_f64(REG(m->arg2[pc])));
        } else if (m->type[pc] == ZBR_F32) {
            REG(pc) = f32_to_u64(u32_to_f32(REG(m->arg1[pc])) - u32_to_f32(REG(m->arg2[pc])));
        } else {
            REG(pc) = REG(m->arg1[pc]) - REG(m->arg2[pc]);
        }
        pc++;
        break;

        case ZBR_OP_MUL:
      if (m->type[pc] == ZBR_F64) {
         REG(pc) = f64_to_u64(u64_to_f64(REG(m->arg1[pc]))  * u64_to_f64(REG(m->arg2[pc])));
        } else if (m->type[pc] == ZBR_F32) {
           REG(pc) = f32_to_u64(u32_to_f32(REG(m->arg1[pc])) * u32_to_f32(REG(m->arg2[pc])));
        } else {
            REG(pc) = REG(m->arg1[pc]) * REG(m->arg2[pc]);
        }
        pc++;
        break;

        case ZBR_OP_DIV:
        if (m->type[pc] == ZBR_F64) {
            REG(pc) = f64_to_u64(u64_to_f64(REG(m->arg1[pc])) / u64_to_f64(REG(m->arg2[pc])));
        } else if (m->type[pc] == ZBR_F32) {
            REG(pc) = f32_to_u64(u32_to_f32(REG(m->arg1[pc])) / u32_to_f32(REG(m->arg2[pc])));
        } else {    
            if(REG(m->arg2[pc]) == 0) {
                printf("YALVM error: division by zero at %%%u\n", pc);
                free(memory);
                free(regs);
                return 0;
        }
        REG(pc) = REG(m->arg1[pc]) / REG(m->arg2[pc]);
    }
        pc++;
        break;

        case ZBR_OP_MOD:
        if (m->type[pc] == ZBR_F64) {
            REG(pc) = f64_to_u64(fmod(u64_to_f64(REG(m->arg1[pc])), u64_to_f64(REG(m->arg2[pc]))));
      } else if (m->type[pc] == ZBR_F32) {
            REG(pc) = f32_to_u64(fmodf(u32_to_f32(REG(m->arg1[pc])), u32_to_f32(REG(m->arg2[pc]))));
      } else {
        if (REG(m->arg2[pc]) == 0) {
            printf("YALVM error: modulo by zero at %%%u\n", pc);
            free(memory);
            free(regs);
         return 0;
    }
        REG(pc) = (uint64_t)((int64_t)REG(m->arg1[pc]) % (int64_t)REG(m->arg2[pc]));
}
        pc++;
        break;

        case ZBR_OP_CMP_EQ:
        if(m->type[pc] == ZBR_F64) {
            REG(pc) = (u64_to_f64(REG(m->arg1[pc])) == u64_to_f64(REG(m->arg2[pc]))) ? 1 : 0;
        }
        else if(m->type[pc] == ZBR_F32) {
            REG(pc) = (u32_to_f32(REG(m->arg1[pc])) == u32_to_f32(REG(m->arg2[pc]))) ? 1 : 0;
        }
        else {
            REG(pc) = (REG(m->arg1[pc])) == (REG(m->arg2[pc])) ? 1 : 0;
        }
        pc++;
        break;

        case ZBR_OP_CMP_NE:
        if (m->type[pc] == ZBR_F64) {
            REG(pc) = (u64_to_f64(REG(m->arg1[pc])) != u64_to_f64(REG(m->arg2[pc]))) ? 1 : 0;
        } else if (m->type[pc] == ZBR_F32) {
            REG(pc) = (u32_to_f32(REG(m->arg1[pc])) != u32_to_f32(REG(m->arg2[pc]))) ? 1 : 0;
        }
        else {
        REG(pc) = (REG(m->arg1[pc])) != (REG(m->arg2[pc])) ? 1 : 0;
        }
        pc++;
        break;

        case ZBR_OP_CMP_LT:
        if (m->type[pc] == ZBR_F64) {
            REG(pc) = (u64_to_f64(REG(m->arg1[pc])) < u64_to_f64(REG(m->arg2[pc]))) ? 1 : 0;
        } else if (m->type[pc] == ZBR_F32) {
            REG(pc) = (u32_to_f32(REG(m->arg1[pc])) < u32_to_f32(REG(m->arg2[pc]))) ? 1 : 0;
        } else {
        REG(pc) =  ((int64_t)REG(m->arg1[pc])  < (int64_t)REG(m->arg2[pc])) ? 1 : 0;
        }
        pc++;
        break;
       
        case ZBR_OP_CMP_GT:
        if (m->type[pc] == ZBR_F64) {
            REG(pc) = (u64_to_f64(REG(m->arg1[pc])) > u64_to_f64(REG(m->arg2[pc]))) ? 1 : 0;
        } else if (m->type[pc] == ZBR_F32) {
            REG(pc) = (u32_to_f32(REG(m->arg1[pc])) > u32_to_f32(REG(m->arg2[pc]))) ? 1 : 0;
        } else {    
        REG(pc) =  ((int64_t)REG(m->arg1[pc])  > (int64_t)REG(m->arg2[pc])) ? 1 : 0;
        }
        pc++;
        break;

        case ZBR_OP_CMP_LE:
        if (m->type[pc] == ZBR_F64) {
            REG(pc) = (u64_to_f64(REG(m->arg1[pc])) <= u64_to_f64(REG(m->arg2[pc]))) ? 1 : 0;
        } else if (m->type[pc] == ZBR_F32) {
            REG(pc) = (u32_to_f32(REG(m->arg1[pc])) <= u32_to_f32(REG(m->arg2[pc]))) ? 1 : 0;
        } else {
        REG(pc) =  ((int64_t)REG(m->arg1[pc])  <= (int64_t)REG(m->arg2[pc])) ? 1 : 0;
        }
        pc++;
        break;

        case ZBR_OP_CMP_GE:
        if (m->type[pc] == ZBR_F64) {
            REG(pc) = (u64_to_f64(REG(m->arg1[pc])) >= u64_to_f64(REG(m->arg2[pc]))) ? 1 : 0;
        } else if (m->type[pc] == ZBR_F32) {
            REG(pc) = (u32_to_f32(REG(m->arg1[pc])) >= u32_to_f32(REG(m->arg2[pc]))) ? 1 : 0;
        } else {
            REG(pc) =  ((int64_t)REG(m->arg1[pc])  >= (int64_t)REG(m->arg2[pc])) ? 1 : 0;
        }
        pc++;
        break;

        case ZBR_OP_JMP:
        pc = m->label_pos[m->arg1[pc]];
        break;

        case ZBR_OP_BR:
        if(REG(m->arg1[pc])) {
            pc = m->label_pos[m->arg2[pc]];
        } else {
            pc = m->label_pos[(ZbrLabel)m->imm[pc]];
        }
        break;

        case ZBR_OP_ALLOCA:
        if (sp + (uint32_t)m->imm[pc] > 0xFFFE) {
            printf("YALVM error: stack memory overflow at %%%u\n", pc);
            free(memory);
            free(regs);
            return 0;
        }
        REG(pc) = sp;
        sp += (uint32_t)m->imm[pc];
        pc++;
        break;

        case ZBR_OP_STORE: {
        uint32_t addr = (uint32_t)REG(m->arg1[pc]);
        uint64_t val  = REG(m->arg2[pc]);
        uint8_t  t    = m->type[pc];

        
        if (addr == 0xFFFF) {
                putchar((int)(val & 0xFF));
                fflush(stdout);
                pc++;
                break;
        }

        size_t size = (t == ZBR_PTR || t == ZBR_F64) ? 8 : (t == ZBR_F32 ? 4 : t);
        if (addr + size > 0xFFFE) {
                printf("YALVM error: out-of-bounds store at %%%u\n", pc);
                free(memory);
                free(regs);
                return 0;
            }
        memcpy(&memory[addr], &val, size);
        pc++;
        break;
        }


        case ZBR_OP_LOAD: {
        uint32_t addr = (uint32_t)REG(m->arg1[pc]);
        uint8_t  t    = m->type[pc];
        if (addr == 0xFFFE) {
                int ch = getchar();
                REG(pc) = (ch == EOF) ? (uint64_t)-1 : (uint64_t)(uint8_t)ch;
                pc++;
                break;
            }

        size_t size = (t == ZBR_PTR || t == ZBR_F64) ? 8 : (t == ZBR_F32 ? 4 : t);
        if (addr + size > 0xFFFE) {
                printf("YALVM error: out-of-bounds load at %%%u\n", pc);
                free(memory);
                free(regs);
                return 0;
            }
        REG(pc) = 0; 
        memcpy(&REG(pc), &memory[addr], size);
        pc++;
        break;
        }

        case ZBR_OP_ARG:
        arg_buf[arg_count++] = REG(m->arg1[pc]);
        pc++;
        break;

        case ZBR_OP_CALL:        
        if (call_depth >= MAX_FRAMES - 1) {
        printf("YALVM error: call stack overflow at %%%u\n", pc);
        free(memory);
        free(regs);
        return 0;
        }
        call_stack[call_depth].caller_pc = pc;
        call_stack[call_depth].saved_sp  = sp;
        call_stack[call_depth].reg_base  = reg_base;
        call_depth++;

        reg_base += m->count;
        memset(&regs[reg_base], 0, m->count * sizeof(uint64_t));
        arg_count = 0; 
        pc = m->label_pos[m->arg1[pc]];
        break;

        case ZBR_OP_PARAM:
        REG(pc) = arg_buf[(uint32_t)m->imm[pc]];
        pc++;
        break;

        case ZBR_OP_RET: {
        uint64_t result = REG(m->arg1[pc]);
        if (call_depth > 0) {
        call_depth--;
        uint32_t caller_pc = call_stack[call_depth].caller_pc;
        sp       = call_stack[call_depth].saved_sp;
        reg_base = call_stack[call_depth].reg_base;
        REG(caller_pc) = result;
        pc = caller_pc + 1;  
        break;
        }
        free(memory);
        free(regs);
        return result;
        }

        case ZBR_OP_ZEXT:
        case ZBR_OP_PTRTOINT:
        case ZBR_OP_INTTOPTR:
        case ZBR_OP_BITCAST:
        REG(pc) = REG(m->arg1[pc]);
        pc++;
        break;

        case ZBR_OP_TRUNC:
        if (m->type[pc] == ZBR_I8)       REG(pc) = REG(m->arg1[pc]) & 0xFF;
        else if (m->type[pc] == ZBR_I16) REG(pc) = REG(m->arg1[pc]) & 0xFFFF;
        else if (m->type[pc] == ZBR_I32) REG(pc) = REG(m->arg1[pc]) & 0xFFFFFFFF;
        pc++;
        break;

        case ZBR_OP_SEXT:
        if (m->type[pc] == ZBR_I64)      REG(pc) = (uint64_t)(int64_t)(int32_t)REG(m->arg1[pc]);
        else if (m->type[pc] == ZBR_I32) REG(pc) = (uint64_t)(int32_t)(int16_t)REG(m->arg1[pc]);
        else                             REG(pc) = (uint64_t)(int16_t)(int8_t)REG(m->arg1[pc]);
        pc++;
        break;

        case ZBR_OP_SITOFP: {
        if (m->type[pc] == ZBR_F64) {
        double d = (double)(int64_t)REG(m->arg1[pc]);
        memcpy(&REG(pc), &d, sizeof(double));
        } else {
        float f = (float)(int64_t)REG(m->arg1[pc]);
        memcpy(&REG(pc), &f, sizeof(float));
        }
        pc++;
        break;
    }

        case ZBR_OP_FPTOSI: {
        if (m->type[pc] == ZBR_F32) {
            float f = u32_to_f32(REG(m->arg1[pc]));
            REG(pc) = (uint64_t)(int64_t)f;
        } else {
            double d = u64_to_f64(REG(m->arg1[pc]));
            REG(pc) = (uint64_t)(int64_t)d;
        }
        pc++;
        break;
    }
        case ZBR_OP_FPEXT: {
            float f = 0;
            memcpy(&f, &REG(m->arg1[pc]), sizeof(float));
            double d = (double)f;
            memcpy(&REG(pc), &d, sizeof(double));
            pc++;
            break;
        }

        case ZBR_OP_FPTRUNC: {
            double d = 0;
            memcpy(&d, &REG(m->arg1[pc]), sizeof(double));
            float f = (float)d;
            REG(pc) = 0;
            memcpy(&REG(pc), &f, sizeof(float));
            pc++;
            break;
        }

        case ZBR_OP_UITOFP: {
            if (m->type[pc] == ZBR_F64) {
            double d = (double)REG(m->arg1[pc]);
            REG(pc) = f64_to_u64(d);
        } else {
            float f = (float)REG(m->arg1[pc]);
            REG(pc) = f32_to_u64(f);
        }
            pc++;
            break;
        }

        case ZBR_OP_FPTOUI: {
            if (m->type[pc] == ZBR_F32) {
                float f = u32_to_f32(REG(m->arg1[pc]));
                REG(pc) = (uint64_t)f;
            } else {
                double d = 0;
                memcpy(&d, &REG(m->arg1[pc]), sizeof(double));
                REG(pc) = (uint64_t)d;
        }
            pc++;
            break;
        }

        case ZBR_OP_AND:
        REG(pc) = REG(m->arg1[pc]) & REG(m->arg2[pc]);
        pc++;
        break;

        case ZBR_OP_OR:
        REG(pc) = REG(m->arg1[pc]) | REG(m->arg2[pc]);
        pc++;
        break;

        case ZBR_OP_XOR:
        REG(pc) = REG(m->arg1[pc]) ^ REG(m->arg2[pc]);
        pc++;
        break;

        case ZBR_OP_SHL:
        REG(pc) = REG(m->arg1[pc]) << (REG(m->arg2[pc]) & 63);
        pc++;
        break;

        case ZBR_OP_SHR:
        REG(pc) = REG(m->arg1[pc]) >> REG(m->arg2[pc]);
        pc++;
        break;

        case ZBR_OP_SAR:
        REG(pc) = (uint64_t)((int64_t)REG(m->arg1[pc]) >> REG(m->arg2[pc]));
        pc++;
        break;

        


        default:
        printf("YALVM: unknown instruction %u\n", m->op[pc]);
        free(memory);
        free(regs);
        return 0;

    }

 }
free(memory);
free(regs);
return 0;

}
