#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS
void jump(int nth);
int Reg[10];
int calculate(char* op, int opr1, int opr2);
int op_examine(char* op, char* opr1, char* opr2);
void opr1_examine(char* op, char* opr1, char* opr2);
void operatorisM(char* op, char* opr1, char* opr2);
void Condition(char* opr1, char* opr2);
int main()
{
	FILE* fp;
	ssize_t len;
	char *line = 0;
	size_t n;
	int index = 1;
	char *op;
	char *opr1;	
	char *opr2;
	int finish;

	fp = fopen("input.txt", "r");// file open

	while (1)
	{
		len = getline(&line, &n, fp);// read file one instruction in while loop
		if (len == -1) break;//when there are no line, break the loop

		printf("[%d] %s", index, line);// print the instruction line
		index++;
		op = strtok(line, " \t\n");// => divided to op/opr1/opr2
		opr1 = strtok(NULL, " \t\n");
		if (*op == 'J' || *op == 'B') {// when Jump or Branch, there is not opr2, so put "0" value
			opr2 = "0";
			finish = op_examine(op, opr1, opr2);// -1 return, the while loop break-> jump function execute
		}
		else {
			opr2 = strtok(NULL, " \t\n");
			finish = op_examine(op, opr1, opr2);//H를 만나면 종료.
		}
		if (finish == -1) break;
	}
	free(line);
	fclose(fp);
	return 0;
}

