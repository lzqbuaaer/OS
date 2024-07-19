#ifndef __SIGNAL_H__
#define __SIGNAL_H__

typedef struct sigset_t {
    uint32_t sig;
} sigset_t;

struct sigaction {
    void     (*sa_handler)(int);
    sigset_t   sa_mask;
};

#define SIGINT 2
#define SIGILL 4
#define SIGKILL 9
#define SIGSEGV 11
#define SIGCHLD 17
#define SIGSYS 31

#define SIG_BLOCK 0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2

#endif
