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

/* MMIO address for stdout character output (MEM_SIZE - 1) */
#define MMIO_PUTCHAR (2 * 1024 * 1024 - 1)

int main() {
    ZbrModule *m = zbr_create();

    ZbrLabel entry = zbr_label_new(m, "main");
    zbr_label_bind(m, entry);

    /* Load the MMIO port address into a virtual register */
    ZbrInst io_port = zbr_const_i(m, ZBR_PTR, MMIO_PUTCHAR);

    /* Emit a 1-byte store to MMIO_PUTCHAR for each character */
    const char *msg = "Hello, B.I.N.G!\n";
    for(size_t i = 0; msg[i] != '\0'; i++) {
        ZbrInst ch = zbr_const_i(m, ZBR_I8, (uint8_t)msg[i]);
        zbr_store(m, ZBR_I8, io_port, ch);

    }

    zbr_ret(m, zbr_const_i(m, ZBR_I64, 0));

    /* Execute in YALVM — output appears directly from VM memory writes */
    yalvm_exec(m, entry);

    zbr_destroy(m);

    return 0;
}