#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

void outprint(int time_x, int time_y, int pid, int arrival_time, int remaining_time);


void scheduler(Process* proc, LinkedQueue** ProcessQueue, int proc_num, int queue_num, int period) {
    printf("Process number: %d\n", proc_num);
    int* remaining_time = (int*)malloc(sizeof(int) * proc_num); // Remaining time for each process
    int* queue_index = (int*)malloc(sizeof(int) * proc_num); // Store the current queue index of each process
    int* now_running_time = (int*)malloc(sizeof(int) * proc_num); // strore the time that have run in queue to count it not exceed the allotment time
    int next_period = period;
    // Setting 
    for (int i = 0; i < proc_num; i++) {
        printf("%d %d %d\n", proc[i].process_id, proc[i].arrival_time, proc[i].execution_time);
    }
    for (int i = 0; i < proc_num; i++) {
        remaining_time[i] = proc[i].execution_time;
        queue_index[i] = queue_num; //process queue number
        now_running_time[i] = 0;
    }
    printf("\nQueue number: %d\n", queue_num);
    printf("Period: %d\n", period);
    for (int i = 0; i < queue_num; i++) {
        printf("%d %d %d\n", i, ProcessQueue[i]->time_slice, ProcessQueue[i]->allotment_time);
    }

    int current_time = 0; // Current time in the scheduling process
    int all_finished = 0;
    int temp;
    while (1) {

        if (current_time % 500 == 0)
            printf("current_time:%d\n", current_time);
        // Finish and process arrived
        for (int i = 0; i < proc_num; i++) {
            // If all processes have finished, exit the scheduling loop
            if (remaining_time[i] == 0) {
                temp++;
            }
            // proc arrived 
            if (proc[i].arrival_time <= current_time && queue_index[i] == queue_num) {
                printf("a process arrived\n");
                printf("current_time:%d progcess: %d\n", current_time, proc[i].process_id);
                int now_queue_index = queue_num - 1; // Index of the last queue in ProcessQueue
                ProcessQueue[now_queue_index] = EnQueue(ProcessQueue[now_queue_index], proc[i]);
                printf("after process %d arrived in Q%d\n", proc[i].process_id,now_queue_index);
                QueuePrint(ProcessQueue[now_queue_index]);
                printf("\n");
                queue_index[i] = now_queue_index;
            }
        }
        // Break when all finish 
        if (temp == proc_num)
        {
            all_finished = 1;
        }
        else
            temp = 0;
        if (all_finished){
            printf("All the processs have been finish \n");
            break;
            }
        // Period coming
        if (current_time == next_period)
        {
            printf("Period coming\n");
            // TODO: Move all unfinished processes to ProcessQueue[queue_num-1] and sort them by process_id

            int num_unfinishedProcesses = 0;
            for(int j = 0; j<queue_num; j++){
                while (!IsEmptyQueue(ProcessQueue[j])) 
                    DeQueue(ProcessQueue[j]);
                QueuePrint(ProcessQueue[j]);
            }
            for(int j =0; j<proc_num; j++)
            {
                if (remaining_time[j]>0)
                {
                    EnQueue(ProcessQueue[queue_num-1],proc[j]);
                }
            }
            num_unfinishedProcesses =Length(ProcessQueue[queue_num-1]);
            printf("Sorting\n");
            QueuePrint(ProcessQueue[queue_num-1]);
            // Convert the queue to an array for sorting
            Process* unfinishedProcesses = (Process*)malloc(sizeof(Process) * num_unfinishedProcesses);
            int index = 0;
            while (!IsEmptyQueue(ProcessQueue[queue_num - 1])) {
                Process front_process = DeQueue(ProcessQueue[queue_num - 1]);
                unfinishedProcesses[index++] = front_process;
            }

            // Sort the unfinished processes by process_id using a simple bubble sort algorithm
            for (int i = 0; i < num_unfinishedProcesses - 1; i++) {
                for (int j = 0; j < num_unfinishedProcesses - i - 1; j++) {
                    if (unfinishedProcesses[j].process_id > unfinishedProcesses[j + 1].process_id) {
                        Process temp = unfinishedProcesses[j];
                        unfinishedProcesses[j] = unfinishedProcesses[j + 1];
                        unfinishedProcesses[j + 1] = temp;
                    }
                }
            }

            // Enqueue the sorted processes back into ProcessQueue[queue_num-1]
            for (int i = 0; i < num_unfinishedProcesses; i++) {
                EnQueue(ProcessQueue[queue_num - 1], unfinishedProcesses[i]);
            }
            for (int i = 0; i < proc_num; i++) {
                now_running_time[i] = 0;
            }

            free(unfinishedProcesses);
            next_period = next_period + period;
            printf("Sorted unfinished processes:\n");
            QueuePrint(ProcessQueue[queue_num - 1]);
            printf("\n");
        }
        // queue loop
        for (int i = queue_num - 1; i >= 0; i--) {
            if (!IsEmptyQueue(ProcessQueue[i])) {
                
                Process front_process = FrontQueue(ProcessQueue[i]);
                int time_slice = ProcessQueue[i]->time_slice;
                int allotment_time = ProcessQueue[i]->allotment_time;
                int process_order;
                for (int i = 0; i < proc_num; i++) {
                    // printf("front_process.process_id is %d\n",front_process.process_id); 
                    if (proc[i].process_id == front_process.process_id){
                        process_order = i;
                        break;}
                }
                printf("remaining_time[%d]:%d\n", process_order,remaining_time[process_order]);
                if (remaining_time[process_order] > 0) {
                    printf("A process have been doing\ncurrent_time:%d\n", current_time);
                    int execution_time;
                    if (time_slice <= remaining_time[process_order])
                        execution_time = time_slice;
                    else
                        execution_time = remaining_time[process_order];
                    if (execution_time + current_time >= next_period){
                        execution_time = next_period - current_time;
                    }
                    printf("execution time: %d \n",execution_time);
                    remaining_time[process_order] -= execution_time;
                    printf("remaining_time[process_order]:%d\n", remaining_time[process_order]);
                    outprint(current_time, current_time + execution_time, front_process.process_id,
                             front_process.arrival_time, remaining_time[process_order]);
                    current_time = current_time + execution_time;
                    now_running_time[process_order] += execution_time;
                    printf("Queue%d Status:\n", i);
                    printf("Before dequeue:");
                    QueuePrint(ProcessQueue[i]);
                    DeQueue(ProcessQueue[i]);
                    printf("After dequeue:");
                    QueuePrint(ProcessQueue[i]);
                    if (remaining_time[process_order] == 0) {
                        printf("process id:%d have been done\n", front_process.process_id);
                    }
                    else if (now_running_time[process_order] == allotment_time)
                    {
                        if(i ==0){
                            // printf("The process will run in the next period\n");
                            printf("The process will remain to this queue\n");
                            ProcessQueue[i] = EnQueue(ProcessQueue[i], front_process);

                        }
                        else{
                            printf("A process have been added to next queue\n");
                            ProcessQueue[i-1] = EnQueue(ProcessQueue[i-1], front_process);
                        }
                        now_running_time[process_order] = 0;

                    }
                    else if (remaining_time[process_order] > 0) {
                        ProcessQueue[i] = EnQueue(ProcessQueue[i], front_process);
                    }
                    printf("\n");
                    printf("After the process finish the current time is %d\n\n", current_time);
                    current_time--;

                }
            break;
            }
        }
        current_time++;
    }

    free(remaining_time);
    free(queue_index);
    free(now_running_time);
}