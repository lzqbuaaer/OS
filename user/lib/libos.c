#include <env.h>
#include <lib.h>
#include <mmu.h>

void exit(void) {
	// After fs is ready (lab5), all our open files should be closed before dying.
#if !defined(LAB) || LAB >= 5
	close_all();
#endif

	syscall_env_destroy(0);
	user_panic("unreachable code");
}

const volatile struct Env *env;
extern int main(int, char **);

void libmain(int argc, char **argv) {
	// set env to point at our env structure in envs[].
	env = &envs[ENVX(syscall_getenvid())];
	
	set_sig_entry();

	// call user main routine
	main(argc, argv);

	// exit gracefully
	exit();
}

int sigaction(int signum, const struct sigaction *newact, struct sigaction *oldact) {
    if (signum < 1 || signum > 32) {
        return -1;
    }
    if (syscall_get_sig_act(0, signum, oldact) != 0) {
        return -1;
    }
    return syscall_set_sig_act(0, signum, newact);
}

int kill(u_int envid, int sig) {
    return syscall_kill(envid, sig);
}

int sigemptyset(sigset_t *__set) {
    if (__set == NULL) {
        return -1;
    }
    __set->sig = 0;
    return 0;
}

int sigfillset(sigset_t *__set) {
    if (__set == NULL) {
        return -1;
    }
    uint32_t i = 0;
    __set->sig = ~i;
    return 0;
}

int sigaddset(sigset_t *__set, int __signo) {
    if (__set == NULL) {
        return -1;
    }
    if (__signo < 1 || __signo > 32) {
        return -1;
    }
    __set->sig |= SIGSET(__signo);
    return 0;
}

int sigdelset(sigset_t *__set, int __signo) {
    if (__set == NULL) {
        return -1;
    }
    if (__signo < 1 || __signo > 32) {
        return -1;
    }
    __set->sig &= (~SIGSET(__signo));
    return 0;
}

int sigismember(const sigset_t *__set, int __signo) {
    if (__set == NULL) {
        return -1;
    }
    if (__signo < 1 || __signo > 32) {
        return -1;
    }
    return ((SIGSET(__signo) & __set->sig) != 0);
}

int sigisemptyset(const sigset_t *__set) {
    if (__set == NULL) {
        return -1;
    }
    return (__set->sig == 0);
}

int sigandset(sigset_t *__set, const sigset_t *__left, const sigset_t *__right) {
    if (__left == NULL || __right == NULL || __set == NULL) {
        return -1;
    }
    __set->sig = __left->sig & __right->sig;
    return 0;
}

int sigorset(sigset_t *__set, const sigset_t *__left, const sigset_t *__right) {
    if (__left == NULL || __right == NULL || __set == NULL) {
        return -1;
    }
    __set->sig = __left->sig | __right->sig;
    return 0;
}

int sigprocmask(int __how, const sigset_t * __set, sigset_t * __oset) {
    return syscall_sigprocmask(0, __how, __set, __oset);
}

int sigpending(sigset_t *__set) {
    return syscall_sigpending(0, __set);
}


