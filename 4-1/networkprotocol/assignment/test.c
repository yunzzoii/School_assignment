#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


char bingoboard[5][5];
void print_bingoboard(){
    int i ,j;
    for(i = 0;i < 5;i++){
        for(j = 0;j < 5;j++){
            printf("%c ", bingoboard[i][j]);
        }
        printf("\n");
    }
}
void check_bingoboard(char check){
    int i, j;
    for(i = 0; i < 5; i++){
            for(j = 0;j < 5; j++){
                if(bingoboard[i][j] == check)
                    bingoboard[i][j] = '@';
            }
        }
}
int main(){
    int fd;
    int i, j;
    char tmp[100];
    fputs("Input characters to make bingo board...\n", stdout);

    for(i = 0;i < 5; i++){
        fgets(tmp, sizeof(tmp), stdin);
        char * sPtr = strtok(tmp, " ");
        for(j = 0;j <5; j++){
            bingoboard[i][j] = sPtr[0];
            sPtr = strtok(NULL, " ");
        }
    }
    printf("board\n");
    print_bingoboard();
    fputs("Input character to check\n", stdout);
    char check = fgetc(stdin);
    check_bingoboard(check);
    printf("-----------\n");
    print_bingoboard();

    return 0;

}