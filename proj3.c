//Nima Sarrafzadeh
//CSC 139

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

void fcfs(int *pid, int *arrival_time, int *burst_time, int num_lines, int total_time);
void get_fcfs_order(int *pid,int num_lines, int *arrival, int *order);
void srtf(int *pid, int *arrival, int *burst, int num_lines);
void rr(int *pid, int *arrival, int *burst, int num_lines, int quantum);
int get_num_lines(char *file);
void populate_arrays(char *filename, int *pid, int *arrival_time, int *burst_time);

void insert(int queue[], int val, int size);
int delete(int queue[]);
void display(int queue[], int val);
int front =  - 1;
int rear =  - 1;

int main(int argc, char *argv[]){


    char *data;
    char *scheduler;
    char *quantum;
    int num_lines;
    if (argc == 3){
        scheduler = argv[2];
        data = argv[1];

        if (strcmp(scheduler, "SRTF") == 0){
            num_lines = get_num_lines(data);
            int pid[num_lines];
            int arrival_time[num_lines];
            int burst_time[num_lines];

            populate_arrays(data, pid, arrival_time, burst_time);

            printf("\nScheduling algorithm: SRTF\n");
            printf("Total %d tasks read from \"%s\".\n", num_lines,data);
            printf("==============================================\n");
            srtf(pid,arrival_time,burst_time,num_lines);
        }
        else if (strcmp(scheduler, "FCFS") == 0){
            num_lines = get_num_lines(data);
            int pid[num_lines];
            int arrival_time[num_lines];
            int burst_time[num_lines];
            int total_time = 0;
            populate_arrays(data, pid, arrival_time, burst_time);

            printf("\nScheduling algorithm: FCFS\n");
            printf("Total %d tasks read from \"%s\".\n", num_lines,data);
            printf("==============================================\n");
            fcfs(pid, arrival_time, burst_time, num_lines, total_time);
        }
        else{
            printf("Usage: proj2 input_file FCFS|RR|SRTF [quantum]\n");
        }
    }
    else if (argc == 4){

        scheduler = argv[2];
        if (strcmp(scheduler, "RR") == 0){
            int quant;
            data = argv[1];
            scheduler = argv[2];
            quantum = argv[3];
            quant = atoi(quantum);

            num_lines = get_num_lines(data);
            int pid[num_lines];
            int arrival_time[num_lines];
            int burst_time[num_lines];

            populate_arrays(data, pid, arrival_time, burst_time);
            printf("\nScheduling algorithm: RR\n");
            printf("Total %d tasks read from \"%s\".\n", num_lines,data);
            printf("==============================================\n");
            rr(pid, arrival_time, burst_time, num_lines, quant);
        }
        else{
            printf("Usage: proj2 input_file FCFS|RR|SRTF [quantum]\n");
        }
    }
    else{
        printf("Invalid format.\n");
        printf("Usage: proj2 input_file FCFS|RR|SRTF [quantum]\n");
    }
}

void get_fcfs_order(int *pid,int num_lines, int *arrival, int *order){
    int min = INT_MAX;
    int min_index;
    int ctr = 0;

    while (ctr < num_lines){
        int min = INT_MAX;
        for (int i=0; i<num_lines; i++){
            if (arrival[i] < min){
               min = arrival[i];
               min_index = i;
            }
        }
        order[ctr] = min_index;
        arrival[min_index] = INT_MAX;
        ctr++;
    }
}

