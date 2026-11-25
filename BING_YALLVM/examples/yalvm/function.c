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

#include "zeirbra.h"
#include "yalvm.h"
#include <stdio.h>

int main() {
    ZbrModule *m = zbr_create();

    ZbrLabel fn_mul = zbr_label_new(m, "multiply");
    ZbrLabel entry =  zbr_label_new(m, "main");

    zbr_label_bind(m, fn_mul);
    ZbrInst a = zbr_param(m, ZBR_I64, 0);
    ZbrInst b = zbr_param(m, ZBR_I64, 1);
    ZbrInst result = zbr_mul(m, ZBR_I64, a, b);
    zbr_ret(m, result);

    /* Entry point: main */
    zbr_label_bind(m, entry);
    ZbrInst x = zbr_const_i(m, ZBR_I64, 6);
    ZbrInst y = zbr_const_i(m, ZBR_I64, 7);


    /* Push arguments and call multiply(6, 7) */
    zbr_arg(m, ZBR_I64, x);
    zbr_arg(m, ZBR_I64, y);
    ZbrInst product = zbr_call(m, ZBR_I64, fn_mul, 2);
    zbr_ret(m, product);

    zbr_dump(m);

    uint64_t out = yalvm_exec(m, entry);
    printf(" 6 * 7 = %llu\n", (unsigned long long) out);

    zbr_destroy(m);


    

}