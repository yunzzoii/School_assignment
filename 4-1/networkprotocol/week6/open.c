#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

int main(){
    int fd, n, ret;

    char buf[128];
    struct timeval tv;

    fd_set readfs;
    fd = open("write.txt", O_RDONLY); //3을 받았다면 배열에 0, 1, 2, 3이 들어가 뒤에 fd+1 하는거

    if(fd < 0){
        perror("file open error");
        exit(1);
    }

    memset(buf, 0x00, 128);

    FD_ZERO(&readfs); //fd 초기화
    while(1){ //fd할당
        FD_SET(fd, &readfs);
        ret = select(fd+1, &readfs, NULL, NULL, NULL);

        if(ret == -1){
            perror("select error");
            exit(1);
        }
        if(FD_ISSET(fd, &readfs)){
            while((n = read(fd, buf, 128)) > 0){
                printf("%s", buf);
            }
        }

        memset(buf, 0x00, 128);
        usleep(100);
        
    }


}