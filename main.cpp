

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric> // For std::accumulate
#include <climits>
#include <deque>  // Include deque header
#include <algorithm>  // Include for std::min
using namespace std;


struct Process {
    int arrivalTime;
    vector<int> cpuBursts;
    vector<int> ioBursts;
    int currentCpuBurst = 0;
    int currentIoBurst = 0;
    int completionTime = 0;
    int totalCpuBurstTime=0;
    int turnaroundTime = 0;
    int waitingTime = 0;
    bool inIO = false;
    bool completed = false;
    int ioCompletionTime = 0;
    int remainingCpuBurst = 0; // For SJF and SRTF
    int startTime = 0; // For CFS
};

// Read workload file
vector<Process> readWorkloadFile(const string& filePath) {
    ifstream infile(filePath);
    vector<Process> processes;
    string line;

    while (getline(infile, line)) {
        Process p;
        istringstream iss(line);
        int value;

        iss >> p.arrivalTime;

        bool isCpuBurst = true;
        while (iss >> value && value != -1) {
            if (isCpuBurst) {
                p.cpuBursts.push_back(value);
            } else {
                p.ioBursts.push_back(value);
            }
            isCpuBurst = !isCpuBurst;
        }

        // Initialize remaining CPU burst for SJF and SRTF
        p.remainingCpuBurst = accumulate(p.cpuBursts.begin(), p.cpuBursts.end(), 0);
        
        processes.push_back(p);
    }

    return processes;
}

// FIFO Scheduling
void fifoScheduling(vector<Process>& processes) {
    int currentTime = 0;
    vector<int> readyQueue;
    int numProcesses = processes.size();
    int processesCompleted = 0;

    while (processesCompleted < numProcesses) {
        // Add new arrivals to the ready queue
        for (int i = 0; i < numProcesses; i++) {
            if (processes[i].arrivalTime <= currentTime && !processes[i].inIO && !processes[i].completed) {
                if (find(readyQueue.begin(), readyQueue.end(), i) == readyQueue.end()) {
                    readyQueue.push_back(i);
                }
            }
        }

        // Check if any process has completed its I/O and is ready for the next CPU burst
        for (int i = 0; i < numProcesses; i++) {
            if (processes[i].inIO && processes[i].ioCompletionTime <= currentTime) {
                readyQueue.push_back(i);
                processes[i].inIO = false;
            }
        }

        if (!readyQueue.empty()) {
            int processID = readyQueue.front();
            readyQueue.erase(readyQueue.begin());

            Process& process = processes[processID];
            cout << "Executing Process " << processID + 1 << ", CPU Burst " << process.currentCpuBurst + 1 << endl;
            currentTime += process.cpuBursts[process.currentCpuBurst];
            process.currentCpuBurst++;

            if (process.currentCpuBurst < process.cpuBursts.size()) {
                process.inIO = true;
                process.ioCompletionTime = currentTime + process.ioBursts[process.currentIoBurst++];
            } else {
                process.completionTime = currentTime;
                process.completed = true;
                processesCompleted++;
            }
        } else {
            int nextEventTime = INT_MAX;
            for (int i = 0; i < numProcesses; i++) {
                if (!processes[i].completed) {
                    if (processes[i].arrivalTime > currentTime) {
                        nextEventTime = min(nextEventTime, processes[i].arrivalTime);
                    }
                    if (processes[i].inIO) {
                        nextEventTime = min(nextEventTime, processes[i].ioCompletionTime);
                    }
                }
            }

            if (nextEventTime != INT_MAX) {
                currentTime = nextEventTime;
                cout << "No process ready at time " << currentTime << ". Advancing time." << endl;
            }
        }
    }

    for (auto& process : processes) {
        process.totalCpuBurstTime = accumulate(process.cpuBursts.begin(), process.cpuBursts.end(), 0);
        process.turnaroundTime = process.completionTime - process.arrivalTime;
        process.waitingTime = process.turnaroundTime - process.totalCpuBurstTime;
    }
}

