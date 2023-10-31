The files included in this project are list.c, scheduler.c, list.h, scheduler.h, process.h and the Makefile used for compile. 

process.h has been modified with extra variables: int state, int burst_time, int temp_burst_time and int finishing_time to keep track
of the process's progress during simulation

list.c and list.h has not been used in this project and primitive arrays have been used instead for simplicity

scheduler.c contains are 3 scheduler algorithms and the logic for creation of the output file.