int calculate(char* op, int opr1, int opr2) {// 사칙연산을 하는 함수

	switch (*op)
	{
	case '+':
		return opr1 + opr2;
	case '-':
		return opr1 - opr2;
	case '*':
		return opr1 * opr2;
	case '/':
		return opr1 / opr2;

	default:
		break;
	}
}
//check operator
int op_examine(char* op, char* opr1, char* opr2) {//operator를 검사하여 각 다른 종류의 operator마다 다른 함수를 호출


	if (*op == '+' || *op == '-' || *op == '*' || *op == '/') {
		//check operand1
		opr1_examine(op, opr1, opr2);
	}
	else {
		switch (*op) {//H: 종료, M: move, C:compare,J:jump, B:branch
		case 'H':
			return -1;
		case 'M':
			operatorisM(op, opr1, opr2);
			break;
		case 'C':
			Condition(opr1, opr2);// C 1<2  condition일때 연산
			break;
		case 'J':
				jump(atoi(opr1));
				return -1;
		case 'B':
			if(Reg[0]== 1){//if R0 value is 1, do the jump! if not, do the next line.
				jump(atoi(opr1));
				return -1;
			}
			else return 0;//to make not stopp the while loop in main,(when R0 is not 1).
	}
}}
void jump(int nth) {//해당 라인을 인자로 받아서 점프하는 함수.
	FILE* fp = fopen("input.txt", "r");
	int n = nth;
	char line[1024];//1024바이트 까지 메모리 용량지정.
	char* op;
	char* opr1;
	char* opr2;
	int finish;
	for (int i = 0; i < n; i++)//for loop를 이용하여 포인터가 n번째 라인에 갈 수 있도록 만든다.
	{
		fgets(line, 1024, fp);
	}
	while (1) {//while loop in main함수도 같은 방식.
		printf("\n[%d] %s", nth, line);
		nth++;
		op = strtok(line, " \t\n");
		opr1 = strtok(NULL, " \t\n");
		if (*op == 'J' || *op == 'B') {
			opr2 = "0";
			finish = op_examine(op, opr1, opr2);

		}
		else {
			opr2 = strtok(NULL, " \t\n");
			finish = op_examine(op, opr1, opr2);
		}
		if (fgets(line, 1024, fp) == NULL || finish == -1)break;
		//더 이상 instruction 없을때, jump함수호출이 되어 그 이전에 호출되었던 jump함수들이 -1반환되어 끝낸다.
	}
	fclose(fp);
}
void Condition(char* opr1, char* opr2) {// operand1 과 operand2 값을 비교
	if (*opr1 >= '0' && *opr1 <= '9') {// C 3 5--> 3<5 :숫자 숫자 일때
		if (*opr1 >= '0' && *opr2 <= '9') {
			if (atoi(opr1) <= atoi(opr2)) {
				Reg[0] = 1;
			}
			else {
				Reg[0] = 0;
			}
			printf("opcode: C operand1: %s < operand2: %s\n", opr1, opr2);
		}
		else {//ex) C 3 R1
			if (atoi(opr1) <= Reg[atoi(opr2 + 1)])
			{
				Reg[0] = 1;
			}
			else
			{
				Reg[0] = 0;
			}
			printf("opcode: C operand1: %s < operand2: %s (value: %d)\n", opr1, opr2, Reg[atoi(opr2 + 1)]);
		}
	}
	else {
		if (*opr2 >= '0' && *opr2 <= '9') {// ex)C R1 3
			if (atoi(opr2) >= Reg[atoi(opr1 + 1)])
			{
				Reg[0] = 1;
			}
			else
			{
				Reg[0] = 0;
			}
			printf("opcode: C operand1: %s (value: %d) < operand2: %s\n", opr1, Reg[atoi(opr1 + 1)], opr2);

		}
		else { // C ex) R2 R3
			if (Reg[atoi(opr1 + 1)] <= Reg[atoi(opr2 + 1)])
			{
				Reg[0] = 1;
			}
			else
			{
				Reg[0] = 0;
			}
			printf("opcode: C operand1: %s (value: %d) < operand2: %s (value: %d)\n", opr1, Reg[atoi(opr1 + 1)], opr2, Reg[atoi(opr2 + 1)]);
		}
	}
	printf("result: R0 = %d\n\n", Reg[0]);
}
void opr1_examine(char* op, char* opr1, char* opr2) {//사칙연산시 호출되는 함수.operand1을 검사해서 그에 따라 결과 값이 출력.
	//when operand1 & operand2 is both figure.
	if (*opr1 >= '0' && *opr1 <= '9'&& *(opr1 + 1) != 'x') {// ex) + 3 0x5
		if (*(opr2 + 1) == 'x') {
			Reg[0] = calculate(op, atoi(opr1), strtol(opr2, NULL, 16));//
		}
		else {// ex) + 3 4
			Reg[0] = calculate(op, atoi(opr1), atoi(opr2));
			printf("opcode: %s operand1: %s operand2: %s\n", op, opr1, opr2);
		}
	}
	else {
		if (*opr1 == 'R' && *opr2 == 'R') {
			//R1 + R2
			printf("opcode: %s operand1: R%d(value: %d) operand2: R%d(value: %d)\n", op, atoi(opr1 + 1), Reg[atoi(opr1 + 1)],
				atoi(opr2 + 1), Reg[atoi(opr2 + 1)]);
			Reg[0] = calculate(op, Reg[atoi(opr1 + 1)], Reg[atoi(opr2 + 1)]);
		}
		else {
			if (*opr1 == 'R'&& *(opr2 + 1) != 'x') {
				printf("opcode: %s operand1: R%d(value: %d) operand2: %d\n", op, atoi(opr1 + 1), Reg[atoi(opr1 + 1)], atoi(opr2));
				Reg[0] = calculate(op, Reg[atoi(opr1 + 1)], atoi(opr2)); // + R1 1
			}
		}
	}
	if (*(opr1 + 1) == 'x') {//16진수 계산이 필요할 때
		if (*(opr2 + 1) == 'x')
		{// ex) 0x1 0x2
			Reg[0] = calculate(op, strtol(opr1, NULL, 16), strtol(opr2, NULL, 16));
			printf("opcode: %s operand1: %s operand2: %s\n", op, opr1, opr2);
		}
		else {//ex) 0x1 5
			Reg[0] = calculate(op, strtol(opr1, NULL, 16), atoi(opr2));
			printf("opcode: %s operand1: %s operand2: %s\n", op, opr1, opr2);
		}
	}
	printf("result: R0 = %d\n\n", Reg[0]);
}
void operatorisM(char* op, char* opr1, char* opr2) {//Move함수를 만났을 때 각 경우에 따라 if else로 Reg[]값을 할당.

	if (*opr2 == 'R' || *opr2 == 'r') {
		printf("opcode: %s operand1: R%d(value: %d) operand2: R%d(value: %d)\n", op, atoi(opr1 + 1), Reg[atoi(opr1 + 1)],
			atoi(opr2 + 1), Reg[atoi(opr2 + 1)]);
		Reg[atoi(opr1 + 1)] = Reg[atoi(opr2 + 1)];// M R2 R0
	}
	else {
		if (*(opr2 + 1) == 'x') {
			printf("opcode: %s operand1: R%d(value: %d) operand2: %ld\n", op, atoi(opr1 + 1), Reg[atoi(opr1 + 1)], strtol(opr2, NULL, 16));
			Reg[atoi(opr1 + 1)] = strtol(opr2, NULL, 16);
		}
		else {
			printf("opcode: %s operand1: R%d(value: %d) operand2: %d\n", op, atoi(opr1 + 1), Reg[atoi(opr1 + 1)], atoi(opr2));
			Reg[atoi(opr1 + 1)] = atoi(opr2);// M R1 3
		}
	}

	printf("result: R%d = %d\n\n", atoi(opr1 + 1), Reg[atoi(opr1 + 1)]);
}