// Shortest Job First (SJF)
void sjfScheduling(vector<Process>& processes) {
    int currentTime = 0;
    vector<int> readyQueue;
    vector<int> completedProcesses;
    int numProcesses = processes.size();
    int processesCompleted = 0;

    while (processesCompleted < numProcesses) {
        // Add new arrivals to the ready queue
        for (int i = 0; i < numProcesses; i++) {
            if (processes[i].arrivalTime <= currentTime && !processes[i].inIO && !processes[i].completed) {
                if (find(readyQueue.begin(), readyQueue.end(), i) == readyQueue.end()) {
                    readyQueue.push_back(i);
                }
            }
        }

        // Check if any process has completed its I/O and is ready for the next CPU burst
        for (int i = 0; i < numProcesses; i++) {
            if (processes[i].inIO && processes[i].ioCompletionTime <= currentTime) {
                readyQueue.push_back(i);
                processes[i].inIO = false;
            }
        }

        if (!readyQueue.empty()) {
            // Find the process with the shortest CPU burst
            int shortestJobID = readyQueue[0];
            for (int id : readyQueue) {
                if (processes[id].cpuBursts[processes[id].currentCpuBurst] < processes[shortestJobID].cpuBursts[processes[shortestJobID].currentCpuBurst]) {
                    shortestJobID = id;
                }
            }

            readyQueue.erase(remove(readyQueue.begin(), readyQueue.end(), shortestJobID), readyQueue.end());

            Process& process = processes[shortestJobID];
            cout << "Executing Process " << shortestJobID + 1 << ", CPU Burst " << process.currentCpuBurst + 1 << endl;
            currentTime += process.cpuBursts[process.currentCpuBurst];
            process.currentCpuBurst++;

            if (process.currentCpuBurst < process.cpuBursts.size()) {
                process.inIO = true;
                process.ioCompletionTime = currentTime + process.ioBursts[process.currentIoBurst++];
            } else {
                process.completionTime = currentTime;
                process.completed = true;
                processesCompleted++;
            }
        } else {
            int nextEventTime = INT_MAX;
            for (int i = 0; i < numProcesses; i++) {
                if (!processes[i].completed) {
                    if (processes[i].arrivalTime > currentTime) {
                        nextEventTime = min(nextEventTime, processes[i].arrivalTime);
                    }
                    if (processes[i].inIO) {
                        nextEventTime = min(nextEventTime, processes[i].ioCompletionTime);
                    }
                }
            }

            if (nextEventTime != INT_MAX) {
                currentTime = nextEventTime;
                cout << "No process ready at time " << currentTime << ". Advancing time." << endl;
            }
        }
    }

    for (auto& process : processes) {
        process.totalCpuBurstTime = accumulate(process.cpuBursts.begin(), process.cpuBursts.end(), 0);
        process.turnaroundTime = process.completionTime - process.arrivalTime;
        process.waitingTime = process.turnaroundTime - process.totalCpuBurstTime;
    }
}

