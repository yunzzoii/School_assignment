#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#define NUM_THREAD 100

void * thread_inc(void *arg);
void * thread_dec(void *arg);
long long num=0; // long long 형은 64비트 정수

int main(int argc, char *argv[]){
	pthread_t thread_id[NUM_THREAD];
	int i;
	
	printf("sizeof lon long:%d \n", sizeof(long long)); //long long 확인하는 부분 없어도 됨.
	for(i=0; i<NUM_THREAD; i++){
		if(i%2)
			pthread_create(&(thread_id[i]), NULL, thread_inc, NULL); // 교대로 50번씩 실행되는 함수 thread. 짝수 thread에 더하기
		else
			pthread_create(&(thread_id[i]), NULL, thread_dec, NULL); // 교대로 50번씩 실행되는 함수 thread. 홀수 thread에 빼기
	}

	for(i=0; i<NUM_THREAD;i++)
		pthread_join(thread_id[i], NULL);

	printf("result: %lld \n", num);
	return 0;
}

void * thread_inc(void *arg){
	int i;
	for(i=0; i<50000000; i++)
		num+=1;
	return NULL;
}

void * thread_dec(void*arg){
	int i;
	for(i=0; i<50000000; i++)
		num-=1;
	return NULL;
}

// 예상되는 결과: 0 하지만 절대 0이 되지는 않음. 따라서 mutex 또는 semaphore가 필요한 부분