void fcfs(int *pid, int *arrival_orig, int *burst, int num_lines, int total_time){
    int systime = 0;
    int wasted_time = 0;
    bool proc_running = false;

    float waiting_time = 0;
    float response_time = 0;
    float turnaround_time = 0;

    int min_arrival_time = INT_MAX;
    int curr_pid = pid[0];
    int proc_completed = 0;

    int order[num_lines];

    int arrival[num_lines];
    for (int i=0; i < num_lines; i++){
        arrival[i] = arrival_orig[i];
    }

    int curr_proc = 0;
    int curr_index;

    get_fcfs_order(pid,num_lines, arrival, order);
    while (proc_completed < num_lines){
        curr_index = order[curr_proc];

        if (systime >= arrival_orig[curr_index]){
            waiting_time += (systime - arrival_orig[curr_index]);
            for (int i=0; i<burst[curr_index]; i++){
                printf("<system time %3d > process %3d is running\n", systime, pid[curr_index]);
                systime++;
            }
            printf("<system time %3d > process %3d is finished.......\n", systime, pid[curr_index]);
            turnaround_time += (systime - arrival_orig[curr_index]);
            curr_proc++;
        }
        else{
            printf("no processes executing at time %d\n", systime);
            systime++;
            wasted_time++;
        }
        proc_completed++;
    }

    printf("<system time %3d > All processes finished.............\n\n", systime);
    printf("==============================================\n");
    printf("Average cpu usage        : %.2f%\n",(((float)systime-wasted_time)/systime)*100);
    printf("Average waiting time     : %.2f s\n", waiting_time/num_lines);
    printf("Average response time    : %.2f s\n", waiting_time/num_lines);
    printf("Average turnaround time  : %.2f s\n", turnaround_time/num_lines);
    printf("==============================================\n");
}

void rr(int *pid, int *arrival, int *burst, int num_lines, int quantum){
    int systime = 0;
    int wasted_time = 0;

    float waiting_time = 0;
    float response_time = 0;
    float turnaround_time = 0;

    bool proc_running = false;
    int proc_completed = 0;


    int wait_time[num_lines];
    bool first_run[num_lines];
    int time_first_run[num_lines];
    for (int i = 0; i < num_lines; i++){
        wait_time[i] = 0;
        first_run[i] = true;
    }

    int curr_pid;
    while (proc_completed < num_lines){
        proc_running = false;
        for (int j=0; j<num_lines; j++){

            if (systime >= arrival[j] && burst[j] > 0){
                if (first_run[j]){
                    time_first_run[j] = systime;
                    response_time += systime;
                    first_run[j] = false;
                }
                proc_running = true;

                curr_pid = pid[j];

                for (int q=0; q<quantum; q++){
                    if (burst[j] == 0){
                        printf("<system time %3d > process %3d is finished......\n", systime, curr_pid);
                        systime++;
                    }
                    else{
                        burst[j]--;
                        printf("<system time %3d > process %3d is running.\n", systime, curr_pid);
                        systime++;

                        if (burst[j] == 0){
                            q = quantum;
                            int curr_turnaround = systime - time_first_run[j];
                            printf("<system time %3d > process %3d is finished......\n", systime, curr_pid);
                            turnaround_time += curr_turnaround;
                        }
                    }

                    for (int z=0; z<num_lines; z++){
                        if (z != j){
                           wait_time[j]++;
                        }
                    }
                }
            }

            if (proc_running == false){
                systime++;
                wasted_time++;
            }

            if (burst[j] == 0){
                proc_completed++;
                burst[j] = -1;
            }
        }
    }

    for(int i=0; i<num_lines; i++){
        waiting_time += wait_time[i];
    }

    printf("<system time %3d > All processes finished.............\n\n", systime);


    printf("==============================================\n");
    wasted_time = 0;
    systime = 1;
    printf("Average cpu usage        : %.2f%\n", (float)((systime-wasted_time)/systime)*100);
    printf("Average waiting time     : %.2f s\n", waiting_time/num_lines);
    printf("Average response time    : %.2f s\n", response_time/num_lines);
    printf("Average turnaround time  : %.2f s\n", turnaround_time/num_lines);
    printf("==============================================\n");
}