// Shortest Remaining Time First (SRTF)
void srtfScheduling(vector<Process>& processes) {
    int currentTime = 0;
    vector<int> readyQueue;
    int numProcesses = processes.size();
    vector<int> remainingTime(numProcesses, 0);
    int processesCompleted = 0;

    // Initialize remaining times for the current burst
    for (int i = 0; i < numProcesses; i++) {
        if (!processes[i].cpuBursts.empty()) {
            remainingTime[i] = processes[i].cpuBursts[processes[i].currentCpuBurst];
        }
    }

    while (processesCompleted < numProcesses) {
        // Add new arrivals to the ready queue
        for (int i = 0; i < numProcesses; i++) {
            if (processes[i].arrivalTime <= currentTime && !processes[i].inIO && !processes[i].completed) {
                if (find(readyQueue.begin(), readyQueue.end(), i) == readyQueue.end()) {
                    readyQueue.push_back(i);
                }
            }
        }

        // Check if any process has completed its I/O and is ready for the next CPU burst
        for (int i = 0; i < numProcesses; i++) {
            if (processes[i].inIO && processes[i].ioCompletionTime <= currentTime) {
                processes[i].inIO = false;
                if (find(readyQueue.begin(), readyQueue.end(), i) == readyQueue.end()) {
                    readyQueue.push_back(i);
                }
                // Update remaining time for the next CPU burst
                if (processes[i].currentCpuBurst < processes[i].cpuBursts.size()) {
                    remainingTime[i] = processes[i].cpuBursts[processes[i].currentCpuBurst];
                }
            }
        }

        if (!readyQueue.empty()) {
            // Find the process with the shortest remaining current CPU burst time
            int shortestTimeID = readyQueue[0];
            for (int id : readyQueue) {
                if (remainingTime[id] < remainingTime[shortestTimeID]) {
                    shortestTimeID = id;
                }
            }

            Process& process = processes[shortestTimeID];
            int timeSlice = min(remainingTime[shortestTimeID], 1); // Time slice of 1 for simplicity

            cout << "Executing Process " << shortestTimeID + 1 << ", CPU Burst " << process.currentCpuBurst + 1 << " for " << timeSlice << " units\n";
            currentTime += timeSlice;
            remainingTime[shortestTimeID] -= timeSlice;

            if (remainingTime[shortestTimeID] <= 0) {
                process.currentCpuBurst++;
                if (process.currentCpuBurst < process.cpuBursts.size()) {
                    // Process goes to I/O
                    process.inIO = true;
                    process.ioCompletionTime = currentTime + process.ioBursts[process.currentIoBurst++];
                    // Remove from ready queue
                    readyQueue.erase(remove(readyQueue.begin(), readyQueue.end(), shortestTimeID), readyQueue.end());
                } else {
                    // Process is completed
                    process.completionTime = currentTime;
                    process.completed = true;
                    processesCompleted++;
                    readyQueue.erase(remove(readyQueue.begin(), readyQueue.end(), shortestTimeID), readyQueue.end());
                }
            }
        } else {
            // Advance time to the next event
            int nextEventTime = INT_MAX;
            for (int i = 0; i < numProcesses; i++) {
                if (!processes[i].completed) {
                    if (processes[i].arrivalTime > currentTime) {
                        nextEventTime = min(nextEventTime, processes[i].arrivalTime);
                    }
                    if (processes[i].inIO) {
                        nextEventTime = min(nextEventTime, processes[i].ioCompletionTime);
                    }
                }
            }


            if (nextEventTime != INT_MAX) {
                currentTime = nextEventTime;
                cout << "No process ready at time " << currentTime << ". Advancing time." << endl;
            }
        }
    }

    // Compute turnaround and waiting times
    for (auto& process : processes) {
        process.totalCpuBurstTime = accumulate(process.cpuBursts.begin(), process.cpuBursts.end(), 0);
        process.turnaroundTime = process.completionTime - process.arrivalTime;
        process.waitingTime = process.turnaroundTime - process.totalCpuBurstTime;
    }
}

