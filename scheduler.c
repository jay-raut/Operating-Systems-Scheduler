/**
 *  scheduler.c
 *
 *  Full Name: Jay Raut
 *  Course section: B
 *  Description of the program: Simulates different types of CPU scheduling
 *  First Come First Serve, Round Robin, Shortest Remaining Job First
 *
 */

#include "scheduler.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "process.h"

#define SIZE 100

void FCFS(Process *queue, int, int, char *);
void RR(Process *queue, int, int, char *);
void SRJF(Process *queue, int, int, char *);
int checkIfAllProcessFinished(Process *, int);

int main(int argc, char *argv[]) {
    FILE *fp;

    int num_of_processes;

    fp = fopen(argv[1], "r");
    fscanf(fp, "%d", &num_of_processes);
    if (num_of_processes > 0) {
        Process *queue = malloc(num_of_processes * sizeof(Process));

        for (int i = 0; i < num_of_processes; i++) {
            fscanf(fp, "%d %d %d %d", &queue[i].pid, &queue[i].cpu_time, &queue[i].io_time, &queue[i].arrival_time);
            queue[i].state = 0;
            queue[i].burst_time = (int)ceil((float)queue[i].cpu_time * 0.5);  // pre calculate burst time
            queue[i].temp_burst_time = queue[i].burst_time;                   // temp burst time for keeping track of cpu cycles
            queue[i].finishing_time = 0;                                      // finishing time
        }

        for (int i = 0; i < num_of_processes; i++) {
            printf("A: %d B: %d  C: %d D: %d \n", queue[i].pid, queue[i].cpu_time, queue[i].io_time, queue[i].arrival_time);
        }

        int size_of_filename = 0;  // extracting file name
        while (argv[1][size_of_filename] != '\0' && argv[1][size_of_filename] != '.') {  // if we have not reached the end of the file and the current char is not '.'
            size_of_filename++;
        }
        char file_name[size_of_filename + 1];  // + 1 for '\0' character
        for (int i = 0; i < size_of_filename; i++) {
            file_name[i] = argv[1][i];
        }

        file_name[size_of_filename] = '\0';

        if (atoi(argv[2]) == 0) {  // added if statement to choose between scheduler type 0 - 2
            FCFS(queue, sizeof(queue), num_of_processes, file_name);
        } else if (atoi(argv[2]) == 1) {
            RR(queue, sizeof(queue), num_of_processes, file_name);
        } else if (atoi(argv[2]) == 2) {
            SRJF(queue, sizeof(queue), num_of_processes, file_name);
        }

        // end of execution
        free(queue);
    }

    fclose(fp);
    return 0;
}

