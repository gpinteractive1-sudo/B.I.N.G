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
#include <limits.h>
#include <dirent.h>

#define PROGRAM_NAME "cchmod"

static int chmod_recursive(const char *path, mode_t mode) {
    struct stat st;
    if (lstat(path, &st) != 0) {
        perror(path);
        return -1;
    }

    if (chmod(path, mode) != 0) {
        perror(path);
        return -1;
    }

    if (S_ISDIR(st.st_mode)) {
        DIR *dir = opendir(path);
        if (!dir) {
            perror(path);
            return -1;
        }

        struct dirent *entry;
        int res = 0;

        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            char sub_path[PATH_MAX];
            int len = snprintf(sub_path, sizeof(sub_path), "%s/%s", path, entry->d_name);

            if (len >= (int)sizeof(sub_path)) {
                fprintf(stderr, "%s: '%s/%s': File name too long\n", PROGRAM_NAME, path, entry->d_name);
                res = -1;
                continue;
            }

            if (chmod_recursive(sub_path, mode) != 0) {
                res = -1;
            }
        }

        closedir(dir);
        return res;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    int opt;
    int R_flag = 0;

    while((opt = getopt(argc, argv, "R")) != -1){
        switch (opt){
            case 'R':
                R_flag = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-R] mode file...\n", PROGRAM_NAME);
                return 1;
        }
    }

    if (optind + 2 > argc){
        fprintf(stderr, "Usage: %s [-R] mode file...\n", PROGRAM_NAME);
        return 1;
    }
    
    char *endptr;
    long val;

    errno = 0;
    val = strtol(argv[optind], &endptr, 8);

    if(errno != 0 || *endptr != '\0' || endptr == argv[optind] || val < 0 || val > 07777) {
        fprintf(stderr, "%s: invalid mode '%s'\n", PROGRAM_NAME, argv[optind]);
        return 1;
    }
    mode_t mode = (mode_t)val;

    int has_error = 0;

    for(int i = optind + 1; i < argc; i++) {
        if (R_flag) {
            if (chmod_recursive(argv[i], mode) != 0) {
                has_error = 1;
            }
        } else {
            if(chmod(argv[i], mode) != 0){
                perror(argv[i]);
                has_error = 1;
            }
        }
    }
    return has_error ? 1 : 0;
}
