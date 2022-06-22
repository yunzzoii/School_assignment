#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
void* thread_main(void *arg);

int main(int argc, char *argv[]) 
{
	pthread_t t_id; //thread id저장
	int thread_param=5; //thread에 넘길 parameter
	void * thr_ret; //thread에서 처리한 결과를 받아올 변수
	
	if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param)!=0)
	{ //thread를 만들고 t_id로 thread id를 받고 thread_main 함수를 넘겨준다. thread_main의 파라미터 thread_param을 넘겨준다
		puts("pthread_create() error"); //에러처리
		return -1;
	}; 	

	if(pthread_join(t_id, &thr_ret)!=0)
	{//t_id thread를 join시킨다. thread_main의 결과를 thr_ret으로 받아온다
		puts("pthread_join() error"); //에러처리
		return -1;//에러처리
	};

	printf("Thread return message: %s \n", (char*)thr_ret); //처리한 결과를 받아온다
	free(thr_ret);//공유메모리를 풀어준다
	return 0;
}

void* thread_main(void *arg) 
{
	int i;
	int cnt=*((int*)arg); // 받은 파라미터(arg)를 int로 캐스팅한다
	char * msg=(char *)malloc(sizeof(char)*50); // 동적으로 50바이트를 할당한다. 여기에 message가 들어간다
	strcpy(msg, "Hello, sini~ Good Morning \n");//문자열을 msg에 복사한다. sini에 다른 걸로 바꿀것

	for(i=0; i<cnt; i++)
	{
		sleep(1);  //1초쉰다
		puts("running thread"); // parameter 값 만큼 running thread를 출력한다. 즉 5번 출력된다.
	}
	return (void*)msg; //msg를 리턴한다
}