// Completely Fair Scheduler (CFS)
void cfsScheduling(vector<Process>& processes) {
    int currentTime = 0;
    vector<int> readyQueue;
    int numProcesses = processes.size();
    int processesCompleted = 0;
    const int timeSlice = 1; // CFS time slice

    while (processesCompleted < numProcesses) {
        // Add new arrivals to the ready queue
        for (int i = 0; i < numProcesses; i++) {
            if (processes[i].arrivalTime <= currentTime && !processes[i].inIO && !processes[i].completed) {
                if (find(readyQueue.begin(), readyQueue.end(), i) == readyQueue.end()) {
                    readyQueue.push_back(i);
                }
            }
        }

        // Check if any process has completed its I/O and is ready for the next CPU burst
        for (int i = 0; i < numProcesses; i++) {
            if (processes[i].inIO && processes[i].ioCompletionTime <= currentTime) {
                readyQueue.push_back(i);
                processes[i].inIO = false;
            }
        }

        if (!readyQueue.empty()) {
            int processID = readyQueue.front();
            readyQueue.erase(readyQueue.begin());

            Process& process = processes[processID];
            int burstTime = process.cpuBursts[process.currentCpuBurst];
            int slice = min(burstTime, timeSlice);

            cout << "Executing Process " << processID + 1 << ", CPU Burst " << process.currentCpuBurst + 1 << " for " << slice << " units\n";
            currentTime += slice;
            process.cpuBursts[process.currentCpuBurst] -= slice;

            if (process.cpuBursts[process.currentCpuBurst] == 0) {
                process.currentCpuBurst++;
                if (process.currentCpuBurst < process.cpuBursts.size()) {
                    process.inIO = true;
                    process.ioCompletionTime = currentTime + process.ioBursts[process.currentIoBurst++];
                } else {
                    process.completionTime = currentTime;
                    process.completed = true;
                    processesCompleted++;
                }
            } else {
                readyQueue.push_back(processID);
            }
        } else {
            int nextEventTime = INT_MAX;
            for (int i = 0; i < numProcesses; i++) {
                if (!processes[i].completed) {
                    if (processes[i].arrivalTime > currentTime) {
                        nextEventTime = min(nextEventTime, processes[i].arrivalTime);
                    }
                    if (processes[i].inIO) {
                        nextEventTime = min(nextEventTime, processes[i].ioCompletionTime);
                    }
                }
            }

            if (nextEventTime != INT_MAX) {
                currentTime = nextEventTime;
                cout << "No process ready at time " << currentTime << ". Advancing time." << endl;
            }
        }
    }

    for (auto& process : processes) {
        process.totalCpuBurstTime = accumulate(process.cpuBursts.begin(), process.cpuBursts.end(), 0);
        process.turnaroundTime = process.completionTime - process.arrivalTime;
        process.waitingTime = process.turnaroundTime - process.totalCpuBurstTime;
    }
}



#include <deque>
#include <algorithm>

void roundRobinScheduling(vector<Process>& processes, int timeQuantum) {
    int currentTime = 0;
    deque<int> readyQueue;
    int numProcesses = processes.size();
    vector<int> remainingTime(numProcesses, 0);
    int processesCompleted = 0;

    // Initialize remaining times
    for (int i = 0; i < numProcesses; i++) {
        remainingTime[i] = accumulate(processes[i].cpuBursts.begin(), processes[i].cpuBursts.end(), 0);
    }

    // Add initial process arrivals to the ready queue
    for (int i = 0; i < numProcesses; i++) {
        if (processes[i].arrivalTime <= currentTime) {
            readyQueue.push_back(i);
        }
    }

    while (processesCompleted < numProcesses) {
        if (!readyQueue.empty()) {
            int processID = readyQueue.front();
            readyQueue.pop_front();
            Process& process = processes[processID];
            int burstTime = process.cpuBursts[process.currentCpuBurst];
            int timeSlice = min(burstTime, timeQuantum);

            cout << "Executing Process " << processID + 1 << ", CPU Burst " << process.currentCpuBurst + 1 << " for " << timeSlice << " units\n";
            currentTime += timeSlice;
            remainingTime[processID] -= timeSlice;
            burstTime -= timeSlice;

            if (burstTime > 0) {
                process.cpuBursts[process.currentCpuBurst] = burstTime;
                readyQueue.push_back(processID);
            } else {
                process.currentCpuBurst++;
                if (process.currentCpuBurst < process.cpuBursts.size()) {
                    process.inIO = true;
                    process.ioCompletionTime = currentTime + process.ioBursts[process.currentIoBurst++];
                } else {
                    process.completionTime = currentTime;
                    process.completed = true;
                    processesCompleted++;
                }
            }

            // Check for any processes that have arrived in the meantime
            for (int i = 0; i < numProcesses; i++) {
                if (processes[i].arrivalTime <= currentTime && !processes[i].completed && !processes[i].inIO) {
                    if (find(readyQueue.begin(), readyQueue.end(), i) == readyQueue.end()) {
                        readyQueue.push_back(i);
                    }
                }
            }

            // Check for processes completing their I/O and moving back to the ready queue
            for (int i = 0; i < numProcesses; i++) {
                if (processes[i].inIO && processes[i].ioCompletionTime <= currentTime) {
                    processes[i].inIO = false;
                    readyQueue.push_back(i);
                }
            }
        } else {
            // Advance time to the next process arrival or I/O completion
            int nextEventTime = INT_MAX;
            for (int i = 0; i < numProcesses; i++) {
                if (!processes[i].completed) {
                    if (processes[i].arrivalTime > currentTime) {
                        nextEventTime = min(nextEventTime, processes[i].arrivalTime);
                    }
                    if (processes[i].inIO) {
                        nextEventTime = min(nextEventTime, processes[i].ioCompletionTime);
                    }
                }
            }

            if (nextEventTime != INT_MAX) {
                currentTime = nextEventTime;
                cout << "No process ready at time " << currentTime << ". Advancing time." << endl;
            } else {
                break; // All processes are completed, so we exit the loop
            }
        }
    }

    // Calculate turnaround and waiting times
    for (auto& process : processes) {
        int totalCpuBurstTime = accumulate(process.cpuBursts.begin(), process.cpuBursts.end(), 0);
        process.turnaroundTime = process.completionTime - process.arrivalTime;
        process.waitingTime = process.turnaroundTime - totalCpuBurstTime;
    }
}



