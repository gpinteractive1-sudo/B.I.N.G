#ifndef ZEIRBRA_H
#define ZEIRBRA_H
#include <stdint.h>

#ifdef __cplusplus 
extern "C" { 
#endif

enum {
    ZBR_VOID = 0,
    ZBR_I8 = 1,
    ZBR_I16 = 2,
    ZBR_I32 = 4,
    ZBR_I64 = 8,
    ZBR_PTR = 9,
    ZBR_F32 = 10,
    ZBR_F64 = 11
};

typedef struct ZbrModule ZbrModule;

typedef uint32_t ZbrInst;
typedef uint32_t ZbrLabel;
typedef uint8_t  ZbrType;

ZbrModule* zbr_create(void);
void       zbr_destroy(ZbrModule *m);
void       zbr_clear(ZbrModule *m);

ZbrInst zbr_const_i(ZbrModule *m, ZbrType type, uint64_t val);
ZbrInst zbr_const_f(ZbrModule *m, ZbrType type, double val);

ZbrInst zbr_add(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b);
void    zbr_ret(ZbrModule *m, ZbrInst val);
void    zbr_dump(const ZbrModule *m);
void    zbr_arg(ZbrModule *m, ZbrType type, ZbrInst val);

ZbrLabel zbr_label_new(ZbrModule *m, const char *name);
void     zbr_label_bind(ZbrModule *m, ZbrLabel label);
ZbrInst  zbr_call(ZbrModule *m, ZbrType ret_type, ZbrLabel func_label, uint32_t arg_count);

ZbrInst  zbr_allocate(ZbrModule *m, uint32_t size_bytes);
void     zbr_store(ZbrModule *m, ZbrType type, ZbrInst ptr, ZbrInst val);
ZbrInst  zbr_load(ZbrModule *m, ZbrType type, ZbrInst ptr);

ZbrInst zbr_param(ZbrModule *m, ZbrType type, uint32_t index);

ZbrInst zbr_sub(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b);
ZbrInst zbr_mul(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b);
ZbrInst zbr_div(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b);
ZbrInst zbr_mod(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b);

ZbrInst zbr_and(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b);
ZbrInst zbr_or (ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b);
ZbrInst zbr_xor(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b);
ZbrInst zbr_shl(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b);
ZbrInst zbr_shr(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b);

ZbrInst zbr_cmp_eq(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b);
ZbrInst zbr_cmp_lt(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b);
ZbrInst zbr_cmp_ne(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b);
ZbrInst zbr_cmp_gt(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b);
ZbrInst zbr_cmp_le(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b);
ZbrInst zbr_cmp_ge(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b);
void    zbr_jmp(ZbrModule *m, ZbrLabel target);
void    zbr_br(ZbrModule *m, ZbrInst cond, ZbrLabel if_true, ZbrLabel if_false);

ZbrInst zbr_sar(ZbrModule *m, ZbrType type, ZbrInst a, ZbrInst b);


#ifdef __cplusplus
}
#endif
#endif 