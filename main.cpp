#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

void readFile();

void printArrays();

void fcfs();

void sjf();

void printStats();

int process[200];
int burstTime[200];
int arrivalTime[200];

int turnAroundTime[200];
int waitingTime[200];
int completionTime[200];
int serviceTime[200];

//Arrival Time: Time at which the process arrives in the ready queue.
//Completion Time: Time at which process completes its execution.
//Burst Time: Time required by a process for CPU execution.
//Turn Around Time: Time Difference between completion time and arrival time.
//Turn Around Time = Completion Time – Arrival Time
//
//Waiting Time(W.T): Time Difference between turn around time and burst time.
//Waiting Time = Turn Around Time – Burst Time

int main() {
    std::cout << "Hello, World!" << std::endl;
    readFile();
//    fcfs();
    sjf();
    printStats();
    return 0;
}

void fcfs() {
//    int currentTime = 0;
    int currentProcess = 0;

    //find the waiting time for each process
    //process initial process
    serviceTime[currentProcess] = arrivalTime[currentProcess];
    waitingTime[currentProcess] = 0;
    currentProcess++;

    for (; currentProcess < 201; currentProcess++) {
        serviceTime[currentProcess] = serviceTime[currentProcess - 1] + burstTime[currentProcess - 1];
        waitingTime[currentProcess] = serviceTime[currentProcess] - arrivalTime[currentProcess];
        //if the processor has been idle before the next process arrived then there is no wait time
        if (waitingTime[currentProcess] < 0) waitingTime[currentProcess] = 0;
    }
}

void sjf() {
    int currentProcess = 1;
    std::vector<int> waitingList;

//    serviceTime[currentProcess] = arrivalTime[currentProcess];
//    waitingTime[currentProcess] = 0;
//    currentProcess++;

    int currentCompletionTime = 0;
    int currentShortest = 0;
    int currentShortestTime = 0;
    int time = 0;

//    for (int time = 0; time < 3000; time++) {
    while (true) {
        //add the process to the waiting list when it has arrived, where the shortest will be taken
        if (time >= arrivalTime[currentProcess] && currentProcess <= 200) {
            //std::cout << "process: " << currentProcess << " arrived";
            waitingList.push_back(currentProcess);
            currentProcess++;
        }

        //when CPU is idle, grab the process with the shortest burst time amongst all the processes which have arrived
        if (time >= currentCompletionTime) {
            //if there are no more
            if (waitingList.empty() && currentProcess == 201)
                break;
            //now that the previous process is complete, save the statistics for that
            //find the process with the shortest burst time in the waiting list
            for (std::vector<int>::iterator it = waitingList.begin(); it != waitingList.end(); ++it) {
                if (burstTime[*it] < currentShortestTime || currentShortest == 0 || currentShortestTime == 0) {
                    currentShortest = *it;
                    currentShortestTime = burstTime[*it];
                }
            }
            serviceTime[currentShortest] = time;
            waitingTime[currentShortest] = serviceTime[currentShortest] - arrivalTime[currentShortest];
            std::cout << "process: " << currentShortest << "\tarrived: " << arrivalTime[currentShortest] << "\tservice: " << serviceTime[currentShortest] << "\twaited: " << waitingTime[currentShortest] << "\n";

            //if the processor has been idle before the next process arrived then there is no wait time
//            if (waitingTime[currentShortest] < 0) waitingTime[currentShortest] = 0;

            currentCompletionTime = time + burstTime[currentShortest];
            //remove that process from the waiting list
            waitingList.erase(std::remove(waitingList.begin(), waitingList.end(), currentShortest), waitingList.end());
            currentShortestTime = 0;
        }

        //get the current service time by adding the current service time to the burst time
        //on arrival of a new process, add it to the list
        //when the current time is past the service time, meaning that the last process is complete, scan through the list and take the process with the lowest burst time
        std::cout << "Time: " << time << "\tCurrent process: " << currentShortest << "\tArrival Time: " << arrivalTime[currentShortest] << "\tBurst Time: " << burstTime[currentShortest] << "Current Completion Time: " << currentCompletionTime << "\n";
        time++;
    }
}

void readFile() {
    std::string filename = "processes-3";
    std::ifstream f(filename);

    int num = 0; // num must start at 0
    while (!f.fail() && !f.eof()) {
        f >> process[num];
        f >> burstTime[num];
        f >> arrivalTime[num];
        num++;
    }
}

void printArrays() {
    for (int i = 0; i < 100; i++) {
        std::cout << (process[i]);
        std::cout << (burstTime[i]);
        std::cout << (arrivalTime[i]);
    }
}

void printStats() {
    int waitingTimeTotal = 0;
    for (int i = 0; i < 200; i++) {
        std::cout << "process: " << process[i] << "\twait time: " << waitingTime[i] << "\n";
        waitingTimeTotal += waitingTime[i];
    }
    float avgWait = (waitingTimeTotal / 200);
    std::cout << "average waiting time: " << avgWait;
}
