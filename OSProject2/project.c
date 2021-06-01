#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

//there can only be 30 patients and 3 doctors
//initialize all global variables
//these are the shared resources
#define MAX_PATIENTS 30
#define MAX_DOCTORS 3

int waitingLineNumber = 0;
int seatForReciptionist[MAX_PATIENTS];
int serveMeReceptionistNumber = 0;
int seatForNurse[MAX_PATIENTS];
int nurseLineNumber = 0;
int doctorLineNumber = 0;
int seatForDoctor[MAX_PATIENTS];
int patientLineNumber = 0;
int nurseIDForDoc[MAX_PATIENTS];
int nurseArrived = 0;
int patientInSession = 0;
int whosLeaving = 0;

//defining semaphores
sem_t patientsWaiting, patientLeaving, doctor[MAX_DOCTORS], receptionist, nurse, mutex, mutex2, mutex3, nurseWaitForDoctor[MAX_DOCTORS], noPatients;

//create a struct to hold information about the thread
struct threadInfo{
	int threadID;
	int patientID;
};

//starts here
void *waitingRoom();
//then here second
void *receptionOffice();
//nurse transition
void *nurseMovePatient();
//last office before leaving
void *doctorOffice();

//the threads should be every individual patient doctor nurse and receptionist
int main(int argc, char *argv[]) {
	int i = 0; //variable for all for loops
	//define how many doctors and how many patients
	int doctors = atoi(argv[1]);
	int patients = atoi(argv[2]);
	int nurses = doctors;

	//make if statement for if that is over the max
	if(doctors > MAX_DOCTORS){
		printf("3 doctors is the max number of doctors");
		exit(EXIT_FAILURE);
	}
	
	if(patients > MAX_PATIENTS){
		printf("30 patients is the max number of patients");
		exit(EXIT_FAILURE);
	}
	
	//tell user how many are in the simulation
	printf("Run with %d patients, %d nurses, %d doctors \n", patients, nurses, doctors);
	
	//semaphore initialization resptionist,nurse, patients
	sem_init(&patientsWaiting,0,0);
	sem_init(&patientLeaving,0,0);
	sem_init(&receptionist,0,0);
	sem_init(&nurse,0,0);
	sem_init(&noPatients,0,0);
	sem_init(&mutex, 0, 1);
	sem_init(&mutex2, 0, 1);
	sem_init(&mutex3, 0, 1);

	//initialize the struct that holds the information of the thread
	struct threadInfo thread[doctors + patients];

	//creation of threads
	pthread_t docs[doctors], customer[patients], reception, nurse[doctors];


	//reception thread
	pthread_create(&reception, NULL, (void *)receptionOffice,(void*) &i);

	//doctor thread MAX=3; nurse thread = number of doctor threads, struct for each id
	for(i = 0; i<doctors; i++){
		thread[i].threadID = i;
		pthread_create(&docs[i], NULL, (void *)doctorOffice, (void *) &thread[i]);
		pthread_create(&nurse[i], NULL, (void *)nurseMovePatient, (void *) &thread[i]);
		sem_init(&doctor[i], 0, 0);
		sem_init(&nurseWaitForDoctor[i],0,0);
		
	}

	//patient threads MAX = 30
	for(i = 0; i<patients; i++){
		thread[doctors + i].threadID = i;
		pthread_create(&customer[i], NULL, (void *)waitingRoom, (void *) &thread[doctors+i]);
	}

	//wait for the patient threads to come back
	for(i = 0; i < patients; i++){
		pthread_join(customer[i], NULL);
	}

	//end of program
	return 0;
}

