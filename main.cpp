#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <deque>

void readFile();

void printArrays();

void fcfs();

void sjf();

void roundrobin();

void printStats();

std::array<int, 200> process;
std::array<int, 200> burstTime;
std::array<int, 200> arrivalTime;

std::array<int, 200> turnAroundTime;
std::array<int, 200> waitingTime;
std::array<int, 200> completionTime;
std::array<int, 200> serviceTime;

bool debug = true;

//Arrival Time: Time at which the process arrives in the ready queue.
//Completion Time: Time at which process completes its execution.
//Burst Time: Time required by a process for CPU execution.
//Turn Around Time: Time Difference between completion time and arrival time.
//Turn Around Time = Completion Time – Arrival Time

//Waiting Time(W.T): Time Difference between turn around time and burst time.
//Waiting Time = Turn Around Time – Burst Time

int main(int argc, char *argv[]) {
    readFile();
    if (std::string(argv[1]) == "fcfs") fcfs();
    else if (std::string(argv[1]) == "sjf") sjf();
    else if (std::string(argv[1]) == "rr") roundrobin();
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
        completionTime[currentProcess] = serviceTime[currentProcess] + burstTime[currentProcess];
        turnAroundTime[currentProcess] = completionTime[currentProcess] - arrivalTime[currentProcess];
        //if the processor has been idle before the next process arrived then there is no wait time
        if (waitingTime[currentProcess] < 0) waitingTime[currentProcess] = 0;
    }
}

void sjf() {
    std::vector<int> waitingList;

    int currentProcessIndex = 0;
    int currentCompletionTime = 0;
    int currentShortestIndex = 0;
    int currentShortestTime = 0;
    int time = 0;
    bool first = true;
    bool last = false;

    while (true) {
        //add the process to the waiting list when it has arrived, where the shortest will be taken.
        //separated to avoid out of bounds array error
        if (currentProcessIndex <= 199) {
            if (time >= arrivalTime[currentProcessIndex]) {
                if (debug) std::cout << "added process: " << process[currentProcessIndex] << " to waiting list";
                waitingList.push_back(currentProcessIndex);
                currentProcessIndex++;
            }
        }

        //when CPU is idle, grab the process with the shortest burst time amongst all the processes which have arrived
        if (time >= currentCompletionTime) {

            //record the statistics of the process that just finished, if not the first
            if (!first) {
                completionTime[currentShortestIndex] = time;
                turnAroundTime[currentShortestIndex] =
                        completionTime[currentShortestIndex] - arrivalTime[currentShortestIndex];

                if (debug)
                    std::cout << "process finished: " << process[currentShortestIndex]
                              << "\tarrived: " << arrivalTime[currentShortestIndex]
                              << "\tcompletionTime: " << completionTime[currentShortestIndex]
                              << "\tturnAround: " << turnAroundTime[currentShortestIndex] << "\n";
            }
            first = false;

            //break out of the loop when all processes have been added
            if (waitingList.empty() && last == true)
                break;

            //find the process with the shortest burst time in the waiting list
            for (std::vector<int>::iterator processIt = waitingList.begin();
                 processIt != waitingList.end(); ++processIt) {
                if (burstTime[*processIt] < currentShortestTime || currentShortestIndex == 0 ||
                    currentShortestTime == 0) {
                    currentShortestIndex = *processIt;
                    currentShortestTime = burstTime[*processIt];
                }
            }

            //record service waiting and completion time for the newly begun process
            serviceTime[currentShortestIndex] = time;
            waitingTime[currentShortestIndex] = serviceTime[currentShortestIndex] - arrivalTime[currentShortestIndex];

            if (debug)
                std::cout << "process started: " << process[currentShortestIndex]
                          << "\tarrived: " << arrivalTime[currentShortestIndex]
                          << "\tservice: " << serviceTime[currentShortestIndex]
                          << "\twaited: " << waitingTime[currentShortestIndex]
                          << "\tburst: " << burstTime[currentShortestIndex]
                          << "\n";

            //record the time when the newly begun process will be complete and the CPU will be able again
            currentCompletionTime = time + burstTime[currentShortestIndex];

            //remove that process from the waiting list
            waitingList.erase(std::remove(waitingList.begin(), waitingList.end(), currentShortestIndex),
                              waitingList.end());
            currentShortestTime = 0;

            //if the last index has been reached, prepare the exit the loop when the waitingList is empty.
            if (currentProcessIndex == 199) last = true;
        }

        if (debug)
            std::cout << "__Time: " << time << "\tCurrent process: " << process[currentShortestIndex]
                      << "\tArrival Time: " << arrivalTime[currentShortestIndex] << "\tBurst Time: "
                      << burstTime[currentShortestIndex] << "Current Completion Time: " << currentCompletionTime
                      << "\n";
        time++;
    }
}