void calculateAndPrintMetrics(const vector<Process>& processes) {
    double totalTAT = 0, totalWT = 0;
    int numProcesses = processes.size();

    cout << "\nProcess\tArrival Time\tTotalCpuBurst\tCompletion Time\tTAT\tWT\n";
    for (const auto& process : processes) {
        cout << "P" << (&process - &processes[0]) + 1 << "\t"
             << process.arrivalTime << "\t\t"
             << process.totalCpuBurstTime <<"\t\t"
             << process.completionTime << "\t\t"
             << process.turnaroundTime << "\t"
             << process.waitingTime << "\n";
            

        totalTAT += process.turnaroundTime;
        totalWT += process.waitingTime;
    }

    double averageTAT = totalTAT / numProcesses;
    double averageWT = totalWT / numProcesses;

    cout << "\nAverage Turnaround Time (ATAT): " << averageTAT << endl;
    cout << "Average Waiting Time (AWT): " << averageWT << endl;
}
int main(int argc, char* argv[]) {
    if (argc != 3 && argc != 4) {
        cerr << "Usage: " << argv[0] << " <scheduling-algorithm> <path-to-workload-description-file> [<Time Quantum>]" << endl;
        return 1;
    }

    string schedulingAlgorithm = argv[1];
    string filePath = argv[2];

    int tq = 0;
    if (schedulingAlgorithm == "RR") {
        if (argc != 4) {
            cerr << "Usage: " << argv[0] << " <scheduling-algorithm> <path-to-workload-description-file> <Time Quantum>" << endl;
            return 1;
        }
        tq = stoi(argv[3]);  // Convert the third argument to an integer for the time quantum
    } else {
        if (argc != 3) {
            cerr << "Usage: " << argv[0] << " <scheduling-algorithm> <path-to-workload-description-file>" << endl;
            return 1;
        }
    }

    vector<Process> processes = readWorkloadFile(filePath);

    if (schedulingAlgorithm == "FIFO") {
        fifoScheduling(processes);
    } else if (schedulingAlgorithm == "SJF") {
        sjfScheduling(processes);
    } else if (schedulingAlgorithm == "SRTF") {
        srtfScheduling(processes);
    } else if (schedulingAlgorithm == "CFS") {
        cfsScheduling(processes);
    } else if (schedulingAlgorithm == "RR") {
        roundRobinScheduling(processes, tq);
    } else {
        cerr << "Unsupported scheduling algorithm!" << endl;
        return 1;
    }

    calculateAndPrintMetrics(processes);

    return 0;
}