void FCFS(Process *queue, int sizeof_queue, int num_of_processes, char *inputFileName) {
    char FCFS_file_location[50];
    sprintf(FCFS_file_location, "%s-0.txt", inputFileName);  // creating the output file
    FILE *FCFSPointer = fopen(FCFS_file_location, "w");

    for (int i = 0; i < num_of_processes; i++) {  // sorting the queue based on arrival time
        for (int j = i + 1; j < num_of_processes; j++) {
            if (queue[i].arrival_time > queue[j].arrival_time) {
                Process temp = queue[i];
                queue[i] = queue[j];
                queue[j] = temp;
            }
        }
    }

    for (int i = 0; i < num_of_processes; i++) {  // checking for tiebreaker rule and sorting if two processes
                                                  // arrive at the same time
        for (int j = i + 1; j < num_of_processes; j++) {
            if (queue[i].arrival_time == queue[j].arrival_time) {
                if (queue[i].pid > queue[j].pid) {
                    Process temp = queue[i];
                    queue[i] = queue[j];
                    queue[j] = temp;
                }
            }
        }
    }

    int current_time = 0;           // keep track of the cpu cycles
    int process_queue_counter = 0;  // pointer for checking for arriving processes from the queue variable

    Process *blocked_queue[num_of_processes];  // holds all blocked processes at each cycle
    int blocked_queue_pointer = -1;            // these pointers will be at a occupied position always
    Process *ready_queue[num_of_processes];    // holds all ready processes at each cycle
    int ready_queue_pointer = -1;
    int cpu_counter = 0;  // counts cycles

    Process *running_process;  // current running process
    int running_process_flag = 0;

    for (int i = process_queue_counter; i < num_of_processes; i++) {  // adding all processes which arrived at time 0
        if (queue[i].arrival_time <= 0) {
            ready_queue_pointer++;
            ready_queue[ready_queue_pointer] = &(queue[i]);
            process_queue_counter++;
        }
    }

    while (checkIfAllProcessFinished(queue, num_of_processes) == 0) {
        for (int i = 0; i <= blocked_queue_pointer; i++) {  // decrementing io time in blocked processes
            blocked_queue[i]->io_time--;
            if (blocked_queue[i]->io_time <= 0 && blocked_queue[i]->cpu_time <= 0) {  // remove from the system immediately once finished
                blocked_queue[i]->state = 3;
                blocked_queue[i]->finishing_time = current_time - 1;
                for (int j = i; j <= blocked_queue_pointer; j++) {  // remove process from blocked queue by shifting all elements
                    blocked_queue[j] = blocked_queue[j + 1];
                }
                blocked_queue_pointer--;
                i--;
                continue;
            }
        }

        if ((running_process_flag == 0 && ready_queue_pointer >= 0)) {  // if nothing is running, poll from ready queue
            running_process = ready_queue[0];
            running_process_flag = 1;
            for (int i = 0; i < num_of_processes - 1; i++) {
                ready_queue[i] = ready_queue[i + 1];
            }
            ready_queue_pointer--;
        }
        if (running_process_flag == 1) {  // run the process
            running_process->temp_burst_time--;
            running_process->cpu_time--;
        }

        Process *print_processes[num_of_processes];  // sorting and printing states of each process
        int print_processes_pointer = -1;
        if (running_process_flag == 1) {
            running_process->state = 1;
            print_processes_pointer++;
            print_processes[print_processes_pointer] = running_process;
        }
        for (int i = 0; i <= ready_queue_pointer; i++) {
            ready_queue[i]->state = 0;
            print_processes_pointer++;
            print_processes[print_processes_pointer] = ready_queue[i];
        }
        for (int i = 0; i <= blocked_queue_pointer; i++) {
            blocked_queue[i]->state = 2;
            print_processes_pointer++;
            print_processes[print_processes_pointer] = blocked_queue[i];
        }
        for (int i = 0; i <= print_processes_pointer; i++) {
            for (int j = i; j <= print_processes_pointer; j++) {
                if (print_processes[i]->pid > print_processes[j]->pid) {
                    Process *temp = print_processes[i];
                    print_processes[i] = print_processes[j];
                    print_processes[j] = temp;
                }
            }
        }
        fprintf(FCFSPointer, "%d ", current_time);

        for (int i = 0; i <= print_processes_pointer; i++) {
            if (print_processes[i]->state == 1) {
                fprintf(FCFSPointer, "%d:running ", print_processes[i]->pid);
                cpu_counter++;
            }
            if (print_processes[i]->state == 0) {
                fprintf(FCFSPointer, "%d:ready ", print_processes[i]->pid);
            }
            if (print_processes[i]->state == 2) {
                fprintf(FCFSPointer, "%d:blocked ", print_processes[i]->pid);
            }
        }
        fprintf(FCFSPointer, "\n");
        current_time++;

        Process *temp_ready_queue[num_of_processes];  // temp queue for adding ready processes and arriving processes
        int temp_ready_queue_pointer = -1;
        for (int i = 0; i <= blocked_queue_pointer; i++) {
            if (blocked_queue[i]->io_time <= 0) {  // move process to ready state from blocked state
                temp_ready_queue_pointer++;
                temp_ready_queue[temp_ready_queue_pointer] = blocked_queue[i];
                for (int j = i; j < num_of_processes; j++) {
                    blocked_queue[j] = blocked_queue[j + 1];  // shifting elements in blocked queue
                }
                blocked_queue_pointer--;
                i--;  // this fixes everything
                continue;
            }
        }

        if (running_process_flag == 1) {
            if (running_process->cpu_time <= 0 && running_process->io_time <= 0) {  // if the process is finished then set running_process_flag to 0
                running_process_flag = 0;
                running_process->finishing_time = current_time - 1;
                running_process->state = 3;
            } else if (running_process->temp_burst_time <= 0) {  // if the process burst time is finished
                if (running_process->io_time <= 0) {             // if there is no more io time then make the process ready and switch to the next ready process
                    temp_ready_queue_pointer++;
                    temp_ready_queue[temp_ready_queue_pointer] = running_process;
                    running_process_flag = 0;
                } else {  // move to blocked state
                    running_process->temp_burst_time = running_process->burst_time;
                    blocked_queue_pointer++;
                    blocked_queue[blocked_queue_pointer] = running_process;
                    running_process_flag = 0;
                }
            }
        }

        while (process_queue_counter < num_of_processes && queue[process_queue_counter].arrival_time <= current_time) {  // if the process has arrived then add it to the queue
            temp_ready_queue_pointer++;
            temp_ready_queue[temp_ready_queue_pointer] = &queue[process_queue_counter];
            process_queue_counter++;
        }
        for (int i = 0; i <= temp_ready_queue_pointer; i++) {  // sort the elements based on pid
            for (int j = i; j <= temp_ready_queue_pointer; j++) {
                if (temp_ready_queue[i]->pid > temp_ready_queue[j]->pid) {
                    Process *temp = temp_ready_queue[i];
                    temp_ready_queue[i] = temp_ready_queue[j];
                    temp_ready_queue[j] = temp;
                }
            }
        }

        for (int i = 0; i <= temp_ready_queue_pointer; i++) {  // add temp queue to main queue
            ready_queue_pointer++;
            ready_queue[ready_queue_pointer] = temp_ready_queue[i];
        }
    }

    for (int i = 0; i < num_of_processes; i++) {  // sorting the queue for printing stats after
        for (int j = i; j < num_of_processes; j++) {
            if (queue[i].pid > queue[j].pid) {
                Process temp = queue[i];
                queue[i] = queue[j];
                queue[j] = temp;
            }
        }
    }
    fprintf(FCFSPointer, "\nFinishing time: %d\n", current_time - 1);
    fprintf(FCFSPointer, "CPU utilization: %.2f\n", roundf(((float)cpu_counter / (current_time)) * 100) / 100);
    for (int i = 0; i < num_of_processes; i++) {
        int turnaround_time = queue[i].finishing_time - queue[i].arrival_time + 1;
        fprintf(FCFSPointer, "Turnaround process %d: %d\n", queue[i].pid, turnaround_time);
    }
    fclose(FCFSPointer);
}

