#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <pthread.h>

#define CHECK_SIZE 30
#define NAME_SIZE 20

void *send_msg(void *arg);
void *recv_msg(void *arg);
void print_bingoboard();
int check_bingoboard(char check);
int calculate_bingo();
void error_handling(char *msg);


char name[NAME_SIZE] = "[DEF]";
char bingoboard[5][5];
pthread_mutex_t mutx;

int main(int argc, char *argv[]){
    int sock;
    struct sockaddr_in serv_adr;
    pthread_t snd_thread, rcv_thread;
    void *thread_return;
    pthread_mutex_init(&mutx, NULL);

    if(argc!=4) {
		printf("usage : %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	}
    sprintf(name, "[%s]", argv[3]);
	sock=socket(PF_INET, SOCK_STREAM, 0);
	if(sock==-1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
    
    //빙고판 생성
    fputs("Input characters to make bingo board...\n", stdout);
    int i = 0, j = 0;
    char tmp[100];
    for(i = 0;i < 5; i++){
        fgets(tmp, sizeof(tmp), stdin);
        char * sPtr = strtok(tmp, " ");
        while(sPtr != NULL){
            bingoboard[i][j++] = sPtr[0];
            sPtr = strtok(NULL, " ");
        }
        if(j != 5){
            printf("다섯 개를 입력해주세요\n");
            i--;
        }
        j = 0;
    }
    /*
            for(j = 0;j <5; j++){
            bingoboard[i][j] = sPtr[0];
            sPtr = strtok(NULL, " ");
        }
*/
    print_bingoboard();
    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);

    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);
    

    close(sock);
    return 0;
}

void *send_msg(void *arg){
    int sock = *((int*)arg);
    int str_len;
    char check[CHECK_SIZE];

    while(1){
        //빙고 체크 입력받고 보내기
        //fputs("Input character to ckeck : ", stdout);  
        fgets(check, CHECK_SIZE, stdin);
        if(strlen(check) > 2){
            printf("다시 한 번 입력하세요\n");
            continue;
        }
        else if(!check_bingoboard(check[0])){ //잘못된 입력
            printf("다시 한 번 입력하세요\n");
            continue;
        }
		else{
            write(sock, check, strlen(check));
        }
    }
    return NULL;

}

void *recv_msg(void *arg){
    int sock = *((int*)arg);
    char check[CHECK_SIZE];
    char msg[CHECK_SIZE];
    int str_len;

    while(1){
        //체크한 빙고 받기
        str_len = read(sock, check, CHECK_SIZE - 1); //\n을 빼기 위함, eof와 구분이 안 가기 때문에
        if(str_len == -1){
            return (void*)-1; //error로 처리하기 위해서
        }
        if(str_len > 2){ //연결 종료
            fputs(check, stdout);
            return NULL;
        }
        //check[str_len] = 0;
        //받은 빙고 체크
        pthread_mutex_lock(&mutx);
        check_bingoboard(check[0]);
        pthread_mutex_unlock(&mutx);
        //빙고판 출력
        print_bingoboard();

        //빙고 상태 전송
        pthread_mutex_lock(&mutx);
        int bingoCount = calculate_bingo();
        pthread_mutex_unlock(&mutx);
        sprintf(msg, "%d", bingoCount);
        write(sock, msg, strlen(msg));
        printf("score : %s\n", msg);
    }
    return NULL;
}

void print_bingoboard(){
    int i ,j;
    printf("---My bingoboard---\n");
    for(i = 0;i < 5;i++){
        for(j = 0;j < 5;j++){
            printf("%c ", bingoboard[i][j]);
        }
        printf("\n");
    }
}
int check_bingoboard(char check){
    int i, j;
    int checked = 0;
    for(i = 0; i < 5; i++){
            for(j = 0;j < 5; j++){
                if(bingoboard[i][j] == check){
                    bingoboard[i][j] = '@';
                    checked = 1;
                }
            }
        }
    return checked;
}
int calculate_bingo(){
    int bingoCount = 0;
    int horizontalCount, verticalCount, diagonalCount;
    int i, j;
    //빙고 계산
    //가로, 세로방향
    for(i = 0; i < 5; i++){
        horizontalCount = 0;
        verticalCount = 0;
        for(j = 0; j < 5; j++){
            if(bingoboard[i][j] == '@')
                horizontalCount++;
            if(bingoboard[j][i] == '@')
                verticalCount++;
        }
        if(horizontalCount == 5)
            bingoCount++;
        if(verticalCount == 5)
            bingoCount++;
    }
    //오른쪽 대각선
    diagonalCount = 0;
    for(i = 0; i < 5; i++){
        if(bingoboard[i][i] == '@')
            diagonalCount++;
    }
    if(diagonalCount == 5)
        bingoCount++;
    //왼쪽 대각선
    diagonalCount = 0;
    for(i = 0; i < 5; i++){
        if(bingoboard[i][4-i] == '@')
            diagonalCount++;
    }
    if(diagonalCount == 5)
        bingoCount++;

    return bingoCount;   
}
void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
