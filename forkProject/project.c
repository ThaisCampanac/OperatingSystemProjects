#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include<time.h>

int main(int argc, char *argv[]){
	//pipe and fork variables
	int pipe1[2], pipe2[2];
	pid_t pid;
	
	//to set random value of instruction 8
	srand(time(NULL));

	//check if pipe and fork worked or errored
	if(pipe(pipe1)<0){
		fprintf(stderr, "Pipe Failed");
		return 1;
	}
	if(pipe(pipe2)<0){
		fprintf(stderr, "Pipe Failed");
		return 1;
	}
	if((pid = fork()) < 0){
		fprintf(stderr, "Fork Failed");
		return 1;
	}

	//start the parent process
	else if(pid > 0){
		//"CPU" six variables
		int pc = 0;//program counter starting at 0
		int sp = 999; //the stack pointer which will point at the user stack
		int ir; //this is where the instruction will be stored
		int ac; //for temp storage
		int x = 0; //register x
		int y = 0; //register y

		//variables about CPU
		int mode = 0; //0 is user mode and 1 is kernel mode of the CPU
		int timeInterrupt = atoi(argv[1]); //time interrupt set by the user and count down
		int timeInterrupt1 = atoi(argv[1]); //temp value to reload the time interrupt
		
		//pipe from mem to cpu
		close(pipe1[1]);

		//variables to memory or from memory
		char rea = 'r';
		char writ = 'w';
		char qui = 'q';
		int instruction;

		do{
			//time interrupt check
			if (0 >= timeInterrupt && mode == 0) {
				mode = 1;//changing to kernel mode
				timeInterrupt = timeInterrupt1; //the next timeInterrupt
				int newStack = 1999; //new stack for kernel

				//write into stack and save pc
				write(pipe2[1], &writ, sizeof(writ));
				write(pipe2[1], &newStack, sizeof(newStack));
				write(pipe2[1], &pc, sizeof(pc));//return address is after the jump
				
				newStack -= 1;
				
				//saving the sp into the stack
				write(pipe2[1], &writ, sizeof(writ));
				write(pipe2[1], &newStack, sizeof(newStack));
				write(pipe2[1], &sp, sizeof(sp));

				//assign the new pc and sp
				sp = newStack - 1;
				pc = 1000;
				
			}

			//"CPU" will start to read instructions from memory
			if((write(pipe2[1], &rea, sizeof(rea))) < 1){
				printf("CPU write fail");
				return 1;
			}
			if((write(pipe2[1], &pc, sizeof(pc))) < 1){
				printf("CPU write fail");
				return 1;
			}

			int _memread = read(pipe1[0], &instruction, sizeof(instruction));
			if(_memread<0){
				printf("CPU reading mem failed");
				return 1;
			}
			//printf("The instruction is %d\n",instruction);
			
			//execute that instruction

			//loading the next number into ac
			if(instruction == 1){
				pc += 1;
				timeInterrupt -= 1;

				//load value into AC
				if((write(pipe2[1], &rea, sizeof(rea))) < 1){
					printf("Request of read error");
					return 1;
				}
				if((write(pipe2[1], &pc,sizeof(pc))) < 1){
					printf("Request to mem error for instruction 1");
					return 1;
				}
				
				_memread = read(pipe1[0], &ac, sizeof(ac));
				if(_memread<0){
					printf("CPU reading mem failed");
					return 1;
				}

				//printf("AC is now %d \n", ac);
				timeInterrupt -= 1;
				pc += 1;
			}

			//loading value at the address into AC
			if(instruction == 2){
				pc += 1;
				timeInterrupt -= 1;

				//loading address
				write(pipe2[1], &rea, sizeof(rea));
				write(pipe2[1], &pc, sizeof(pc));

				int address;
				read(pipe1[0], &address, sizeof(address));
				
				//checking for memory violations
				if((address > 999 && mode == 0) || (address < 1000 && mode == 1)){
					if(address > 999){
						printf("Memory violation: accessing system address 1000 in user mode");
						instruction = 50;
					}
					else{
						printf("Memory violation: accessing user address 0 while in kernel mode");
						instruction = 50;
					}
				}
				else{
					//loading data into ac
					write(pipe2[1], &rea, sizeof(rea));
					write(pipe2[1], &address, sizeof(address));

					read(pipe1[0], &ac, sizeof(ac));
					pc += 1;
					timeInterrupt -= 1;

					//printf("Ac is now %d \n", ac);
				}

				

			}

			//loading the address for the address of the data
			if(instruction == 3){
				pc += 1;
				timeInterrupt -= 1;

				write(pipe2[1], &rea, sizeof(rea));
				write(pipe2[1], &pc, sizeof(pc));

				int address;
				read(pipe1[0], &address, sizeof(address));
				
				//checking for memory violations
				if((address > 999 && mode == 0) || (address < 1000 && mode == 1)){
					if(address > 999){
						printf("Memory violation: accessing system address 1000 in user mode");
						instruction = 50;
					}
					else{
						printf("Memory violation: accessing user address 0 while in kernel mode");
						instruction = 50;
					}
				}
				else{

					//go to new address for address
					write(pipe2[1], &rea, sizeof(rea));
					write(pipe2[1], &address, sizeof(address));

					int address2;
					read(pipe1[0], &address2, sizeof(address2));

					if ((address2 > 999 && mode == 0) || (address2 < 1000 && mode == 1)) {
						if (address2 > 999) {
							printf("Memory violation: accessing system address 1000 in user mode");
							instruction = 50;
						}
						else {
							printf("Memory violation: accessing user address 0 while in kernel mode");
							instruction = 50;
						}
					}
					else {
						//go to new address for data
						write(pipe2[1], &rea, sizeof(rea));
						write(pipe2[1], &address2, sizeof(address2));

						read(pipe1[0], &ac, sizeof(ac));
					}
				}
			}

			//loading value from pc + x
			if(instruction == 4){
				pc += 1;
				timeInterrupt -= 1;

				if((write(pipe2[1], &rea, sizeof(rea))) < 0){
					printf("first write");
					return 1;
				}
				
				if((write(pipe2[1], &pc, sizeof(pc))) < 0){
					printf("second write fail");
					return 1;
				}

				int address;
				read(pipe1[0], &address, sizeof(address));

				//making the new address for memory to look at
				address  += x;

				//checking for memory violations
				if ((address > 999 && mode == 0) || (address < 1000 && mode == 1)) {
					if (address > 999) {
						printf("Memory violation: accessing system address 1000 in user mode");
						instruction = 50;
					}
					else {
						printf("Memory violation: accessing user address 0 while in kernel mode");
						instruction = 50;
					}
				}
				else {
					write(pipe2[1], &rea, sizeof(rea));
					write(pipe2[1], &address, sizeof(address));

					read(pipe1[0], &ac, sizeof(ac));
					pc += 1;
					timeInterrupt -= 1;

					//printf("AC is now %d \n",ac);
				}
			}

			//loading value from pc + y
			if(instruction == 5){
				pc += 1;
				timeInterrupt -= 1;

				write(pipe2[1], &rea, sizeof(rea));
				write(pipe2[1], &pc, sizeof(pc));

				int address;
				read(pipe1[0], &address, sizeof(address));

				// making new address for memory to look at
				address += y;

				//checking for mem violations
				if ((address > 999 && mode == 0) || (address < 1000 && mode == 1)) {
					if (address > 999) {
						printf("Memory violation: accessing system address 1000 in user mode");
						instruction = 50;
					}
					else {
						printf("Memory violation: accessing user address 0 while in kernel mode");
						instruction = 50;
					}
				}
				else {
					write(pipe2[1], &rea, sizeof(rea));
					write(pipe2[1], &address, sizeof(address));

					read(pipe1[0], &ac, sizeof(ac));
					pc += 1;
					timeInterrupt -= 1;

					//printf("AC is now %d \n", ac);
				}
			}

			//load data from sp + x
			if(instruction == 6){
				pc += 1;
				timeInterrupt -= 1;

				//adding 1 since sp is already ahead
				int address = sp + x + 1;

				//checking for mem violations
				if ((address > 999 && mode == 0) || (address < 1000 && mode == 1)) {
					if (address > 999) {
						printf("Memory violation: accessing system address 1000 in user mode");
						instruction = 50;
					}
					else {
						printf("Memory violation: accessing user address 0 while in kernel mode");
						instruction = 50;
					}
				}
				else {
					write(pipe2[1], &rea, sizeof(rea));
					write(pipe2[1], &address, sizeof(address));

					read(pipe1[0], &ac, sizeof(ac));
				}
			}

			//store ac into the following address
			if(instruction == 7){
				pc += 1;
				timeInterrupt -= 1;

				//finding the address
				int address;
				write(pipe2[1], &rea, sizeof(rea));
				write(pipe2[1], &pc, sizeof(pc));

				read(pipe1[0], &address, sizeof(address));
				pc += 1;
				timeInterrupt -= 1;

				//checking for mem violations
				if ((address > 999 && mode == 0) || (address < 1000 && mode == 1)) {
					if (address > 999) {
						printf("Memory violation: accessing system address 1000 in user mode");
						instruction = 50;
					}
					else {
						printf("Memory violation: accessing user address 0 while in kernel mode");
						instruction = 50;
					}
				}
				else {
					//write the data
					write(pipe2[1], &writ, sizeof(writ));
					write(pipe2[1], &address, sizeof(address));
					write(pipe2[1], &ac, sizeof(ac));
				}
			}

			//get a random number into ac
			if(instruction == 8){
				pc += 1;
				timeInterrupt -= 1;

				//using rand() for random number
				ac = (rand() % 100) + 1;
			}

			//read port number then print as character or integer
			if (instruction == 9) {
				pc += 1;
				timeInterrupt -= 1;

				write(pipe2[1], &rea,sizeof(rea));
				write(pipe2[1], &pc, sizeof(pc));

				int port;
				read(pipe1[0], &port,sizeof(port));
				pc += 1;
				timeInterrupt -= 1;
				//int print
				if(port == 1){
					printf("%d",ac);
				}
				//char print
				else{
					printf("%c", ac);
				}
			}

			//adding x to ac
			if(instruction == 10){
				pc += 1;
				timeInterrupt -= 1;

				ac += x;
			}

			//adding y to ac
			if(instruction == 11){
				pc += 1;
				timeInterrupt -= 1;

				ac += y;
			}

			//subtracting x from ac
			if(instruction == 12){
				pc += 1;
				timeInterrupt -= 1;

				ac -= x;
			}

			//subtracting y from ac
			if(instruction == 13){
				pc += 1;
				timeInterrupt -= 1;

				ac -= y;
			}

			//copy ac to x
			if(instruction == 14){
				pc += 1;
				timeInterrupt -= 1;

				x = ac;
			}

			//copy x to ac
			if(instruction == 15){
				pc += 1;
				timeInterrupt -= 1;

				ac = x;
			}

			//copy ac to y
			if(instruction == 16){
				pc += 1;
				timeInterrupt -= 1;

				y = ac;
			}

			//copy y to ac
			if(instruction == 17){
				pc += 1;
				timeInterrupt -= 1;

				ac = y;
			}

			//copy ac to sp
			if(instruction == 18){
				pc += 1;
				timeInterrupt -= 1;

				sp = ac;
			}

			//copy sp to ac
			if(instruction == 19){
				pc += 1;
				timeInterrupt -= 1;

				ac = sp;
			}

			//jumping to address
			if(instruction == 20){
				pc += 1;
				timeInterrupt -= 1;
				int address;

				write(pipe2[1], &rea, sizeof(rea));
				write(pipe2[1], &pc, sizeof(pc));
				read(pipe1[0], &address,sizeof(address));

				//check for mem violation
				if ((address > 999 && mode == 0) || (address < 1000 && mode == 1)) {
					if (address > 999) {
						printf("Memory violation: accessing system address 1000 in user mode");
						instruction = 50;
					}
					else {
						printf("Memory violation: accessing user address 0 while in kernel mode");
						instruction = 50;
					}
				}
				else {
					pc = address;
				}
				
			}

			//jump to the address if ac is zero
			if(instruction == 21){
				pc += 1;
				timeInterrupt -= 1;

				if( ac == 0){
					int address;
					write(pipe2[1], &rea, sizeof(rea));
					write(pipe2[1], &pc, sizeof(pc));
					read(pipe1[0], &address, sizeof(address));

					//check for mem violation
					if ((address > 999 && mode == 0) || (address < 1000 && mode == 1)) {
						if (address > 999) {
							printf("Memory violation: accessing system address 1000 in user mode");
							instruction = 50;
						}
						else {
							printf("Memory violation: accessing user address 0 while in kernel mode");
							instruction = 50;
						}
					}
					else {
						pc = address;
					}
				}
				else {
					//skip the address load
					pc += 1;
					timeInterrupt -= 1;
				}
			}

			//jump to the address if ac is not zero
			if(instruction == 22){
				pc += 1;
				timeInterrupt -= 1;

				if(ac != 0){
					int address;
					write(pipe2[1], &rea, sizeof(rea));
					write(pipe2[1], &pc, sizeof(pc));
					read(pipe1[0], &address, sizeof(address));

					//check for mem violation
					if ((address > 999 && mode == 0) || (address < 1000 && mode == 1)) {
						if (address > 999) {
							printf("Memory violation: accessing system address 1000 in user mode");
							instruction = 50;
						}
						else {
							printf("Memory violation: accessing user address 0 while in kernel mode");
							instruction = 50;
						}
					}
					else {
						pc = address;
					}
				}
				else{
					//skip the address load
					pc += 1;
					timeInterrupt -= 1;
				}
			}
			
			//push return address onto stack then jump
			if(instruction == 23){
				pc += 1; //completeing instruction 23
				timeInterrupt -= 1;
				pc += 1; //completeing the jump address
				timeInterrupt -= 1;

				//write into stack and save pc
				write(pipe2[1], &writ, sizeof(writ));
				write(pipe2[1], &sp, sizeof(sp));
				write(pipe2[1], &pc, sizeof(pc));//return address is after the jump

				pc -= 1;
				sp -= 1;
				int address;
				write(pipe2[1], &rea, sizeof(rea));
				write(pipe2[1], &pc, sizeof(pc));
				read(pipe1[0], &address, sizeof(address));

				//check for mem violation
				if ((address > 999 && mode == 0) || (address < 1000 && mode == 1)) {
					if (address > 999) {
						printf("Memory violation: accessing system address 1000 in user mode");
						instruction = 50;
					}
					else {
						printf("Memory violation: accessing user address 0 while in kernel mode");
						instruction = 50;
					}
				}
				else {
					pc = address;
				}
			}

			//pop out of stack then jump
			if(instruction == 24){
				pc += 1;
				timeInterrupt -= 1;
				sp += 1;

				int address;
				write(pipe2[1], &rea, sizeof(rea));
				write(pipe2[1], &sp, sizeof(sp));
				read(pipe1[0], &address, sizeof(address));

				//check for mem violation
				if ((address > 999 && mode == 0) || (address < 1000 && mode == 1)) {
					if (address > 999) {
						printf("Memory violation: accessing system address 1000 in user mode");
						instruction = 50;
					}
					else {
						printf("Memory violation: accessing user address 0 while in kernel mode");
						instruction = 50;
					}
				}
				else {
					pc = address;
				}
			}

			//increase x
			if(instruction == 25){
				pc += 1;
				timeInterrupt -= 1;

				x += 1;
			}

			//decrease x
			if(instruction == 26){
				pc += 1;
				timeInterrupt -= 1;

				x -= 1;
			}

			//push into stack
			if(instruction == 27){
				pc += 1;
				timeInterrupt -= 1;

				//write into stack
				write(pipe2[1], &writ, sizeof(writ));
				write(pipe2[1], &sp, sizeof(sp));
				write(pipe2[1], &ac, sizeof(ac));

				sp -= 1;
			}

			//pop out of stack
			if(instruction == 28){
				pc += 1;
				timeInterrupt -= 1;
				sp += 1;

				//read from stack
				write(pipe2[1], &rea, sizeof(rea));
				write(pipe2[1], &sp, sizeof(sp));
				read(pipe1[0], &ac, sizeof(ac));
			}

			//get ready for a system call
			if(instruction == 29){
				mode = 1;//changing to kernel mode
				pc += 1;
				int newStack = 1999;

				//write into stack and save pc
				write(pipe2[1], &writ, sizeof(writ));
				write(pipe2[1], &newStack, sizeof(newStack));
				write(pipe2[1], &pc, sizeof(pc));//return address is after the jump
				
				newStack -= 1;
				
				//saving the sp into the stack
				write(pipe2[1], &writ, sizeof(writ));
				write(pipe2[1], &newStack, sizeof(newStack));
				write(pipe2[1], &sp, sizeof(sp));

				//assign the new pc and sp
				sp = newStack - 1;
				pc = 1500;
			}

			if(instruction == 30){
				mode = 0;//changing to user mode
				int newStack = 1998;//to replace the sp

				//restore pc and sp into the stack of kernel
				write(pipe2[1], &rea, sizeof(rea));
				write(pipe2[1], &newStack, sizeof(newStack));
				read(pipe1[0], &sp, sizeof(sp));

				newStack += 1;

				//bring back the pc
				write(pipe2[1], &rea, sizeof(rea));
				write(pipe2[1], &newStack, sizeof(newStack));
				read(pipe1[0], &pc, sizeof(pc));
			}

			if(instruction == 50){
				write(pipe2[1], &qui, sizeof(qui));
			}

		}while(instruction != 50);

		return 0;
	}

	//start the child process
	else {
		//take in program and put it into "memory"
		int program[2000];

		FILE *file = fopen(argv[2],"r");
		int i = 0;
		char word[50];

		if(file == NULL){
			printf("File Reading Failed");
			return 1;
		}
		else{
			while(!feof(file)){
				fscanf(file, "%s%*[^\n]", word);

				//check if the line has an instruction or not
				if(word[0] == '\r' || word[0] == ' ' || word[0] == '/') {
					continue;
				}
				//save instruction to the list of instructions
				else{
					if(word[0] == '.'){
						memmove(&word[0], &word[1], strlen(word)-0);
						i = atoi(word);
					}
					else{
						program[i] = atoi(word);
						i += 1;
					}
				}
				
			}
		}

		fclose(file);
		//read and write functions through piping happens here (pipe 1 is what it uses
		char operation;
		int  address;
		int cpuRequest;
		while(operation  != 'q'){
			cpuRequest = read(pipe2[0], &operation, sizeof(operation));
			if(cpuRequest < 0){
				printf("Piperead fail");
				return 1;
			}

			//CPU wants to read in memory
			if (operation == 'r'){
				if((read(pipe2[0], &address, sizeof(address))) < 0){
					printf("Piperead fail");
					return 1;
				}

				int instruction = program[address];
				if((write(pipe1[1], &instruction, sizeof(instruction)))< 1){
					printf("Pipewrite fail");
					return 1;
				}
			}

			//CPU wants to write in memory
			if (operation == 'w'){
				if((read(pipe2[0], &address, sizeof(address))) <0){
					printf("Piperead fail");
					return 1;
				}

				int store;
				if((read(pipe2[0], &store, sizeof(store))) <0){
					printf("Pipe read fail");
					return 1;
				}
				program[address] = store;
			}
		}

		exit(0);
	}
}
