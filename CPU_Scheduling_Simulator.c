/* Gil Yaakov
*  Date: 29/12/2020
*  File name: main.c
*  Description: This program is a simulator for
*               running processes according to
*               5 strategies for the scheduler:
*               1) FCFS (First come first serve)
*               2) LCFS (Last come first serve) – Not preemptive
*               3) LCFS (Last come first serve) – Preemptive
*               4) Round Robin with 2 time quantum
*               5) SJF (Shortest job first) – Preemptive
*/

#include <stdio.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <limits.h> 
#define SIZE 1

/**********STRUCT PROCESS**********/
struct process
{
 int arrivalTime;
 int computationTime;
 int waitingTimeFCFS;
 int turnAroundTime;
 int turnAroundTimeLCFSNP;
 int endTime;
 int turnAroundTimeLCFSP;
 int endTimeP;
 int computationTimeRR;
 
};
/**********************************/
/**********HELP FUNCTION FOR SJF(P) TO FIND THE WAITING TIME FOR ALL PROCESSES**********/ 
void findWaitingTime(struct process *proc, int n, int wt[]) 
{ 
	int rt[n];
        int i;
        int j;
	/*copy the burst time into rt[]*/ 
	for (i = 0; i < n; i++){
             rt[i] = proc[i].computationTime;
             }
	int complete = 0, t = 0, minm = INT_MAX; 
	int shortest = 0, finish_time; 
	bool check = false; 
	/*process until all processes gets completed*/ 
	while(complete != n){ 
	      /*find process with minimum remaining time among the processes that arrives till the current time*/ 
	      for(j = 0; j < n; j++){ 
		  if((proc[j].arrivalTime <= t) && (rt[j] < minm) && rt[j] > 0){ 
		     minm = rt[j]; 
		     shortest = j; 
		     check = true; 
		     } 
		  } 
   	      if(check == false){ 
	         t++; 
   	         continue; 
	   	 } 
	      /*reduce remaining time by one*/ 
   	      rt[shortest]--; 
	      /*update minimum*/ 
   	      minm = rt[shortest]; 
   	      if(minm == 0){ 
	         minm = INT_MAX;
                 } 
   	      /*if a process gets completely executed*/ 
   	      if(rt[shortest] == 0){ 
   	         /*increment complete*/ 
   		 complete++; 
   	         check = false; 
   	         /*find finish time of current process*/ 
   		 finish_time = t + 1; 
   	         /*calculate waiting time*/ 
   		 wt[shortest] = finish_time - proc[shortest].computationTime - proc[shortest].arrivalTime; 
   		 if(wt[shortest] < 0){ 
   		    wt[shortest] = 0;
                    } 
   		} 
   		/*increment time*/ 
   		t++; 
	   }
 return; 
} 
/**********HELP FUNCTION FOR SJF TO CALCULATE TURN AROUND TIME**********/ 
void findTurnAroundTime(struct process proc[], int n, int wt[], int tat[]) 
{ 
	/*calculating turnaround time by adding bt[i] + wt[i]*/
        int i; 
	for (i = 0; i < n; i++){
             tat[i] = proc[i].computationTime + wt[i];
             }
        return; 
}
/**********FUNCTION FCFS**********/
void FCFS(struct process *ProcessArr, int Num_Pro)
{         
          int j = 0; /*index for the time*/
          int i = 0; /*index to find the process that came in 'j' time*/
          int flag = 0; /*flag to point it out we have found all the processes*/
          int nowRunning = 0; /*simulate when the CPU won't be busy*/
          float ATA = 0; /*parameter to sum all the turn around times of all the processes*/
          while(1){ /*start the time*/
                while(i < Num_Pro){ /*find which process came in this time*/
                      if(ProcessArr[i].arrivalTime == j){ /*if this process came in this time*/
                         if(flag == 0){ /*it's the first one came*/
                            ProcessArr[i].waitingTimeFCFS = 0;
                            ProcessArr[i].turnAroundTime = ProcessArr[i].computationTime;
                            nowRunning = ProcessArr[i].computationTime;
                            }
                         if(flag != 0){ /*if we allready found the first one*/
                            ProcessArr[i].waitingTimeFCFS = nowRunning; /*the process will waiting the CPU to finish*/
                            nowRunning = nowRunning + ProcessArr[i].computationTime; /*update the CPU availability*/
                            ProcessArr[i].turnAroundTime = ProcessArr[i].waitingTimeFCFS + ProcessArr[i].computationTime; /*calculate the TA of the process*/
                            }
                         flag++; /*update the flag - we find on more process*/
                         }  
                      i++;
                      }
                i = 0;
                if(flag == Num_Pro){ /*if we have found all processes*/
                   break;
                   }
                if(nowRunning > 0){
                   nowRunning = nowRunning - 1; /*the CPU have finished one cycle*/
                   }
                j++;
                }
          /**********CALCULATE & PRINT ATA**********/
          int k = 0;
          while(k < Num_Pro){
                ATA = ATA + ProcessArr[k].turnAroundTime;
                k++;
                }
          printf("FCFS: mean turnaround = %f\n", ATA/Num_Pro);
          return;
}
/*********************************/
/**********LCFS FUNCTION (NOT PREEMPTIVE)**********/
void LCFSNP(struct process *ProcessArr, int Num_Pro)
{ 
            int j = 0; /*index for time*/
            int i = 0; /*index to find the process that came in 'j' time*/
            int flag = 0; /*flag to point it out we have found all the process*/
            int firstP = 0; /*flag to know the place in ProcessArr of the first process*/
            int k = 0; /*index to update earlier process in ProcessArr*/
            float ATA = 0;/*parameter to sum all the turn arounds time of the processes*/
            int nowRunning = 0;
            while(1){/*start the time*/
                  while(i < Num_Pro){/*find which process came in this time*/
                        if(ProcessArr[i].arrivalTime == j){ /*if the process came in this time*/
                           if(flag == 0){ /*if it's the first process*/
                              ProcessArr[i].endTime = ProcessArr[i].arrivalTime + ProcessArr[i].computationTime;
                              firstP = i;
                              nowRunning = ProcessArr[i].computationTime;
                              }      
                           if(flag != 0){ /*if it's not the first*/
                              ProcessArr[i].endTime = ProcessArr[i].arrivalTime + nowRunning + ProcessArr[i].computationTime;
                              if(nowRunning == 0){ /*if there are no process*/
                                 nowRunning = ProcessArr[i].computationTime;
                                 firstP = i;
                                 }
                              while(k < Num_Pro){ /*update the end time of earlier live process*/
                                    if((k != i) && (k != firstP) && (ProcessArr[k].endTime - 1 >= j) && (ProcessArr[k].arrivalTime <= j)){
                                       ProcessArr[k].endTime = ProcessArr[k].endTime + ProcessArr[i].computationTime;
                                       }
                                    k++;
                                   }
                              k = 0;
                              }
                           flag++;
                           }
                        i++;
                        }
                  i = 0;
                  if(flag == Num_Pro){ /*if we have found them all*/
                     break;
                     }
                  if(nowRunning > 0){
                     nowRunning = nowRunning - 1;
                     }
                  j++;
                  }
            /**********CALCULATE & PRINT ATA**********/
            int p = 0;
            while(p < Num_Pro){
                  ProcessArr[p].turnAroundTimeLCFSNP = ProcessArr[p].endTime - ProcessArr[p].arrivalTime;
                  ATA = ATA + ProcessArr[p].turnAroundTimeLCFSNP;
                  p++;
                  }
            printf("LCFS (NP): mean turnaround = %f\n", ATA/Num_Pro);
            return;
}
/**************************************************/
/**********LCFS FUNCTION (PREEMPTIVE)**********/
void LCFSP(struct process *ProcessArr, int Num_Pro)
{ 
            int j = 0; /*index for time*/
            int i = 0; /*index to find the process that came in 'j' time*/
            int flag = 0; /*flag to point it out we have found all the process*/
            int k = 0; /*index to update earlier process in ProcessArr*/
            float ATA = 0;/*parameter to sum all the turn arounds time of the processes*/
            while(1){/*start the time*/
                  while(i < Num_Pro){/*find which process came in this time*/
                        if(ProcessArr[i].arrivalTime == j){ /*if the process came in this time*/
                           ProcessArr[i].endTimeP = ProcessArr[i].arrivalTime + ProcessArr[i].computationTime;
                           while(k < Num_Pro){ /*update the end time of earlier live process*/
                                 if((k != i) && (ProcessArr[k].endTimeP - 1 >= j) && (ProcessArr[k].arrivalTime <= j)){
                                    ProcessArr[k].endTimeP = ProcessArr[k].endTimeP + ProcessArr[i].computationTime;
                                    }
                                 k++;
                                 }
                           k = 0;
                           flag++;
                           }
                        i++;
                        }
                  i = 0;
                  if(flag == Num_Pro){ /*if we have found them all*/
                     break;
                     }
                  j++;
                  }
            /**********CALCULATE & PRINT ATA**********/
            int p = 0;
            while(p < Num_Pro){
                  ProcessArr[p].turnAroundTimeLCFSP = ProcessArr[p].endTimeP - ProcessArr[p].arrivalTime;
                  ATA = ATA + ProcessArr[p].turnAroundTimeLCFSP;
                  p++;
                  }
            printf("LCFS (P): mean turnaround = %f\n", ATA/Num_Pro);
            return;
}
/**************************************************/
/**********SJF (PREEMPTIVE)**********/
void SJF(struct process *ProcessArr, int Num_Pro)
{ 
         int j = 0; /*index for time*/
         int t = 0; /*index to find the process that came in 'j' time*/
         int flag = 0; /*flag to point it out we have found all the process*/
         int l = 0; /*parameter to point on the objects of the array of ProcessArrRR*/
         struct process *ProcessArrSJF = malloc(Num_Pro*sizeof(struct process)); /*dynamic allocation to array process structs*/
         if(ProcessArrSJF == NULL){ /*check failure of malloc*/
            printf("ERROR: dynamic allocation failed\n");
            exit(-1);
            }
         while(1){/*start the time*/
               while(t < Num_Pro){/*find which process came in this time*/
                     if((ProcessArr[t].arrivalTime == j) && (ProcessArr[t].computationTime > 0)){ /*if the process came in this time*/
                        ProcessArrSJF[l].computationTime = ProcessArr[t].computationTime;
                        ProcessArrSJF[l].arrivalTime = ProcessArr[t].arrivalTime;
                        l++;
                        }
                     if(ProcessArr[t].arrivalTime == j){ /*if the process came in this time*/
                        flag++;
                        }
                     t++;
                     }
               t = 0;
               if(flag == Num_Pro){ /*if we have found them all and the last one finished*/
                  break;
                  }
               j++;
               }
         int n = l;
         int i;
         int wt[n], tat[n], total_wt = 0; 
         float total_tat = 0;
         /*Function to find waiting time of all processes*/ 
	 findWaitingTime(ProcessArrSJF, n, wt);
	 /*Function to find turn around time for all processes*/
	 findTurnAroundTime(ProcessArrSJF, n, wt, tat); 
         /*Calculate total waiting time and total turnaround time*/ 
	 for(i = 0; i < n; i++){ 
             total_wt = total_wt + wt[i]; 
             total_tat = total_tat + tat[i];  
	     }  
         free(ProcessArrSJF);
         printf("SJF (P): mean turnaround = %f \n", total_tat/Num_Pro);
         return;
}
/**************************************************/
/**********Round Robin with 2 time quantum**********/
void RR(struct process *ProcessArr, int Num_Pro)
{ 
            int j = 0; /*index for time*/
            int t = 0; /*index to find the process that came in 'j' time*/
            int flag = 0; /*flag to point it out we have found all the process*/
            int l = 0; /*parameter to point on the objects of the array of ProcessArrRR*/
            struct process *ProcessArrRR = malloc(Num_Pro*sizeof(struct process)); /*dynamic allocation to array process structs*/
            if(ProcessArrRR == NULL){ /*check failure of malloc*/
               printf("ERROR: dynamic allocation failed\n");
               exit(-1);
               }
            while(1){/*start the time*/
                  while(t < Num_Pro){/*find which process came in this time*/
                        if((ProcessArr[t].arrivalTime == j) && (ProcessArr[t].computationTime > 0)){ /*if the process came in this time*/
                           ProcessArrRR[l].computationTimeRR = ProcessArr[t].computationTime;
                           ProcessArrRR[l].arrivalTime = ProcessArr[t].arrivalTime;
                           l++;
                           }
                        if(ProcessArr[t].arrivalTime == j){ /*if the process came in this time*/
                           flag++;
                           }
                        t++;
                        }
                  t = 0;
                  if(flag == Num_Pro){ /*if we have found them all and the last one finished*/
                     break;
                     }
                  j++;
                  }
            int i, limit, total = 0, x, counter = 0, time_quantum = 2; /*parameters for the round robin*/
            int wait_time = 0, turnaround_time = 0, arrival_time[l], burst_time[l], temp[l]; /*help parameters*/
            float average_turnaround_time;
            limit = l;
            x = limit;
            /**********UPDATE THE ARRAYS**********/
            for(i = 0; i < limit; i++){
                    arrival_time[i] = ProcessArrRR[i].arrivalTime;
                    burst_time[i] = ProcessArrRR[i].computationTimeRR;
                    temp[i] = burst_time[i];
                }
            /**********CHECK EXTREME CASE**********/
            int check;
            int flag_2 = 0;
            for(check = 0; check < limit - 1; check++){
                if(arrival_time[check] + burst_time[check] < arrival_time[check + 1]){ /*it's means that they are not overlap*/
                   flag_2++;
                   }
                }
            check = 0;
            if(flag_2 == limit - 1){ /*all the processes are not overlap*/
               for(check = 0; check < limit; check++){
                   turnaround_time = turnaround_time + burst_time[check];
                   }
               average_turnaround_time = (float)turnaround_time/Num_Pro;
               printf("RR: mean turnaround = %f \n", average_turnaround_time);
               return;
               }
            /**********CALCULATE**********/
            for(total = 0, i = 0; x != 0;){
                if(temp[i] <= time_quantum && temp[i] > 0){
                   total = total + temp[i];
                   temp[i] = 0;
                   counter = 1;
                   }
                else if(temp[i] > 0){
                        temp[i] = temp[i] - time_quantum;
                        total = total + time_quantum;
                        }
                if(temp[i] == 0 && counter == 1){
                   x--;
                   turnaround_time = turnaround_time + total - arrival_time[i];
                   counter = 0;
                   }
                if(i == limit - 1){
                   i = 0;
                   }
                else if(arrival_time[i + 1] <= total){
                        i++;
                        }
                else{
                     i = 0;
                    }
                }
               average_turnaround_time = turnaround_time * 1.0 / Num_Pro;
               printf("RR: mean turnaround = %f \n", average_turnaround_time);


            free(ProcessArrRR);
            return;
}
/**************************************************/