void RR(Process *queue, int sizeof_queue, int num_of_processes, char *inputFileName) {
    char FCFS_file_location[50];
    sprintf(FCFS_file_location, "%s-1.txt", inputFileName);
    FILE *FCFSPointer = fopen(FCFS_file_location, "w");

    for (int i = 0; i < num_of_processes; i++) {  // sorting the queue based on arrival time
        for (int j = i + 1; j < num_of_processes; j++) {
            if (queue[i].arrival_time > queue[j].arrival_time) {
                Process temp = queue[i];
                queue[i] = queue[j];
                queue[j] = temp;
            }
        }
    }

    for (int i = 0; i < num_of_processes; i++) {  // checking for tiebreaker rule and sorting if two processes
                                                  // arrive at the same time
        for (int j = i + 1; j < num_of_processes; j++) {
            if (queue[i].arrival_time == queue[j].arrival_time) {
                if (queue[i].pid > queue[j].pid) {
                    Process temp = queue[i];
                    queue[i] = queue[j];
                    queue[j] = temp;
                }
            }
        }
    }

    int current_time = 0;
    int process_queue_counter = 0;

    Process *blocked_queue[num_of_processes];
    int blocked_queue_pointer = -1;  // these pointers will be at a occupied position always
    Process *ready_queue[num_of_processes];
    int ready_queue_pointer = -1;
    int cpu_counter = 0;

    Process *running_process;
    int running_process_flag = 0;

    for (int i = process_queue_counter; i < num_of_processes; i++) {
        if (queue[i].arrival_time <= 0) {
            ready_queue_pointer++;
            ready_queue[ready_queue_pointer] = &(queue[i]);
            process_queue_counter++;
        }
    }
    int quantum = QUANTUM;

    while (checkIfAllProcessFinished(queue, num_of_processes) == 0) {
        for (int i = 0; i <= blocked_queue_pointer; i++) {
            blocked_queue[i]->io_time--;
            if (blocked_queue[i]->io_time <= 0 && blocked_queue[i]->cpu_time <= 0) {  // remove from the system immediately once finished
                blocked_queue[i]->state = 3;
                blocked_queue[i]->finishing_time = current_time - 1;
                for (int j = i; j <= blocked_queue_pointer; j++) {
                    blocked_queue[j] = blocked_queue[j + 1];
                }
                blocked_queue_pointer--;
                i--;  // this fixes everything
                continue;
            }
        }

        if ((running_process_flag == 0 && ready_queue_pointer >= 0)) {  // if nothing is running, poll from ready queue
            running_process = ready_queue[0];
            running_process_flag = 1;
            for (int i = 0; i < num_of_processes - 1; i++) {
                ready_queue[i] = ready_queue[i + 1];
            }
            ready_queue_pointer--;
        }
        if (running_process_flag == 1) {  // run the process
            running_process->temp_burst_time--;
            running_process->cpu_time--;
            quantum--;
        }

        Process *print_processes[num_of_processes];  // sorting and printing states of each process
        int print_processes_pointer = -1;
        if (running_process_flag == 1) {
            running_process->state = 1;
            print_processes_pointer++;
            print_processes[print_processes_pointer] = running_process;
        }
        for (int i = 0; i <= ready_queue_pointer; i++) {
            ready_queue[i]->state = 0;
            print_processes_pointer++;
            print_processes[print_processes_pointer] = ready_queue[i];
        }
        for (int i = 0; i <= blocked_queue_pointer; i++) {
            blocked_queue[i]->state = 2;
            print_processes_pointer++;
            print_processes[print_processes_pointer] = blocked_queue[i];
        }
        for (int i = 0; i <= print_processes_pointer; i++) {
            for (int j = i; j <= print_processes_pointer; j++) {
                if (print_processes[i]->pid > print_processes[j]->pid) {
                    Process *temp = print_processes[i];
                    print_processes[i] = print_processes[j];
                    print_processes[j] = temp;
                }
            }
        }
        fprintf(FCFSPointer, "%d ", current_time);

        for (int i = 0; i <= print_processes_pointer; i++) {
            if (print_processes[i]->state == 1) {
                fprintf(FCFSPointer, "%d:running ", print_processes[i]->pid);
                cpu_counter++;
            }
            if (print_processes[i]->state == 0) {
                fprintf(FCFSPointer, "%d:ready ", print_processes[i]->pid);
            }
            if (print_processes[i]->state == 2) {
                fprintf(FCFSPointer, "%d:blocked ", print_processes[i]->pid);
            }
        }
        fprintf(FCFSPointer, "\n");
        current_time++;

        Process *temp_ready_queue[num_of_processes];  // temp queue for adding ready processes and arriving processes
        int temp_ready_queue_pointer = -1;
        for (int i = 0; i <= blocked_queue_pointer; i++) {
            if (blocked_queue[i]->io_time <= 0) {  // move process to ready state from blocked state
                temp_ready_queue_pointer++;
                temp_ready_queue[temp_ready_queue_pointer] = blocked_queue[i];
                for (int j = i; j < num_of_processes; j++) {
                    blocked_queue[j] = blocked_queue[j + 1];  // shifting elements in blocked queue
                }
                blocked_queue_pointer--;
                i--;  // this fixes everything
                continue;
            }
        }

        if (running_process_flag == 1) {  // running process check
            if (running_process->cpu_time <= 0 && running_process->io_time <= 0) {
                running_process_flag = 0;
                running_process->finishing_time = current_time - 1;
                running_process->state = 3;
                quantum = QUANTUM;
            } else if (running_process->temp_burst_time <= 0) {
                if (running_process->io_time <= 0) {  // if there is no more io time then make the process ready and switch to the next ready process
                    temp_ready_queue_pointer++;
                    temp_ready_queue[temp_ready_queue_pointer] = running_process;
                    running_process_flag = 0;
                    quantum = QUANTUM;
                } else {
                    running_process->temp_burst_time = running_process->burst_time;
                    blocked_queue_pointer++;
                    blocked_queue[blocked_queue_pointer] = running_process;
                    running_process_flag = 0;
                    quantum = QUANTUM;
                }
            } else if (quantum <= 0) {
                temp_ready_queue_pointer++;
                temp_ready_queue[temp_ready_queue_pointer] = running_process;
                running_process_flag = 0;
                quantum = QUANTUM;
            }
        }
        while (process_queue_counter < num_of_processes && queue[process_queue_counter].arrival_time <= current_time) {  // if the process has arrived then add it to the queue
            temp_ready_queue_pointer++;
            temp_ready_queue[temp_ready_queue_pointer] = &queue[process_queue_counter];
            process_queue_counter++;
        }
        for (int i = 0; i <= temp_ready_queue_pointer; i++) {  // sort the elements based on pid
            for (int j = i; j <= temp_ready_queue_pointer; j++) {
                if (temp_ready_queue[i]->pid > temp_ready_queue[j]->pid) {
                    Process *temp = temp_ready_queue[i];
                    temp_ready_queue[i] = temp_ready_queue[j];
                    temp_ready_queue[j] = temp;
                }
            }
        }

        for (int i = 0; i <= temp_ready_queue_pointer; i++) {  // add temp queue to main queue
            ready_queue_pointer++;
            ready_queue[ready_queue_pointer] = temp_ready_queue[i];
        }
    }
    for (int i = 0; i < num_of_processes; i++) {  // sorting the queue for printing stats after
        for (int j = i; j < num_of_processes; j++) {
            if (queue[i].pid > queue[j].pid) {
                Process temp = queue[i];
                queue[i] = queue[j];
                queue[j] = temp;
            }
        }
    }
    fprintf(FCFSPointer, "\nFinishing time: %d\n", current_time - 1);
    fprintf(FCFSPointer, "CPU utilization: %.2f\n", roundf(((float)cpu_counter / (current_time)) * 100) / 100);
    for (int i = 0; i < num_of_processes; i++) {
        int turnaround_time = queue[i].finishing_time - queue[i].arrival_time + 1;
        fprintf(FCFSPointer, "Turnaround process %d: %d\n", queue[i].pid, turnaround_time);
    }
    fclose(FCFSPointer);
}

