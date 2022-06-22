#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <pthread.h>
#include <ctype.h>

#define BUF_SIZE 100
# define MAX_CLNT 256

void *handle_clnt(void *arg);
void send_msg(char *msg, int len);
void error_handling(char *message);
int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[]){ //process id로 다중 접속자 관리할 수 있음
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id;

    if (argc != 2){
        printf("usage : %s <port>\n", argv[0]);
        exit(1);
    }
    pthread_mutex_init(&mutx, NULL);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1){
        error_handling("bind() error");
    }
    if(listen(serv_sock, 5) == -1){
        error_handling("listen() error");
    }

    while(1){
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutx);

        pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
        pthread_detach(t_id);
        printf("connected client IP : %s \n", inet_ntoa(clnt_adr.sin_addr));

    }
    close(serv_sock);
    return 0;
}

void *handle_clnt(void *arg){
    int clnt_sock = *((int*)arg); 
    int str_len = 0, i;
    char msg[BUF_SIZE];
    char* str_win = "You Win";
    char* str_lose = "You Lose";

    while((str_len = read(clnt_sock, msg, sizeof(msg))) != 0){
        if(isdigit(msg[0])){ //빙고상태를 받은 경우
            if((msg[0] - '0') == 3){
                pthread_mutex_lock(&mutx);
                for(i = 0; i < clnt_cnt; i++){
                    if(clnt_sock == clnt_socks[i]){
                        write(clnt_socks[i], str_win, sizeof(str_win));
                    }
                    else{
                        write(clnt_socks[i], str_lose, sizeof(str_lose));
                    }
                }
                pthread_mutex_unlock(&mutx);
                break;
            }
   
            else 
                continue;
        }
        else{ //check인 경우
            send_msg(msg, str_len);
        }    
    }

    //클라이언트 연결 끊는 과정 : 3빙고일때 -> 모든 클라이언트 연결 종료 필요
    pthread_mutex_lock(&mutx);
    for(i = 0; i < clnt_cnt; i++){
        close(clnt_socks[i]);
    }
    clnt_cnt = 0;
    pthread_mutex_unlock(&mutx);

    return NULL;
}

void send_msg(char *msg, int len){
    int i;
    pthread_mutex_lock(&mutx);
    for(i = 0; i < clnt_cnt; i++){
        write(clnt_socks[i], msg, len);
    }
    pthread_mutex_unlock(&mutx);
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}