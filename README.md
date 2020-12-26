# myshell
리다이랙션과 파이프 기능을 포함한 쉘  프로그램





## redirection

![image](https://user-images.githubusercontent.com/48829883/103146744-e8b57180-4790-11eb-84a7-7c63b36d526a.png)


## pipe + redirection

![image](https://user-images.githubusercontent.com/48829883/103146753-0c78b780-4791-11eb-9791-996f01d42c22.png)



## 기본적인 명령어 수행
![image](https://user-images.githubusercontent.com/48829883/103146765-36ca7500-4791-11eb-8976-d1f1b5b67f27.png)

execvp로 명령어 인자들을 전달해서 환경변수 위에 있는 기본 명령어를 수행.

```
void execute_cmd(char * cmd){
	char * cmd_vector[MAX_CMD_ARG];

	//make redirection
	redirection_maker(cmd);

	makelist(cmd, " \t", cmd_vector, MAX_CMD_ARG);
	
	execvp(cmd_vector[0], cmd_vector);
}
```



