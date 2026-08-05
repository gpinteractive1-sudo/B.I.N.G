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
#include <string.h>
#include <getopt.h> 

#define PROGRAM_NAME "ccommand"
#define VERSION "1.0.0"
#define POSIX_DEF_PATH "/usr/bin:/bin:/usr/sbin:/sbin"

void print_help() {
    printf("Usage: %s [-p] command_name [argument...]\n", PROGRAM_NAME);
    printf("Options:\n");
    printf("  -h, --help     Show this help message and exit\n");
    printf("  -v, --version  Show program version and exit\n");
    printf("  -p             Use default POSIX PATH to find commands\n");
}

void print_version() {
    printf("%s version %s\n", PROGRAM_NAME, VERSION);
}

int main(int argc, char *argv[]) {

    struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };
    
    int opt;
    int p_flag = 0;

  
    while ((opt = getopt_long(argc, argv, "+pvh", long_options, NULL)) != -1) {
        switch (opt) {
            case 'p':
                p_flag = 1;
                break;
            case 'v':
                print_version();
                return 0;
            case 'h':
                print_help();
                return 0;
            default:
                fprintf(stderr, "Usage: %s [-p] command_name [argument...]\n", argv[0]);
                return 127;
        }
    }

    if (optind >= argc) {
        return 0;
    }

    if (p_flag) {
        if (setenv("PATH", POSIX_DEF_PATH, 1) != 0) {
            perror("ccommand: setenv failed");
            return 126;
        }
    }

    execvp(argv[optind], &argv[optind]);

    fprintf(stderr, "ccommand: %s: command not found\n", argv[optind]);
    return 127;
}
