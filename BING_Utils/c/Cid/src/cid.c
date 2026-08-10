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
#include <limits.h>

#define PROGRAM_NAME "cid"

int main(int argc, char *argv[]) {
    int opt;
    int u_flag = 0, g_flag = 0, G_flag = 0, n_flag = 0;

    while ((opt = getopt(argc, argv, "ugGn")) != -1) {
        switch (opt) {
            case 'u': u_flag = 1; break;
            case 'g': g_flag = 1; break;
            case 'G': G_flag = 1; break;
            case 'n': n_flag = 1; break;
            default:
                fprintf(stderr, "Usage: %s [-u|-g|-G] [-n] [user]\n", PROGRAM_NAME);
                return 1;
        }
    }

    if ((u_flag && g_flag) || (u_flag && G_flag) || (g_flag && G_flag)) {
        fprintf(stderr, "%s: mutually exclusive flags\n", PROGRAM_NAME);
        return 1;
    }

    if (n_flag && !u_flag && !g_flag && !G_flag) {
        fprintf(stderr, "%s: -n requires -u, -g, or -G\n", PROGRAM_NAME);
        return 1;
    }

    uid_t ruid = getuid();
    uid_t euid = geteuid();
    gid_t rgid = getgid();
    gid_t egid = getegid();

    struct passwd *pw = NULL;
    if (optind < argc) {
        pw = getpwnam(argv[optind]);
        if (!pw) {
            fprintf(stderr, "%s: %s: no such user\n", PROGRAM_NAME, argv[optind]);
            return 1;
        }
        euid = ruid = pw->pw_uid;
        egid = rgid = pw->pw_gid;
    } else {
        pw = getpwuid(euid);
    }

    if (u_flag) {
        if (n_flag && pw) {
            fputs(pw->pw_name, stdout);
        } else {
            printf("%u", (unsigned int)euid);
        }
        putchar('\n');
        return 0;
    }

    if (g_flag) {
        struct group *gr = getgrgid(egid);
        if (n_flag && gr) {
            fputs(gr->gr_name, stdout);
        } else {
            printf("%u", (unsigned int)egid);
        }
        putchar('\n');
        return 0;
    }

    gid_t groups[NGROUPS_MAX + 1];
    int ngroups = 0;

    if (optind < argc && pw) {
        ngroups = NGROUPS_MAX;
        if (getgrouplist(pw->pw_name, egid, groups, &ngroups) < 0) {
            ngroups = NGROUPS_MAX;
            fprintf(stderr, "%s: %s: too many groups for static buffer\n", PROGRAM_NAME, pw->pw_name);
            return 1;
        }
    } else {
        ngroups = getgroups(NGROUPS_MAX, groups);
        if (ngroups < 0) {
            perror(PROGRAM_NAME ": getgroups failed");
            return 1;
        }
    } 
    if (G_flag) {
        for (int i = 0; i < ngroups; i++) {
            struct group *gr = getgrgid(groups[i]);
            if (n_flag && gr) {
                fputs(gr->gr_name, stdout);
            } else {
                printf("%u", (unsigned int)groups[i]);
            }
            if (i < ngroups - 1) {
                putchar(' ');
            }
        }
        putchar('\n');
        return 0;
    }

    if (pw) {
        printf("uid=%u(%s)", (unsigned int)ruid, pw->pw_name);
    } else {
        printf("uid=%u", (unsigned int)ruid);
    }

    struct group *gr = getgrgid(rgid);
    if (gr) {
        printf(" gid=%u(%s)", (unsigned int)rgid, gr->gr_name);
    } else {
        printf(" gid=%u", (unsigned int)rgid);
    }

    if (euid != ruid) {
        struct passwd *epw = getpwuid(euid);
        if (epw) {
            printf(" euid=%u(%s)", (unsigned int)euid, epw->pw_name);
        } else {
            printf(" euid=%u", (unsigned int)euid);
        }
    }

    if (egid != rgid) {
        struct group *egr = getgrgid(egid);
        if (egr) {
            printf(" egid=%u(%s)", (unsigned int)egid, egr->gr_name);
        } else {
            printf(" egid=%u", (unsigned int)egid);
        }
    }

    if (ngroups > 0) {
        printf(" groups=");
        for (int i = 0; i < ngroups; i++) {
            struct group *g = getgrgid(groups[i]);
            if (g) {
                printf("%u(%s)", (unsigned int)groups[i], g->gr_name);
            } else {
                printf("%u", (unsigned int)groups[i]);
            }
            if (i < ngroups - 1) {
                putchar(',');
            }
        }
    }
    putchar('\n');
    return 0;
}
