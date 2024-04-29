//==========================================================================
//  PROGRAM DESCRIPTION:
//  This program attempts to implemetn the First-Come-First-Serve (FCFS)
//  scheduling and Shortest-Job-First (SJF) scheduling. This program takes
//  in two command line arguments (name of file & type of scheduling) and
//  organizes the processes. This program reads from a file containing a 
//  total number of processes, their arrival times, and their burst times.
//  depending on the input the program will either organize them in order
//  of execution depending on the scheduling type selected. This program
//  also calclated Averag Waiting Time and Average Turn-Around Time. 
//  Finally it outputs the order and averages to the console. 
//==========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <limits.h>

//==============================================================
// Structs
//==============================================================
struct Process{ //creates a way to store each processes information in the same array
    int pNum; //assigns a number to each process (ex. P1 P2 ...)
    int arrival;
    int burst;
    int start; //tracks process start time
    int exitTime; //tracks process completion time
    bool executed; //tracks if process was executed
};

//==============================================================
//Initial Method Declaration
//==============================================================
void toLowerCase(char *str); //Turns input into lowercase
int compArrTime(const void *a, const void *b); //compares array for qsort function
void FCFS(struct Process *processes, int numProcess); //FCFS scheduling
void SJF(struct Process *processes, int numProcess); //SJF scheduling
double avgWaitingTime(struct Process *processes, int numProcess); //Average Waiting Time
double avgTAT(struct Process *processes, int numProcess);// Average Turn Around Time

//==============================================================
//  Main
//==============================================================

int main(int argc, char **argv) {
    //=====================
    // Initial Variables
    //=====================
    FILE *file;
    int numProcess; //stores total amount of processes
    char *scheduleType; //stores cmd line scheduling input

    //=====================
    // Reading CMD Args
    //=====================
    if (argc != 3) {
        perror("Usage: schedule <fileName> <scheduling alg FCFS|SJF>");
        return 1;
    }//checks for correct usage 

    scheduleType = argv[2];
    toLowerCase(scheduleType); //makes all input valid regardless of capitalization

    //=======================
    // File Opening/Reading
    //=======================

    file = fopen(argv[1], "r");

    if (file == NULL) {
        perror("Error: File Not Found");
        return 1;
    }//checks for file existance

    fscanf(file, "%d", &numProcess); //loads total number of process from file

    struct Process *processes = (struct Process *)malloc(numProcess * sizeof(struct Process)); //allocates memory to struct array based on number of processes

    for (int i = 0; i <  numProcess; i++){ //fills structs in the array with info from file
        fscanf(file, "%d %d", &processes[i].arrival, &processes[i].burst);

        //start time and exit time are initialized to 0
        processes[i].start = 0; 
        processes[i].exitTime = 0;

        //assigned a name based on reading order (for keeping track & printing)
        processes[i].pNum = i + 1;
    }

    //========================
    // Performing Operations
    //========================
    if (strcmp(scheduleType, "fcfs") == 0){

        //FCFS scheduling & calculations
        FCFS(processes, numProcess);
        printf("Avg Waiting Time: %.2f \nAvg TAT: %.2f\n", avgWaitingTime(processes,numProcess), avgTAT(processes,numProcess));

    } else if (strcmp(scheduleType, "sjf") == 0){
        
        //SJF scheduling & calculations
        SJF(processes,numProcess);
        printf("Avg Waiting Time: %.2f \nAvg TAT: %.2f\n", avgWaitingTime(processes,numProcess), avgTAT(processes,numProcess));

    }
    
    fclose(file);

    return 0;

}//end Main

//==============================================================
//  Methods
//==============================================================

//----------------------------------------------------------------
// Turns CMD SCHEDULE input into lower case to prevent errors
//----------------------------------------------------------------
void toLowerCase(char *str) { 

    for (int i = 0; i < str[i]; i++) { //parses through input and makes it lowecase for consistency
        str[i] = tolower(str[i]);
    }
    
}//end toLowerCase

//------------------------------------------
//Compare Function for qsort (arrival)
//------------------------------------------
int compArrTime(const void *a, const void *b) { //necessary for qsort compares values to sort from least to greatest
    const struct Process *pA = (const struct Process *)a;
    const struct Process *pB = (const struct Process *)b;
    
    if (pA->arrival == pB->arrival) { //compares two values to determine lowest
        return pA->pNum - pB->pNum;
    } else {
        return pA->arrival - pB->arrival;
    }
}//end comArrTime

//--------------------------------
//Performs FSFC scheduling
//--------------------------------
void FCFS(struct Process *processes, int numProcess) {
    int currTime = 0;
    qsort(processes, numProcess, sizeof(struct Process), compArrTime); //sorts processes by arrival time

    for (int i = 0; i < numProcess; i++){

        processes[i].start = currTime; //sets process start time to current time 
        currTime += processes[i].burst; // sets current time to end of process
        processes[i].executed = true; //declares process as executed (not really necessary this was more for sjf but put it here for consistency)
        processes[i].exitTime = currTime; //sets exit time to current time

        printf("P%d", processes[i].pNum); //prints processes as they're finished
        if (i != numProcess - 1) {
            printf(" -> ");
        } else {
            printf("\n");
        }
        
    }//end for

}//end FCFS

//-----------------------------
//Performs SJF scheduling
//-----------------------------
void SJF(struct Process *processes, int numProcess) {

    int currTime = 0;
    int completed = 0;

    qsort(processes, numProcess, sizeof(struct Process), compArrTime);

    for (int i = 0; i < numProcess; i++){

        while (!processes[i].executed) {//loops until they're all executed
            int shortestIndex = -1; //holds current period w/ smallest burst
            int shortestBurst = INT_MAX; //holds current smallest burst

            for (int j = 0; j < numProcess; j++){ //looks for avaiklable process with the smallest burst
                if (processes[j].arrival <= currTime && !processes[j].executed && processes[j].burst < shortestBurst) { //combination here ensures processes that arrived at the same time will be given priority based on burst
                    shortestIndex = j; //store values
                    shortestBurst = processes[j].burst;
                }
            }

            if (shortestIndex != -1) { //starts on shortest available index

                processes[shortestIndex].start = currTime;//same process as fifo
                currTime += processes[shortestIndex].burst;
                processes[shortestIndex].executed = true;
                processes[shortestIndex].exitTime = currTime;
                completed++;

                printf("P%d", processes[shortestIndex].pNum); //prints as they're execute

                if (completed < numProcess){
                    printf(" -> ");
                } else {
                    printf("\n");
                }

            } else {
                currTime++; //progresses time in case of no process arriving at current time or none available until later
            }
        }//end while
    }//end for
    

}//end SJF

//------------------------------------
//Calculates Average Waiting Time
//------------------------------------
double avgWaitingTime(struct Process *processes, int numProcess){

    double totalWaitingtime = 0;

    for (int i = 0; i < numProcess; i++){ //gets all the info from each process struct and performs the operation
        totalWaitingtime += (processes[i].start - processes[i].arrival);
    }

    return totalWaitingtime/numProcess;
}//end avgWaitigTime

//-------------------------------------
//Calculates Average Turn Around Time
//-------------------------------------
double avgTAT(struct Process *processes, int numProcess){
    
    double totalTAT = 0;

    for (int i = 0; i < numProcess; i++){ //gets all the info from each process struct and performs the operation
        totalTAT += (processes[i].exitTime - processes[i].arrival);
    }

    return totalTAT/numProcess;
}// end avgTAT




