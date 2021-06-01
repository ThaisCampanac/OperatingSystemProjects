#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//struct for info getting (maybe make a class)
typedef struct processInfo{
    char *processID;
    int startTime;
    int length;
}ProcessInfo;

int numberOfProcesses;

//make a matrix for the results of each algorithm
char fcfsMatrix[26][1000];
char spnMatrix[26][1000];
char hrrnMatrix[26][1000];
//make a queue for each algorithm for the current processes incoming
ProcessInfo fcfsQueue[26];
ProcessInfo spnQueue[26];
ProcessInfo hrrnQueue[26];

//methods for the reading and algorithms and sorting
void readFile(FILE *file, ProcessInfo *process);
void fcfsAlgorithm(ProcessInfo *process);
void spnAlgorithm(ProcessInfo *process);
void sortByShortestProcess(ProcessInfo *process, int start, int numberToOrganize);
void hrrnAlgorithm(ProcessInfo *process);
void sortByResponseRatio(ProcessInfo *process, int start, int numberToOrganize, int timeNow);

//main method
int main(int argc, char *argv[]){
    //create structs for the processes
    ProcessInfo process[26];

    //open file and send to method to read
    //FILE *file = fopen(argv[1], "r");
    FILE *file = fopen("jobs3.txt", "r");
    readFile(file, process);
    fclose(file);

    //do the first come first serve executing thing
    printf("Executing the FCFS algorithm\n");
    fcfsAlgorithm(process);

    //do the spn algorithm
    printf("\nExecuting the SPN algorithm\n");
    spnAlgorithm(process);

    //do the hrrn algorithm
    printf("\nExecuting the HRRN algorithm\n");
    hrrnAlgorithm(process);

    return 0;
}

//read the file and then sort the start time
void readFile(FILE* file, ProcessInfo *process){
    //local variables for the method
    char word[260];
    int position = 0;
    char processName;
    int startTime;
    int length;

    //check if file reading failed
    if(file == NULL){
        printf("FILE READING FAIL");
        return 1;
    }

    //read file and store as a process to execute
    while(!feof(file)){
        fscanf(file, "%s", &word);
        processName = *word;
        fscanf(file, "%s", &word);
        startTime = atoi(word);
        fscanf(file, "%s", &word);
        length = atoi(word);

        //make sure that all processes are id correctly
        if(isalpha(processName)){
            process[position].processID = processName;
            process[position].startTime = startTime;
            process[position].length = length;

            position ++;
        }
    }
}

void fcfsAlgorithm(ProcessInfo *process){
    //all local variables
    int time = 0;
    int positionInProcess = 0;
    int positionInQueue = 0;
    int nextProcess = 0;
    int somethingInQueue = 0;
    int rowNumber = 0;
    int columNumber = 1;
    
    //execute the algorithm
    for(time; time<100; time++){

        //check if a process has came
        while(process[positionInProcess].startTime == time){
            fcfsQueue[positionInQueue] = process[positionInProcess];
            positionInQueue ++;
            positionInProcess++;
            somethingInQueue ++;
        }

        //if there is no process to execute then continue the time in the matrix
        if(somethingInQueue == 0 && process[positionInProcess].startTime != (time)){
            columNumber++;
        }

        //if there is a process in the queue then execute that process
        if(somethingInQueue != 0){
            int length = fcfsQueue[nextProcess].length;
            fcfsMatrix[rowNumber][0] = fcfsQueue[nextProcess].processID;
            somethingInQueue--;

            //"execute" the process
            for(length; length > 0; length --){
                fcfsMatrix[rowNumber][columNumber] = 'X';
                columNumber++;

                //if a process came while the current process is executing then put process in the queue
                while(process[positionInProcess].startTime == time){
                    fcfsQueue[positionInQueue] = process[positionInProcess];
                    positionInQueue ++;
                    positionInProcess++;
                    somethingInQueue ++;
                }
                time++;
            }
            rowNumber++;
            nextProcess++;
            time--;
        }
    }

    //print the output
    numberOfProcesses = rowNumber;
    rowNumber = 0;
    columNumber = 0;
    for(rowNumber; rowNumber < numberOfProcesses; rowNumber++){
        for(columNumber; columNumber < 100; columNumber++){
            if(fcfsMatrix[rowNumber][columNumber] == 0){
                printf(" ");
            }
            else{
                printf("%c",fcfsMatrix[rowNumber][columNumber]);
            }
        }
        printf("\n");
        columNumber = 0;
    }
}

void spnAlgorithm(ProcessInfo *process){
    //variables for local method
    int time = 0;
    int positionInProcess = 0;
    int positionInQueue = 0;
    int nextProcess = 0;
    int somethingInQueue = 0;
    int rowNumber = 0;
    int columNumber = 1;
    
    //execute the algorithm
    for(time; time<1000; time++){

        //check if a process came
        while(process[positionInProcess].startTime == time){
            spnQueue[positionInQueue] = process[positionInProcess];
            somethingInQueue ++;
            positionInQueue ++;
            positionInProcess++;
        }

        //if there is no processes at this time then skip the time to no process
        if(somethingInQueue == 0 && process[positionInProcess].startTime != (time)){
            columNumber++;
        }

        //if there is a process in queue then execute that process
        if(somethingInQueue != 0){
            //sort the queue for the shortest process
            if(somethingInQueue > 1){
                sortByShortestProcess(spnQueue, nextProcess, somethingInQueue);
            }
            int length = spnQueue[nextProcess].length;
            //finding which row in the matrix to assign the values
            int findRow = 0;
            int asciiValue = 65;
            char character = spnQueue[nextProcess].processID;
            for(asciiValue; asciiValue <= 90; asciiValue++){
                char value = (char) asciiValue;
                if(character == value){
                    rowNumber = findRow;
                    break;
                }
                findRow++;
            }

            spnMatrix[rowNumber][0] = spnQueue[nextProcess].processID;
            somethingInQueue--;

            //"execute" that process
            for(length; length > 0; length --){
                spnMatrix[rowNumber][columNumber] = 'X';
                columNumber++;
                
                //check if there is another process coming into the CPU and add to queue
                while(process[positionInProcess].startTime == time){
                    spnQueue[positionInQueue] = process[positionInProcess];
                    somethingInQueue ++;
                    positionInQueue ++;
                    positionInProcess++;
                }
                time++;
            }
            rowNumber++;
            nextProcess++;
            time--;
        }
    }

    //print the output
    rowNumber = 0;
    columNumber = 0;
    for(rowNumber; rowNumber < numberOfProcesses; rowNumber++){
        for(columNumber; columNumber < 100; columNumber++){
            if(spnMatrix[rowNumber][columNumber] == 0){
                printf(" ");
            }
            else{
                printf("%c",spnMatrix[rowNumber][columNumber]);
            }
        }
        printf("\n");
        columNumber = 0;
    }
}

