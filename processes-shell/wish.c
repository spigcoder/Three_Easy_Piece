#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<fcntl.h>

const int SIZE = 1024;
const char* DELIM = " \t";
int WR = 0;

void Exec(const char* command[]){
    pid_t id = fork();
    if(id == 0){
        //child -> 执行子任务
        execvp(command[0], command);
        exit(0);
    }else{
        waitpid(id, NULL, 0);
    }
}

int SplitCommand(char* buff, char* command[]){
    size_t n = 0;
    //在\n处换上\0
    buff[strlen(buff) - 1] = '\0';
    //解析指令
    int i = 0;
    command[i++] = strtok(buff, DELIM);
    while(command[i++] = strtok(NULL, DELIM));
    command[i] = NULL;
    //在这里进行重定向操作(如果有的话)
    for(int j = 0; command[j]; ++j){
        if(strcmp(command[j], ">") == 0){
            //输出重定向
            command[j + 1][strlen(command[j+1])] = '\0';
            int fd = open(command[j + 1], O_WRONLY|O_TRUNC|O_CREAT, 0666);
            dup2(1, 2);
            dup2(fd, 1); 
            WR = 1;
            command[j] = NULL;
            break;
        }else if(strcmp(command[j], ">>") == 0){
            //追加重定向
            command[j + 1][strlen(command[j+1])] = '\0';
            int fd = open(command[j + 1], O_APPEND|O_WRONLY|O_APPEND, 0666);
            dup2(1, 2);
            dup2(fd, 1); 
            WR = 1;
            command[j] = NULL;
            break;
        }else if(strcmp(command[j], "<") == 0){
            command[j + 1][strlen(command[j+1])] = '\0';
            int fd = open(command[j + 1], O_RDONLY, 0666);
            WR = 2;
            dup2(0, 2);
            dup2(fd, 0); 
            command[j] = NULL;
            break;
        }
    }
    return i - 1;
}

//每次执行内建命令之后都会返回1
bool BuiltCommand(const int com_num, const char* command[]){
    if(strcmp(command[0], "cd") == 0){
        if(com_num > 2) perror("cd command need only two argument");
        else chdir(command[1]);
        return 1;
    }else if(strcmp(command[0], "exit") == 0){
        exit(0);
        return 1;
    }else{
        return 0;
    }
}

//与用户进行交互
void Interaction(int flag){
    //flag == 0表示由用户进行输入
    char buff[SIZE];
    char* command[SIZE];
    if(!flag){
        while(true){
            //如果重定向了文件，将这个文件定向回来,还有一种解决方法就是不在父进程进行重定向操作
            //而是在子进程执行命令时进行重定向操作
            if(WR){
                if(WR == 1) dup2(2, 1);
                else dup2(2, 0);
            }
            printf("wish> ");
            char* tmp = fgets(buff, SIZE, stdin);
            if(!tmp || strcmp(tmp, "\n") == 0) continue; 
            int com_num = SplitCommand(buff, command);
            bool is_built = BuiltCommand(com_num, command);
            if(!is_built)Exec(command);
        }
    }else{
        while(fgets(buff, SIZE, stdin)){
            int com_num = SplitCommand(buff, command);
            bool is_built = BuiltCommand(com_num, command);
            if(!is_built)Exec(command);
        }
    }
}

//对输入文件进行批量处理操作
void BatchMode(const char* file_name){
   int fd = open(file_name, O_RDONLY);
   dup2(fd, 0);     
   Interaction(1);
}

int main(int argc, const char* argv[], const char* env[]){
    if(argc == 1)
        Interaction(0);
    else if(argc == 2)
        BatchMode(argv[1]);
    else{
        perror("argument must be one or null");
        exit(1);
    }
    return 0;
}