void SRJF(Process *queue, int sizeof_queue, int num_of_processes, char *inputFileName) {
    char FCFS_file_location[50];
    sprintf(FCFS_file_location, "%s-2.txt", inputFileName);  // creating the output file
    FILE *FCFSPointer = fopen(FCFS_file_location, "w");

    for (int i = 0; i < num_of_processes; i++) {  // sorting the queue based on arrival time
        for (int j = i + 1; j < num_of_processes; j++) {
            if (queue[i].arrival_time > queue[j].arrival_time) {
                Process temp = queue[i];
                queue[i] = queue[j];
                queue[j] = temp;
            }
        }
    }

    for (int i = 0; i < num_of_processes; i++) {  // checking for tiebreaker rule and sorting if two processes
                                                  // arrive at the same time
        for (int j = i + 1; j < num_of_processes; j++) {
            if (queue[i].arrival_time == queue[j].arrival_time) {
                if (queue[i].pid > queue[j].pid) {
                    Process temp = queue[i];
                    queue[i] = queue[j];
                    queue[j] = temp;
                }
            }
        }
    }

    int current_time = 0;           // keep track of the cpu cycles
    int process_queue_counter = 0;  // pointer for checking for arriving processes from the queue variable

    Process *blocked_queue[num_of_processes];  // holds all blocked processes at each cycle
    int blocked_queue_pointer = -1;            // these pointers will be at a occupied position always
    Process *ready_queue[num_of_processes];    // holds all ready processes at each cycle
    int ready_queue_pointer = -1;
    int cpu_counter = 0;  // counts cycles

    Process *running_process;  // current running process
    int running_process_flag = 0;

    for (int i = process_queue_counter; i < num_of_processes; i++) {  // adding all processes which arrived at time 0
        if (queue[i].arrival_time <= 0) {
            ready_queue_pointer++;
            ready_queue[ready_queue_pointer] = &(queue[i]);
            process_queue_counter++;
        }
    }

    while (checkIfAllProcessFinished(queue, num_of_processes) == 0) {
        for (int i = 0; i <= blocked_queue_pointer; i++) {  // decrementing io time in blocked processes
            blocked_queue[i]->io_time--;
            if (blocked_queue[i]->io_time <= 0 && blocked_queue[i]->cpu_time <= 0) {  // remove from the system immediately once finished
                blocked_queue[i]->state = 3;
                blocked_queue[i]->finishing_time = current_time - 1;
                for (int j = i; j <= blocked_queue_pointer; j++) {  // remove process from blocked queue by shifting all elements
                    blocked_queue[j] = blocked_queue[j + 1];
                }
                blocked_queue_pointer--;
                i--;
                continue;
            }
        }

        if ((running_process_flag == 0 && ready_queue_pointer >= 0)) {  // if nothing is running, poll from ready queue
            running_process = ready_queue[0];
            running_process_flag = 1;
            for (int i = 0; i < num_of_processes - 1; i++) {
                ready_queue[i] = ready_queue[i + 1];
            }
            ready_queue_pointer--;
        }
        if (running_process_flag == 1) {  // run the process
            running_process->temp_burst_time--;
            running_process->cpu_time--;
        }

        Process *print_processes[num_of_processes];  // sorting and printing states of each process
        int print_processes_pointer = -1;
        if (running_process_flag == 1) {
            running_process->state = 1;
            print_processes_pointer++;
            print_processes[print_processes_pointer] = running_process;
        }
        for (int i = 0; i <= ready_queue_pointer; i++) {
            ready_queue[i]->state = 0;
            print_processes_pointer++;
            print_processes[print_processes_pointer] = ready_queue[i];
        }
        for (int i = 0; i <= blocked_queue_pointer; i++) {
            blocked_queue[i]->state = 2;
            print_processes_pointer++;
            print_processes[print_processes_pointer] = blocked_queue[i];
        }
        for (int i = 0; i <= print_processes_pointer; i++) {
            for (int j = i; j <= print_processes_pointer; j++) {
                if (print_processes[i]->pid > print_processes[j]->pid) {
                    Process *temp = print_processes[i];
                    print_processes[i] = print_processes[j];
                    print_processes[j] = temp;
                }
            }
        }
        fprintf(FCFSPointer, "%d ", current_time);

        for (int i = 0; i <= print_processes_pointer; i++) {
            if (print_processes[i]->state == 1) {
                fprintf(FCFSPointer, "%d:running ", print_processes[i]->pid);
                cpu_counter++;
            }
            if (print_processes[i]->state == 0) {
                fprintf(FCFSPointer, "%d:ready ", print_processes[i]->pid);
            }
            if (print_processes[i]->state == 2) {
                fprintf(FCFSPointer, "%d:blocked ", print_processes[i]->pid);
            }
        }
        fprintf(FCFSPointer, "\n");
        current_time++;

        Process *temp_ready_queue[num_of_processes];  // temp queue for adding ready processes and arriving processes
        int temp_ready_queue_pointer = -1;
        for (int i = 0; i <= blocked_queue_pointer; i++) {
            if (blocked_queue[i]->io_time <= 0) {  // move process to ready state from blocked state
                temp_ready_queue_pointer++;
                temp_ready_queue[temp_ready_queue_pointer] = blocked_queue[i];
                for (int j = i; j < num_of_processes; j++) {
                    blocked_queue[j] = blocked_queue[j + 1];  // shifting elements in blocked queue
                }
                blocked_queue_pointer--;
                i--;  // this fixes everything
                continue;
            }
        }

        if (running_process_flag == 1) {
            if (running_process->cpu_time <= 0 && running_process->io_time <= 0) {  // if the process is finished then set running_process_flag to 0
                running_process_flag = 0;
                running_process->finishing_time = current_time - 1;
                running_process->state = 3;
            } else if (running_process->temp_burst_time <= 0) {  // if the process burst time is finished
                if (running_process->io_time <= 0) {             // if there is no more io time then make the process ready and switch to the next ready process
                    temp_ready_queue_pointer++;
                    temp_ready_queue[temp_ready_queue_pointer] = running_process;
                    running_process_flag = 0;
                } else {  // move to blocked state
                    running_process->temp_burst_time = running_process->burst_time;
                    blocked_queue_pointer++;
                    blocked_queue[blocked_queue_pointer] = running_process;
                    running_process_flag = 0;
                }
            }
        }

        while (process_queue_counter < num_of_processes && queue[process_queue_counter].arrival_time <= current_time) {  // if the process has arrived then add it to the queue
            temp_ready_queue_pointer++;
            temp_ready_queue[temp_ready_queue_pointer] = &queue[process_queue_counter];
            process_queue_counter++;
        }
        for (int i = 0; i <= temp_ready_queue_pointer; i++) {  // sort the elements based on pid
            for (int j = i; j <= temp_ready_queue_pointer; j++) {
                if (temp_ready_queue[i]->pid > temp_ready_queue[j]->pid) {
                    Process *temp = temp_ready_queue[i];
                    temp_ready_queue[i] = temp_ready_queue[j];
                    temp_ready_queue[j] = temp;
                }
            }
        }

        for (int i = 0; i <= temp_ready_queue_pointer; i++) {  // add temp queue to main queue
            ready_queue_pointer++;
            ready_queue[ready_queue_pointer] = temp_ready_queue[i];
        }

        if (running_process_flag == 1) {  // added lines for srjf, if there is a running process, add to ready queue
            running_process_flag = 0;     // set back to 0 to poll next shortest job
            ready_queue_pointer++;
            ready_queue[ready_queue_pointer] = running_process;
        }

        for (int i = 0; i <= ready_queue_pointer; i++) {  // sort ready queue based on cpu time
            for (int j = i; j <= ready_queue_pointer; j++) {
                if (ready_queue[i]->cpu_time > ready_queue[j]->cpu_time) {
                    Process *temp = ready_queue[i];
                    ready_queue[i] = ready_queue[j];
                    ready_queue[j] = temp;
                }
            }
        }
    }

    for (int i = 0; i < num_of_processes; i++) {  // sorting the queue for printing stats after
        for (int j = i; j < num_of_processes; j++) {
            if (queue[i].pid > queue[j].pid) {
                Process temp = queue[i];
                queue[i] = queue[j];
                queue[j] = temp;
            }
        }
    }
    fprintf(FCFSPointer, "\nFinishing time: %d\n", current_time - 1);
    fprintf(FCFSPointer, "CPU utilization: %.2f\n", roundf(((float)cpu_counter / (current_time)) * 100) / 100);
    for (int i = 0; i < num_of_processes; i++) {
        int turnaround_time = queue[i].finishing_time - queue[i].arrival_time + 1;
        fprintf(FCFSPointer, "Turnaround process %d: %d\n", queue[i].pid, turnaround_time);
    }
    fclose(FCFSPointer);
}

int checkIfAllProcessFinished(Process *queue, int num_of_process) {
    for (int i = 0; i < num_of_process; i++) {
        if (queue[i].state != 3) {
            return 0;
        }
    }
    return 1;
}