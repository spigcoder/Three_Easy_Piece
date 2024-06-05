#include<iostream>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string>
using namespace std;

const char* FileName = "file.txt";

void test_fork(){
	int x = 10;
	cout << x << endl;
	pid_t id = fork();
	if(id == 0){
		cout << getpid() << endl;
		x = 100;
		cout << x << endl;
	}else{
		cout << getpid() << endl;
		x = 1100;
		cout << x << endl;
		wait(NULL);
	}		
}

void test_open(){
	//如果先fork再open就会覆盖就是他们都是从最开始开始写入然后打开文件时都会清楚文件内容
	//，但是先open再fork就不会覆盖
	pid_t id = fork();
	int fd = open(FileName, O_CREAT|O_WRONLY|O_TRUNC, 0666);
	if(id == 0){
		const string message = "I'm child\n";
		write(fd, message.c_str(), message.size());
	}else{
		waitpid(id, NULL, 0);
		const string message = "I'm p\n";
		write(fd, message.c_str(), message.size());
	}	
}

void test_child_first(){
	pid_t id = fork();
	if(id == 0){
		cout << "Hello" << endl;	
	}else{
		usleep(100);
		cout << "GoodBye " << endl;	
		wait(NULL);
	}
}

void test_exec(){
	pid_t id = fork();
	if(id < 0){
		perror("open:");	
	}else if(id == 0){
		char* const command[] = {"ls", "-l", "-i", nullptr};	
		execvp(command[0], command);
	}else{
		wait(nullptr);	
	}
}

int main(){
	test_open();
	//test_fork();
	//test_child_first();
	//test_exec();
	return 0;
}
