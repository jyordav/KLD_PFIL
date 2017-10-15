#include <stdio.h>
#include <fcntl.h>
 
int main(){

	char buf[1024];
	int fd;

	if((fd = open("/dev/FILTER", O_RDONLY)) == -1){
		perror("open");
		exit(1);
	}
	read(fd, buf, sizeof(buf) - 1);
	printf("Received packets: %s \n", buf);
	
	close(fd);
	return 0;

}