void roundrobin() {
    //on context switch, reset mini-timer to quantum time (2)
    //count down mini-timer
    //hold a map of process indices to burst time
    //after every turn, remove the value of the burst time of the current process
    //at every switch, keep track on the switch time and total it
    std::deque<int> waitingQueue;
    std::array<int, 200> burstTimeLeft;

    int arrivalIndex = 0;
    int currentProcessIndex = 0;
    int time = 0;
    float switchTime = 0;
    int currentQuantum = 0; //counts down from 2 to 0 then resets
    bool first = true;
    bool last = false;


    while (true) {
        //add the process to the waiting queue.
        //separated to avoid out of bounds array error
        if (arrivalIndex <= 199) {
            if (time >= arrivalTime[arrivalIndex]) {
//                if (debug) std::cout << "added process: " << process[arrivalIndex] << " to waiting list\n";
                waitingQueue.push_back(arrivalIndex);
                burstTimeLeft[arrivalIndex] = burstTime[arrivalIndex];
                arrivalIndex++;
            }
        }

        if (currentQuantum == 0 || burstTimeLeft[currentProcessIndex] == 0) {
            if (burstTimeLeft[currentProcessIndex] > 0) {//only put it back on the queue if it hasnt been fully processed
                if (debug) std::cout << "\tput process: " << process[currentProcessIndex] << " back into the queue\n";
                if (!first) waitingQueue.push_back(currentProcessIndex); //put the index back on the queue, excludes the first as there is no previous
                first = false;
            } else {
                //the process has finished, record the stats and do not put it back in the queue
                completionTime[currentProcessIndex] = time;
                turnAroundTime[currentProcessIndex] = completionTime[currentProcessIndex] - arrivalTime[currentProcessIndex];
                waitingTime[currentProcessIndex] = turnAroundTime[currentProcessIndex] - burstTime[currentProcessIndex];
                if (debug) std::cout << "\tfinished process: " << process[currentProcessIndex] << "\n";
            }
            currentProcessIndex = waitingQueue.front(); //set index to front of queue
            waitingQueue.pop_front(); //pop the front off
            if (debug) std::cout << "\tstarted process: " << process[currentProcessIndex] << " with burst time left: " << burstTimeLeft[currentProcessIndex] << "\n";

            //if its the first time that the process is processing, record the service time
//            if (burstTimeLeft[currentProcessIndex] == burstTime[currentProcessIndex]) {
//
//            }
            switchTime += 0.1;
            currentQuantum = 2; //reset quantum time
        } else {
            if (debug) std::cout << "\n";
        }

        currentQuantum--;
        burstTimeLeft[currentProcessIndex]--;

        time++;
        if (debug) std::cout << time << ": ";
        //if the last process has arrived
        if (last && waitingQueue.empty())
                break;

        if (arrivalIndex == 199) last = true;
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
    int turnAroundTimeTotal = 0;
    for (int i = 0; i < 200; i++) {
        std::cout << "process: " << process[i] << "\t\twait time: " << waitingTime[i]
        << "\t\tturn around time: " << turnAroundTime[i] << "\n";
        waitingTimeTotal += waitingTime[i];
        turnAroundTimeTotal += turnAroundTime[i];
    }
    float avgWait = 1.0 * waitingTimeTotal / 200;
    float avgTurn = 1.0 * turnAroundTimeTotal / 200;
    std::cout << "average waiting time: " << avgWait << "\n";
    std::cout << "average turn around time: " << avgTurn;
}
