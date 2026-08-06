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
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#define PROGRAM_NAME "cid"

int main(int argc, char *argv[]) {
    int opt;
    int u_flag = 0;
    int g_flag = 0;
    int n_flag = 0;

    while ((opt = getopt(argc, argv, "ugn")) != -1) {
        switch (opt) {
            case 'u': u_flag = 1; break;
            case 'g': g_flag = 1; break;
            case 'n': n_flag = 1; break;
            default:
                fprintf(stderr, "Usage: %s [-u|-g] [-n]\n", PROGRAM_NAME);
                return 1;
        }
    }

    if (u_flag && g_flag) {
        fprintf(stderr, "%s: cannot print both user and group IDs\n", PROGRAM_NAME);
        return 1;
    }

    if (n_flag && !u_flag && !g_flag) {
        fprintf(stderr, "%s: -n must be used with -u or -g\n", PROGRAM_NAME);
        return 1;
    }

    uid_t uid = geteuid();
    gid_t gid = getegid();

    if (u_flag && n_flag) {
        struct passwd *pw = getpwuid(uid);
        if (pw != NULL) {
            fputs(pw->pw_name, stdout);
            putchar('\n');
            return 0;
        }
        perror(PROGRAM_NAME);
        return 1;
    }

    if (u_flag) {
        printf("%u\n", (unsigned int)uid);
        return 0;
    }

    if (g_flag && n_flag) {
        struct group *gr = getgrgid(gid);
        if (gr != NULL) {
            fputs(gr->gr_name, stdout);
            putchar('\n');
            return 0;
        }
        perror(PROGRAM_NAME);
        return 1;
    }

    if (g_flag) {
        printf("%u\n", (unsigned int)gid);
        return 0;
    }

    struct passwd *pw = getpwuid(uid);
    struct group *gr = getgrgid(gid);

    if (pw != NULL) {
        printf("uid=%u(%s)", (unsigned int)uid, pw->pw_name);
    } else {
        printf("uid=%u", (unsigned int)uid);
    }

    if (gr != NULL) {
        printf(" gid=%u(%s)", (unsigned int)gid, gr->gr_name);
    } else {
        printf(" gid=%u", (unsigned int)gid);
    }

    int ngroups = getgroups(0, NULL);
    if (ngroups > 0) {
        gid_t *groups = malloc(ngroups * sizeof(gid_t));
        if (groups != NULL) {
            if (getgroups(ngroups, groups) >= 0) {
                printf(" groups=");
                for (int i = 0; i < ngroups; i++) {
                    struct group *g = getgrgid(groups[i]);
                    if (g != NULL) {
                        printf("%u(%s)", (unsigned int)groups[i], g->gr_name);
                    } else {
                        printf("%u", (unsigned int)groups[i]);
                    }
                    if (i < ngroups - 1) {
                        putchar(',');
                    }
                }
            }
            free(groups);
        }
    }
    putchar('\n');
    return 0;

