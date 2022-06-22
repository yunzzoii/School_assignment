#include <stdio.h>
#include <pthread.h>
void * thread_summation(void * arg); 

int sum=0; // 전역변수로 sum을 정의하고 0으로 초기화한다.

int main(int argc, char *argv[])
{
	pthread_t id_t1, id_t2; // thread id를 저장할 두개의 변수
	int range1[]={1, 5}; // 첫번째 범위는 1~5
	int range2[]={6, 10}; // 두번째 범위는 6~10
	
	pthread_create(&id_t1, NULL, thread_summation, (void *)range1); //첫번째 thread를 만들고 thread_summation함수를 실행한다. parameter는 range1을 넘긴다
	pthread_create(&id_t2, NULL, thread_summation, (void *)range2); //두번째 thread를 만들고 thread_summation함수를 실행한다. parameter는 range2를 넘긴다

	pthread_join(id_t1, NULL);//첫번째 thread를 join시킨다. return은 받지 않는다.
	pthread_join(id_t2, NULL);//두번째 thread를 join시킨다. return은 받지 않는다.
	printf("result: %d \n", sum); //결과를 전역번수로 정의 한 sum을 출력한다.
	return 0;
}

void * thread_summation(void * arg) 
{
	int start=((int*)arg)[0]; //배열로 받은 parameter의 첫번째를 start로 한다
	int end=((int*)arg)[1]; //배열로 받은 parameter의 두번째를 end로 한다

	while(start<=end)
	{
		sum+=start;
		start++;
	}//누적합을 구한다. start가 end에 도달할때까지
	return NULL;
}

//예상되는 결과: 55 하지만 임계영역을 통제시키지 않기 때문에 55보다 적을 수 있음.