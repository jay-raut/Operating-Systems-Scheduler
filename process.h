/**
 *  process.h
 *
 *  Full Name: Jay Raut
 *  Course section: B
 *  Representation of a process in the system.
 *
 */

#ifndef PROCESS_H
#define PROCESS_H

// representation of a a process
typedef struct process {
    int pid;
    int cpu_time;
    int io_time;
    int arrival_time;
    int state;  // 0 for ready, 1 for running, 2 for blocked, 3 for terminated
    int burst_time;
    int temp_burst_time;
    int finishing_time;
} Process;

#endif
