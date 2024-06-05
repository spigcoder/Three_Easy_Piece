#include<stdio.h>
#include<unistd.h>
#include<sys/time.h>

void test_syscall(){
	struct timeval tv;
	int times = 100;
	gettimeofday(&tv, NULL);
	suseconds_t prev = tv.tv_usec;
		read(1, NULL, 0);	
	gettimeofday(&tv, NULL);
	suseconds_t next = tv.tv_usec;
	suseconds_t time = next - prev;
	printf("system call cost %ld\n", time/100);	
}

int main(){
	test_syscall();
	return 0;
}
