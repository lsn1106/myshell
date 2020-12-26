# myshell
리다이랙션과 파이프 기능을 포함한 쉘  프로그램


---

## pipe 

![image](https://user-images.githubusercontent.com/48829883/103146753-0c78b780-4791-11eb-9791-996f01d42c22.png)

‘|’ 로 구분된 블록단위로 수행하고, 매 수행시 자식 프로세스를 하나 만들어서 수행하도록. 그리고 파이프를 둬서 본인 프로세스는 stdin을 파이프에서 입력받고 자식 프로세스(두 번째 이후로 만들어지는 경우 파이프에서 입력)는 stdout을 파이프로 출력하도록 해서 이전 블록이 다음 블록의 입력이 되도록 구현.

![image](https://user-images.githubusercontent.com/48829883/103146829-2a92e780-4792-11eb-9393-decacc428c4b.png)

|’로 나뉜 블록들은 다음 함수에서 처리함. execvp수행 전 리다이랙션을 고려해준다.

![image](https://user-images.githubusercontent.com/48829883/103146840-5a41ef80-4792-11eb-89c4-e8c95ce024fa.png)

---

## redirection

![image](https://user-images.githubusercontent.com/48829883/103146744-e8b57180-4790-11eb-84a7-7c63b36d526a.png)

‘|’로 나뉜 블록들을 하나씩 처리. 이 작은 블록은 ‘>’와 ‘<’으로 나뉜다. ‘>’, ‘<’ 이후엔 파일이름이 온다. ‘>’ 다음 파일은 출력을 할 파일이 되고, ‘<’ 다음 파일은 프로세스가 입력을 받을 파일이 된다. strtok으로 파싱.

![image](https://user-images.githubusercontent.com/48829883/103146855-72b20a00-4792-11eb-82ac-dc9bd8dd0033.png)



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



