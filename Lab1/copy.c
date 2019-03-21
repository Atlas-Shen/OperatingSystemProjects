#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s [source_file] [target_file]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	//O_RDONLY read only
	//O_WRONLY write only
	//O_RDWR   read & write

	int source_fd = open(argv[1], O_RDONLY);
	if (source_fd == -1) {
		fprintf(stderr, "Fail to open the source file!\n");
		exit(EXIT_FAILURE);
	}

	//S_IRWXU 00700 user(file owner) has read, write, and execute permission
	int target_fd = open(argv[2], O_WRONLY | O_CREAT, S_IRWXU);
	if (target_fd == -1) {
		fprintf(stderr, "Fail to open the target file!\n");
		close(source_fd);
		exit(EXIT_FAILURE);
	}

	char buffer[1024] = {0};
	int read_count = 0;
	while (read_count = read(source_fd, buffer, 1024)) {
		if (read_count == -1) {
			fprintf(stderr, "Fail to read the source file!\n");
			close(source_fd);
			close(target_fd);
			exit(EXIT_FAILURE);
		}

		int write_count = write(target_fd, buffer, read_count);
		if (write_count == -1) {
			fprintf(stderr, "Fail to write the target file!\n");
			close(source_fd);
			close(target_fd);
			exit(EXIT_FAILURE);
		}
		memset(buffer, 0, 1024);

	}
	close(source_fd);
	close(target_fd);

	return 0;
}
