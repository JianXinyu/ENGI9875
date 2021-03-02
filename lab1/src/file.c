#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define SIZE 4096

int main()
{
	int fd = open("hamlet.txt", O_RDONLY); 	// open a file, read only, fails if not found
	if(fd == -1){				// check for errors on open
		perror("Failed to open file\n");
	}				
	
	char buffer[SIZE];
	int bytes_read = read(fd, buffer, SIZE);// read up to 4096 B of its contents into a buffer
	write(STDOUT_FILENO, buffer, bytes_read);// write to the screen
	int result = close(fd); 		// close the file associated with fd
	if(result == -1){			// check for eerors on close
		perror("Failed to close the file\n");
	}
}
