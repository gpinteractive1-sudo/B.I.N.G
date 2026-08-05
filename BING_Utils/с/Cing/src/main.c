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

#include <stdio.h>    
#include <stdlib.h>   
#include <unistd.h>   
#include <fcntl.h>    
#include <getopt.h>   
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#define PROGRAM_NAME "cing"
#define VERSION "1.0.0"
#define BUFFER_SIZE 4096

void print_help() {
    printf("Usage: %s <input_file>\n", PROGRAM_NAME);
    printf("Options:\n");
    printf("  -h, --help          Show this help message and exit\n");
    printf("  -v, --version       Show program version and exit\n");
}

void print_version() {
    printf("%s version %s\n", PROGRAM_NAME, VERSION);
    printf("\n");
}

static ssize_t safe_read(int fd, void *buf, size_t count) {
    ssize_t r;
    do {
        r = read(fd, buf, count);
    } while (r < 0 && errno == EINTR);
    return r;
}

static ssize_t safe_write_all(int fd, const void *buf, size_t count) {
    const char *cbuf = buf;
    size_t offset = 0;
    while (offset < count) {
        ssize_t w;
        do {
            w = write(fd, cbuf + offset, count - offset);
        } while (w < 0 && errno == EINTR);
        if (w < 0) return -1;
        offset += (size_t)w;
    }
    return (ssize_t)offset;
}

int copy_stream(int src_fd, int dest_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t nread;

    while ((nread = safe_read(src_fd, buffer, sizeof(buffer))) > 0) {
        ssize_t nwritten = safe_write_all(dest_fd, buffer, (size_t)nread);
        if (nwritten < 0) {
            perror(PROGRAM_NAME);
            return -1;
        }
    }

    if (nread < 0) {
        perror(PROGRAM_NAME);
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "hv", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_help();
                return 0;
            case 'v':
                print_version();
                return 0;
            default:
                fprintf(stderr, "Try '%s --help' for help.\n", PROGRAM_NAME);
                return 1;
        }
    }

    bool had_error = false;

    if (optind >= argc) {
        if (copy_stream(STDIN_FILENO, STDOUT_FILENO) < 0) had_error = true;
    } else {
        for (int i = optind; i < argc; i++) {
            if (strcmp(argv[i], "-") == 0) {
                if (copy_stream(STDIN_FILENO, STDOUT_FILENO) < 0) had_error = true;
                continue;
            }

            int input_fd = open(argv[i], O_RDONLY);
            if (input_fd < 0) {
                perror(PROGRAM_NAME);
                had_error = true;
                continue;
            }
            if (copy_stream(input_fd, STDOUT_FILENO) < 0) had_error = true;
            close(input_fd);
        }
    }

    return had_error ? 1 : 0;
}