//starts here, where patient enters and leaves the doctors office
void *waitingRoom(void *id){
	//local vaiables 
	int patientID, i, index;
	struct threadInfo *thread = (struct threadInfo*) (id);
	index = (*thread).threadID;

	//make a line for the receptionist but others can't interact with it
	sem_wait(&mutex);
	printf("Patient %d enters waiting room, waits for receptionist\n", index);

	//where the patient index will be
	waitingLineNumber = (waitingLineNumber) % MAX_PATIENTS;

	//place the patient id in the array
	seatForReciptionist[waitingLineNumber] = index;
	//increase the patient id counter
	waitingLineNumber ++;
	//open the mutex for other patient threads
	sem_post(&mutex);

	//receptionist is called since there is a patient
	sem_post(&receptionist);

	//nurses has move patients so relaease the doctors
	for(i = 0; i < MAX_PATIENTS; i++){
		sem_post(&noPatients);
	}

	//wait for the receptionist to finish registering the patient
	sem_wait(&patientsWaiting);

	//wait for the doctors to be done
	sem_wait(&patientLeaving);

	//get patientID and the patient is leaving
	sem_wait(&mutex);
	whosLeaving = (whosLeaving) % MAX_PATIENTS;
	patientID = seatForReciptionist[whosLeaving];
	whosLeaving++;
	printf("Patient %d leaves \n", patientID);
	sem_post(&mutex);
}

//then here second, register the patients
void *receptionOffice(void *id){
	//variables for the nurse list
	int patientID, nursePatient;

	//make the receptionist never exit
	while(1){
		//wait for patient to call the receptionist
		sem_wait(&receptionist);

		//since we are using two shared variables we need to make sure the receptionist can access
		sem_wait(&mutex);
		//find the patient that is ready
		serveMeReceptionistNumber = (serveMeReceptionistNumber) % MAX_PATIENTS;

		//get patient id and add to nurse list
		patientID = seatForReciptionist[serveMeReceptionistNumber];
		serveMeReceptionistNumber++;
		sem_post(&mutex);

		printf("Receptionist registers patient %d \n", patientID);

		sem_wait(&mutex2);
		//put patient in line for the nurse
		nurseLineNumber = (nurseLineNumber) % MAX_PATIENTS;
		seatForNurse[nurseLineNumber] = patientID;
		nurseLineNumber++;
		//open for other threads that want the receptionist
		sem_post(&mutex2);
		sem_post(&patientsWaiting);

		//have the nurse come and get the patients
		sem_post(&nurse);
	}
}

/*USE STRUCTS TO MAINTAIN THREADIDS AND NOT OVERWRITE THEM
*/

//nurse will move patients to coresponding doctors office
void *nurseMovePatient(void* id){
	//initialize the variables
	int patientID, index;
	struct threadInfo *thread = (struct threadInfo*) (id);
	index = (*thread).threadID;

	//randomNurse
	while(1){
		//wait for the patient to be ready
		sem_wait(&nurse);
		//access the nurse list
		sem_wait(&mutex2);
		//find the patientID
		doctorLineNumber = (doctorLineNumber) % MAX_PATIENTS;
		patientID = seatForNurse[doctorLineNumber];
		doctorLineNumber++;
		sem_post(&mutex2);

		printf("Nurse %d takes patient %d to doctor's office\n", index, patientID);

		sem_wait(&mutex3);
		//get patientID and nurseID for the doctor
		seatForDoctor[index] = patientID;
		nurseIDForDoc[patientLineNumber] = index;
		patientLineNumber++;

		//release the mutexs and then ask for the doctor
		sem_post(&mutex3);
		
		//call for that certain doctor
		sem_post(&doctor[index]);
		
		//wait for the doctor to be done
		sem_wait(&nurseWaitForDoctor[index]);
	}
}

//last office where the doctor listens the symptoms
void *doctorOffice(void *id){
	int patientID, nurseID, index;
	struct threadInfo *thread = (struct threadInfo*) (id);
	index = (*thread).threadID;

	//same id as Nurse
	sem_wait(&noPatients);
	while(1){
		//look for the NurseID -> DoctorID and then look for patientID
		sem_wait(&doctor[index]);

		//safely get both nurse and patientID
		sem_wait(&mutex3);
		nurseArrived = (nurseArrived) % MAX_PATIENTS;
		nurseID = nurseIDForDoc[nurseArrived];
		patientID = seatForDoctor[nurseID];
		nurseArrived++;
		patientInSession++;
		sem_post(&mutex3);

		//DOCTOR LISTENING TIME
		printf("Doctor %d listens to symptoms from patient %d\n", index, patientID);	

		//release the customer and nurse threads to do their next task
		sem_post(&nurseWaitForDoctor[index]);
		sem_post(&patientLeaving);
	}
}
