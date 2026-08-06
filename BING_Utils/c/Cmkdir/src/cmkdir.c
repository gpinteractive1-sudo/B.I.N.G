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
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

#define PROGRAM_NAME "cmkdir"

int make_parents(char *path, mode_t mode) {
    char *p = path;

    if (*p == '/') {
        p++;
    }

    while (*p != '\0') {
        if (*p == '/') {

            if (p == path || *(p - 1) == '/') {
            p++;
            continue;
    }
    
            *p = '\0';

            if (mkdir(path, mode) != 0 && errno != EEXIST) {
                perror(PROGRAM_NAME);
                return -1;
            }

           
            *p = '/';
        }
        p++;
    }

    if (mkdir(path, mode) != 0 && errno != EEXIST) {
        perror(PROGRAM_NAME);
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    int opt;
    int p_flag = 0;
    mode_t mode = 0777;

    while ((opt = getopt(argc, argv, "pm:")) != -1) {
        switch(opt){
          case 'p': 
            p_flag = 1;
            break;
          
          case 'm':
            mode = (mode_t)strtol(optarg, NULL, 8);
            break;
          default:
          fprintf(stderr, "Usage: %s [-p] [-m mode] directory...\n", PROGRAM_NAME);
          return 1;
        }
    }
    if (optind >= argc) {
        fprintf(stderr, "%s: missing operand\n", PROGRAM_NAME);
        return 1;
    }

    int has_error = 0;
    for (int i = optind; i < argc; i++) {
        if (p_flag) {
            if (make_parents(argv[i], mode) != 0) {
                has_error = 1;
            }
        } else {
            if (mkdir(argv[i], mode) != 0) {
                perror(argv[i]);
                has_error = 1;
            }
        }
    }

    return has_error ? 1 : 0;

}