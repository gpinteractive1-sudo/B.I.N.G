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
#include <time.h>
#include <errno.h>


#define PROGRAM_NAME "csleep"


int main(int argc, char *argv[]) {

    if(argc != 2 ){
        fprintf(stderr, "Usage: %s seconds\n", PROGRAM_NAME);
        return 1;

    }
    char *endptr;
    double seconds = strtod(argv[1], &endptr);
    
    if(*endptr != '\0'){
        fprintf(stderr, "%s: invalid time interval '%s'\n", PROGRAM_NAME, argv[1]);
        return 1;

    }

    if(seconds < 0.0 ){
        fprintf(stderr, "%s: invalid time interval '%s'\n", PROGRAM_NAME, argv[1]);
        return 1;
    }
    
    struct timespec req;
    struct timespec rem;
    req.tv_sec = (time_t)seconds;
    req.tv_nsec = (long)((seconds - req.tv_sec) * 1000000000); 

    while(nanosleep(&req, &rem) == -1){
    if (errno == EINTR)
    {
        req = rem;

    } else {
        perror(PROGRAM_NAME ": nanosleep failed");
        return 1;
    }
  }
    
    return 0;
    
}