void sortByShortestProcess(ProcessInfo *process, int start, int numberToOrder){
    //I need where to start and how many
    char processName;
    int startTime;
    int length;

    int position = start;
    int endHere = start + numberToOrder;
    for(position; position < endHere - 1; position++){
        //if previous process starts later
        if(process[position].length > process[position + 1].length){
            //temp hold info in the first temp
            processName = process[position].processID;
            startTime = process[position].startTime;
            length = process[position].length;

            //transfer the old info
            process[position].processID = process[position + 1].processID;
            process[position].startTime = process[position + 1].startTime;
            process[position].length = process[position + 1].length;

            //transfer the temp info
            process[position + 1].processID = processName;
            process[position + 1].startTime = startTime;
            process[position + 1].length = length;
            position = start - 1;
        }
    }
}

void hrrnAlgorithm(ProcessInfo *process){
    //local variables
    int time = 0;
    int positionInProcess = 0;
    int positionInQueue = 0;
    int nextProcess = 0;
    int somethingInQueue = 0;
    int rowNumber = 0;
    int columNumber = 1;
    
    //execute the algorithm
    for(time; time<1000; time++){

        //check if there is an incoming process and add to queue
        while(process[positionInProcess].startTime == time){
            hrrnQueue[positionInQueue] = process[positionInProcess];
            positionInQueue ++;
            positionInProcess++;
            somethingInQueue ++;
        }

        //if there are no processes and nothing in queue then skip that time from inputting a value
        if(somethingInQueue == 0 && process[positionInProcess].startTime != (time)){
            columNumber++;
        }

        if(somethingInQueue != 0){
            //sort the queue based on the HRRN response time
            if(somethingInQueue > 1){
                sortByResponseRatio(hrrnQueue, nextProcess, somethingInQueue, time);
            }
            int length = hrrnQueue[nextProcess].length;

            //finding which row in the matrix to assign the values
            int findRow = 0;
            int asciiValue = 65;
            char character = hrrnQueue[nextProcess].processID;
            for(asciiValue; asciiValue <= 90; asciiValue++){
                char value = (char) asciiValue;
                if(character == value){
                    rowNumber = findRow;
                    break;
                }
                findRow++;
            }

            hrrnMatrix[rowNumber][0] = hrrnQueue[nextProcess].processID;
            somethingInQueue--;
            for(length; length > 0; length --){
                hrrnMatrix[rowNumber][columNumber] = 'X';
                columNumber++;
                
                // if another process comes then put into queue
                while(process[positionInProcess].startTime == time){
                    hrrnQueue[positionInQueue] = process[positionInProcess];
                    positionInQueue ++;
                    positionInProcess++;
                    somethingInQueue ++;
                }
                time++;
            }
            rowNumber++;
            nextProcess++;
            time--;
        }
    }

    //print the output
    rowNumber = 0;
    columNumber = 0;
    for(rowNumber; rowNumber < numberOfProcesses; rowNumber++){
        for(columNumber; columNumber < 100; columNumber++){
            if(hrrnMatrix[rowNumber][columNumber] == 0){
                printf(" ");
            }
            else{
                printf("%c",hrrnMatrix[rowNumber][columNumber]);
            }
        }
        printf("\n");
        columNumber = 0;
    }
}

void sortByResponseRatio(ProcessInfo *process, int start, int numberToOrder, int timeNow){
    double firstResponseTime;
    double secondResponseTime;
    int firstWaitTime;
    int secondWaitTime;
    //I need where to start and how many
    char processName;
    int startTime;
    int length;

    int position = start;
    int endHere = start + numberToOrder;
    for(position; position < endHere - 1; position++){
        char first = process[position].processID;
        char second = process[position + 1].processID;
        //calculate the first response time
        firstWaitTime = timeNow - (process[position].startTime);
        firstResponseTime = (firstWaitTime + process[position].length)/process[position].length;
        secondWaitTime = timeNow - (process[position + 1].startTime);
        secondResponseTime = (secondWaitTime + process[position + 1].length)/process[position + 1].length;
        //if previous process starts later
        if(firstResponseTime < secondResponseTime){
            //temp hold info in the first temp
            processName = process[position].processID;
            startTime = process[position].startTime;
            length = process[position].length;

            //transfer the old info
            process[position].processID = process[position + 1].processID;
            process[position].startTime = process[position + 1].startTime;
            process[position].length = process[position + 1].length;

            //transfer the temp info
            process[position + 1].processID = processName;
            process[position + 1].startTime = startTime;
            process[position + 1].length = length;
            position = start - 1;
        }
    }
}