void srtf(int *pid, int *arrival, int *burst, int num_lines){
    int systime = 0;
    int wasted_time = 0;
    bool proc_running = false;
    int proc_completed = 0;
    int elapsed_time = 0;

    float waiting_time = 0;
    float response_time = 0;
    float turnaround_time = 0;

    int remaining_time[num_lines];
    int wait_time[num_lines];
    bool first_run[num_lines];

    for (int i=0; i<num_lines;i++){
        remaining_time[i] = -1;
        first_run[i] = true;
        wait_time[i] = 0;
    }

    int min_remaining_time = INT_MAX;
    int curr_pid = -1;

    while (proc_completed < num_lines){
        proc_running = false;

        for (int i=0; i<num_lines; i++){
            if (systime >= arrival[i]){
                if (remaining_time[i] != 0){
                    if(pid[i] != curr_pid){
                        if (remaining_time[i] == -1){
                            remaining_time[i] = burst[i];
                        }
                    }
                }
            }
        }

        int curr_index;
        min_remaining_time = INT_MAX;
        for (int i=0; i<num_lines; i++){
            if (remaining_time[i] > 0){
                if (remaining_time[i] < min_remaining_time){
                    min_remaining_time = remaining_time[i];
                    curr_pid = pid[i];
                    curr_index = i;
                    proc_running = true;
                }
            }
        }

        if (first_run[curr_index] == true){
            first_run[curr_index] = false;
            response_time += systime-arrival[curr_index];
        }

        if (proc_running == false){
            wasted_time++;
            printf("<system time %3d > no process is running\n", systime);
        }
        else{
            printf("<system time %3d > process %3d is running\n", systime, curr_pid);
            remaining_time[curr_index]--;
        }

        for (int i=0; i<num_lines; i++){
            if (curr_pid != pid[i] && remaining_time[i] > 0 && systime >= arrival[i]){
                wait_time[i]++;
            }
        }
        systime++;

        if (remaining_time[curr_index] == 0 && proc_running==true){
            printf("<system time %3d > process %3d is finished.......\n", systime, curr_pid);
            turnaround_time += (systime - arrival[curr_index]);
        }

        int completed = 0;
        for (int i =0; i<num_lines; i++){
            if (remaining_time[i] == 0){
                completed++;
            }
        }
        proc_completed = completed;
    }

    printf("<system time %3d > All processes finished.............\n\n", systime);
    for (int i=0; i<num_lines; i++){
        waiting_time += wait_time[i];
    }


    printf("==============================================\n");
    printf("Average cpu usage        : %.2f%\n",(((float)systime-wasted_time)/systime)*100);
    printf("Average waiting time     : %.2f s\n", waiting_time/num_lines);
    printf("Average response time    : %.2f s\n", response_time/num_lines);
    printf("Average turnaround time  : %.2f s\n", turnaround_time/num_lines);
    printf("==============================================\n");
}

void populate_arrays(char *filename, int *pid, int *arrival, int *burst){
    char line[256];
    FILE *file = fopen(filename, "r");
    int i = 0;

    while (fgets(line, sizeof(line), file)){
        sscanf(line,"%d %d %d", &pid[i], &arrival[i], &burst[i]);
        i++;
    }
}

int get_num_lines(char *filename){
    int num_lines = 0;
    FILE *file = fopen(filename, "r");
    char line[256];
    while (fgets(line, sizeof(line), file)){
        num_lines++;
    }
    return num_lines;
}


void insert(int queue[], int item,int size)
{
    if ((front == 0 && rear == size - 1) || (front == rear + 1))
    {
        printf("queue is full");
        return;
    }
    else if (rear ==  -1)
    {
        rear++;
        front++;
    }
    else if (rear == size-1 && front > 0)
    {
        rear = 0;
    }
    else
    {
        rear++;
    }
    queue[rear] = item;
}

void display(int queue[], int size)
{
    int i;
    printf("\n");
    if (front > rear)
    {
        for (i = front; i < size; i++)
        {
            printf("%d ", queue[i]);
        }
        for (i = 0; i <= rear; i++)
            printf("%d ", queue[i]);
    }
    else
    {
        for (i = front; i <= rear; i++)
            printf("%d ", queue[i]);
    }
    printf("\n");
}

int delete(int queue[])
{
    int next;
    if (front ==  -1)
    {

    }
    else if (front == rear)
    {
        next = queue[front];
        front =  -1;
        rear =  -1;
    }
    else
    {
        next = queue[front];
        front++;
    }
    return next;
}
