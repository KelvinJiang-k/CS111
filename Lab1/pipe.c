#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	if (argc == 1) {
		perror("must contain at least 1 argument");
		return EINVAL;
	}

	errno = 0;
	int pfd[2];
	int i;

	for (i = 1; i < argc - 1; i++) {
		// create pipe
		if (pipe(pfd) == -1) {
			perror("error: pipe");
			return errno;
		}

		// create fork
		pid_t pid = fork();
		if (pid == -1) {
			perror("error: fork");
			return errno;
		}

		if (pid == 0) {
			// child process

			// close read pipe
			close(pfd[0]);

			// redirect stdout to write
			if (dup2(pfd[1], 1) == -1) {
				perror("error: dup2");
				return errno;
			}

			// close write pipe
			close(pfd[1]);

			// run commands
			if (execlp(argv[i], argv[i], NULL) == -1) {
				perror("error: execlp");
				return errno;
			}
		}
		else {
			// parent process

			// close write pipe
			close(pfd[1]);

			// redirect stdin to read
			if (dup2(pfd[0], 0) == -1) {
				perror("error: dup2");
				return errno;
			}

			// close read pipe
			close(pfd[0]);

			if (wait(NULL) == -1) {
				perror("error: wait");
				return errno;
			}
		}
	}

	// run last command
	if (execlp(argv[i], argv[i], NULL) == -1) {
		perror("error: execlp");
		return errno;
	}

	return 0;
}