int main(int argc,char* argv[]){

         int fd_input = open(argv[1],O_RDONLY); /*open fd for input file*/
         if(fd_input == -1){
            perror("open");
            exit(-1);
            }
         int i = 0; /*index for the processes*/
         int Num_Pro; /*variable to the number of processes*/
         char temp_buf[SIZE+2]; /*buffer for the content of input.txt*/
         int read_input = read(fd_input,temp_buf,SIZE+2); /*read the number of processes*/
         if(read_input == -1){
            perror("read");
            exit(-1);
            }
         Num_Pro = atoi(temp_buf);
         struct process *ProcessArr = malloc(Num_Pro*sizeof(struct process)); /*dynamic allocation to array process structs*/
         if(ProcessArr == NULL){ /*check failure of malloc*/
            printf("ERROR: dynamic allocation failed\n");
            exit(-1);
            }
         while(i<Num_Pro){
               char temp2_buf[10]; /*buffer for the arrival time of the process*/
               char temp4_buf[10]; /*buffer for the computation time of the process*/
               char temp3_buf[1]; /*buffer for help*/
               /**********CAREFULLY READING OF ARRIVAL TIME**********/
               int j = 0;
               read_input = read(fd_input,temp3_buf,1);
               if(read_input == -1){
                  perror("read");
                  exit(-1);
                  }
               while(temp3_buf[0] != ','){
                     temp2_buf[j] = temp3_buf[0];
                     read_input = read(fd_input,temp3_buf,1);
                     if(read_input == -1){
                        perror("read");
                        exit(-1);
                        }   
                     j++;
                     }
               /**********CAREFULLY READING OF COMPUTATION TIME**********/
               j = 0;
               read_input = read(fd_input,temp3_buf,1);
               if(read_input == -1){
                  perror("read");
                  exit(-1);
                  }
               while(temp3_buf[0] != '\n'){
                     temp4_buf[j] = temp3_buf[0];
                     read_input = read(fd_input,temp3_buf,1);
                     if(read_input == -1){
                        perror("read");
                        exit(-1);
                        }
                     j++;
                     }
               /**********UPDATE THE PROCESS PARAMETERS**********/
               ProcessArr[i].arrivalTime = atoi(temp2_buf);
               ProcessArr[i].computationTime = atoi(temp4_buf);   
               i++;
               }
         FCFS(ProcessArr,Num_Pro); /*first strategy*/
         LCFSNP(ProcessArr,Num_Pro); /*second strategy*/
         LCFSP(ProcessArr,Num_Pro); /*third strategy*/
         RR(ProcessArr,Num_Pro); /*four strategy*/
         SJF(ProcessArr,Num_Pro); /*five strategy*/
         
close(fd_input);
free(ProcessArr); /*release dynamic allocation*/          
exit(0);
}

