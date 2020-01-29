#include <unistd.h>
#include <sys/socket.h>

#include "libunix.h"

void my_install_child(int sock_fd, char* argv[]) {
	if(execvp(argv[0], &argv[0]) < 0) {
		exit(10);
	}
}

// fork exec server, create a socket for comparison and dup it to the <to_fd>
int exec_server_socket(int *pid, char **argv, int to_fd) {
	int sock[2];

	if(socketpair(PF_LOCAL, SOCK_STREAM, 0, sock) < 0) {
		sys_die(socketpair, failed);
	}

	if((*pid = fork()) < 0 ){
		sys_die(fork, fork failed);
	} 
	if(*pid != 0) {
		if(close(sock[1]) < 0) {
			sys_die(close, impossible);
		}
		return sock[0];
	} else {
		if(close(sock[0]) < 0) {
			sys_die(close, close failed); 
		}
		if(dup2(sock[1], to_fd) < 0) {
			sys_die(dup, bad dup);
		}
		my_install_child(sock[1], argv);
	}

	for(int i = 1; argv[i]; i++)
        fprintf(stderr, " argv[%d]=<%s> ", i, argv[i]);
    fprintf(stderr, "\n");
    return sock[0];
}
