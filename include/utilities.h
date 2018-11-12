
#ifndef UTILITIES_H
#define UTILITIES_H

#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>

#define ERROR "[ERROR] "
#define INFO "[INFO] "
#define WARNING "[WARNING] "


//#define LOG_STDOUT(format, ...)     do{printf("[PID:%d][TID:%ld]",getpid(),syscall(SYS_gettid)); printf(format, ##__VA_ARGS__); fflush(stdout);}while(0)
#define LOG_STDOUT(format, ...)       printf(format, ##__VA_ARGS__)
#define LOG_SYSLOG(format, ...)		

#define SUCCESS ((void*)0)
#define ERR	((void*)1)


void print_scheduler(void);


#endif

