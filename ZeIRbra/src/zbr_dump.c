#include "zeirbra_internal.h"
#include <stdio.h>

static void dump_label_ref(const ZbrModule *m, ZbrLabel l) {
    if (l < m->label_count && m->label_name[l]) printf("$%s", m->label_name[l]);
    else                  printf(".L%u", l);
}

static void dump_labels_at(const ZbrModule *m, uint32_t pos) {
    for (uint32_t l = 0; l < m->label_count; ++l) {
        if (m->label_pos[l] == pos) {
            if (m->label_name[l]) printf("$%s:\n", m->label_name[l]);
            else                  printf(".L%u:\n", l);
        }
    }
}

void zbr_dump(const ZbrModule *m) {
    if(!m) return;
    for(uint32_t i = 0; i < m->count; ++i) {
       dump_labels_at(m, i);

        switch(m->op[i]){
            case ZBR_OP_CONST:
            if(m->type[i] == ZBR_F32 || m->type[i] == ZBR_F64) {
                printf("    MOVF $0x%llx, %%%u\n", (unsigned long long) m->imm[i], i);
            
            } else {
                printf("    MOV $%llu, %%%u\n", (unsigned long long) m->imm[i], i);
            }

            break;

            case ZBR_OP_ADD:
            printf("    ADD %%%u, %%%u, %%%u\n", m->arg1[i], m->arg2[i], i );
            break;

            case ZBR_OP_RET:
            printf("    RET %%%u\n", m->arg1[i]);
            break;
            
            case ZBR_OP_CMP_EQ:
            printf("    CMP_EQ %%%u, %%%u, %%%u\n", m->arg1[i], m->arg2[i], i);
            break;

            case ZBR_OP_CMP_LT:
            printf("    CMP_LT %%%u, %%%u, %%%u\n", m->arg1[i], m->arg2[i], i);
            break;
            
            case ZBR_OP_CMP_NE:
            printf("    CMP_NE %%%u, %%%u, %%%u\n", m->arg1[i], m->arg2[i], i);
            break;

            case ZBR_OP_CMP_GT:
            printf("    CMP_GT %%%u, %%%u, %%%u\n", m->arg1[i], m->arg2[i], i);
            break;

            case ZBR_OP_CMP_GE:
            printf("    CMP_GE %%%u, %%%u, %%%u\n", m->arg1[i], m->arg2[i], i);
            break;

            case ZBR_OP_CMP_LE:
            printf("    CMP_LE %%%u, %%%u, %%%u\n", m->arg1[i], m->arg2[i], i);
            break;

            case ZBR_OP_JMP:
            printf("    JMP     ");
            dump_label_ref(m, m->arg1[i]);
            printf("\n");
            break;

            case ZBR_OP_BR:
            printf("    BR %%%u, ", m->arg1[i]);
            dump_label_ref(m, m->arg2[i]);
            printf(", ");
            dump_label_ref(m, (ZbrLabel)m->imm[i]);
            printf("\n");
            break;

            case ZBR_OP_ARG:
            printf("    ARG %%%u\n", m->arg1[i]);
            break;

            case ZBR_OP_CALL:
            printf("    CALL ");
            dump_label_ref(m, m->arg1[i]);
            printf(", $%llu, %%%u\n", (unsigned long long) m->imm[i], i);
            break;

            case ZBR_OP_ALLOCA:
            printf("    ALLOCA $%llu, %%%u\n", (unsigned long long) m->imm[i], i);
            break;

            case ZBR_OP_STORE:
            printf("    STORE %%%u, (%%%u)\n", m->arg2[i], m->arg1[i]);
            break;

            case ZBR_OP_LOAD:
            printf("    LOAD (%%%u), %%%u\n", m->arg1[i], i);
            break;

            case ZBR_OP_PARAM:
            printf("    PARAM $%llu, %%%u\n", (unsigned long long) m->imm[i], i);
            break;

            case ZBR_OP_SUB: 
            printf("    SUB %%%u, %%%u, %%%u\n", m->arg1[i], m->arg2[i], i);
            break;
          
            case ZBR_OP_MUL: 
            printf("    MUL %%%u, %%%u, %%%u\n", m->arg1[i], m->arg2[i], i); 
            break;

            case ZBR_OP_DIV: 
            printf("    DIV %%%u, %%%u, %%%u\n", m->arg1[i], m->arg2[i], i); 
            break;

            case ZBR_OP_MOD: 
            printf("    MOD %%%u, %%%u, %%%u\n", m->arg1[i], m->arg2[i], i); 
            break;

            case ZBR_OP_AND: 
            printf("    AND %%%u, %%%u, %%%u\n", m->arg1[i], m->arg2[i], i); 
            break;

            case ZBR_OP_OR:  
            printf("    OR  %%%u, %%%u, %%%u\n", m->arg1[i], m->arg2[i], i); 
            break;

            case ZBR_OP_XOR: 
            printf("    XOR %%%u, %%%u, %%%u\n", m->arg1[i], m->arg2[i], i); 
            break;

            case ZBR_OP_SHL: 
            printf("    SHL %%%u, %%%u, %%%u\n", m->arg1[i], m->arg2[i], i); 
            break;
            
            case ZBR_OP_SHR: 
            printf("    SHR %%%u, %%%u, %%%u\n", m->arg1[i], m->arg2[i], i); 
            break;

            case ZBR_OP_SAR:
            printf("    SAR %%%u, %%%u, %%%u\n", m->arg1[i], m->arg2[i], i);
            break;

            }

        }
        dump_labels_at(m, m->count);
    }
    
