/* Oscar Esparza
** CSC 139 : Operating System Principles
** Assignment 2 : CPU Scheduling Simulation
** 3 March 2019
**
** This program will simulate: 
** First Come First Serve (FCFS)
** Round Robbin (RR)
** Shortest Remaining Time First (SRTF)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// kind of like PCB? 
struct Process
{
    int pid, arrival, timeLeft, lastActive, waitingTime, response;
};

// How we store our processes
struct LinkedList
{
    struct Process data; // important data of the process "node"
    struct LinkedList *next;    // the next process
};

typedef struct LinkedList *node;

// in charge of creating the node
node newNode()
{
    node temp;
    struct Process process;
    
    // initializing our variables
    process.pid = -1;
    process.arrival = -1;
    process.timeLeft = -1;

    // creating the node
    temp = (node)malloc(sizeof(struct LinkedList));
    temp->data = process;
    temp->next = NULL;

    //return the fresh node
    return temp;
}

// in charge of attaching node to linked list
node addNode(node n, struct Process p)
{
    node temp0, temp1;
    temp0 = newNode();
    temp0->data = p;

    // if the current node is empty, add the process
    if (n == NULL)
        n = temp0;

    else
    {
        temp1 = n;
        
        // find the node at the end
        while (temp1->next != NULL)
            temp1 = temp1->next;
        
        // add node to the list
        temp1->next = temp0;
    }

    return n; 
}

// "removes" node n, returns the next node in the list
node removeNode(node n)
{
    node next;
    next = n->next;

    // node just floating somewhere
    return next;
}

//used to get turn around time
int calcTurnAround(int arrival, int current, int turnAround)
{
    int difference = 0;

    difference = current - arrival;
    turnAround = turnAround + difference;
    
    return turnAround;
}

// used to get wait time
int calcWait(int current, int lastActive)
{
    int difference = 0;

    difference = current - lastActive;

    return difference;
}

// used to get the response time
int calcResponse(int arrival, int current, int response)
{
    int difference = 0;

    difference = current - arrival;
    response = response + difference;

    return response;
}

node rebuildNode(node oldNode, node newNode, int time)
{
    struct Process p;

    while(oldNode != NULL)
    {
        p = oldNode->data;

        if (p.arrival <= time)
            newNode = addNode(newNode, p);
        oldNode = removeNode(oldNode);
    }
    return newNode;
}

node sortedInsert (node n, struct Process p)
{
    //need this or else it will loop forever for some reason...
    node this = n; 
    node next = this->next;
    node new;
    node head;
    struct Process newP, nextP;
    new = newNode();
    new = addNode(new, p);
    new = removeNode(new);
    newP = this->data;

    // check if new process is shorter than the first in list
    if (p.timeLeft < newP.timeLeft) 
    {
        new->next = this;
        return new;
    }

    // check is this is the only process
    if (this->next == NULL)  
    {
        this->next = new;
        return this;
    }

    while (this != NULL)
    {
        newP = this->data;
        nextP = next->data;
        if( p.timeLeft < nextP.timeLeft) 
        {
            new->next = next;
            this->next = new;
            return n;
        }
        if(next->next == NULL)
        {
            next->next = new;
            return head;
        }
        next = next->next;
        this = this->next;
    }
    return new;
}

node rebuildNodeSorted(node oldNode, node newNode, int time)
{
    struct Process p0, p1;

    while (oldNode != NULL)
    {
        p0 = oldNode->data;

        if (p0.arrival <= time)
        {
            if (newNode == NULL)
                newNode = addNode(newNode, p0);
            p1 = newNode->data;
            
            if (p1.pid == -1)
            {
                newNode = addNode(newNode, p0);
                newNode = removeNode(newNode);
            }
            else
                newNode = sortedInsert(newNode, p0);
        }
        oldNode = removeNode(oldNode);
    }
    return newNode;
}

node updateArrival(node head, int t)
{
    struct Process p;

    while (head != NULL)
    {
        p = head->data;

        if (p.arrival <= t)
            head = removeNode(head);
        else
            break;
    }

    return head; 
}

// First Come First Serve Simulation
void fcfs(node n)
{
    struct Process process;
    node head = n; 
    node current = n;
    int currentTime = 0; 
    int workTime = 0;
    float turnAroundTime = 0;
    float waitTime = 0;
    float responseTime = 0;
    float cpuSimulation;

    // empty process linked list
    if(current == NULL)
        return; 

    // get the process at head;
    process = current->data;
    responseTime = process.arrival;

    ///////////////////////////////////
    //printf("\n\n%d, %d %d\n\n", process.pid, process.arrival, process.timeLeft);
    //printf("\n\n%d, %d %d\n\n", n->data.pid, n->data.arrival, n->data.timeLeft);

    // loop until you get through all process
    while (current != NULL)
    {
        if (process.arrival <= currentTime)
        {
            // process is done
            if(process.timeLeft == 0)
            {
                printf("<System Time:\t%d>\t\tprocess %d has finished......\n",
                       currentTime, process.pid);
                waitTime += process.waitingTime;
                //get the next process
                current = removeNode(current);
                turnAroundTime = calcTurnAround(process.arrival, currentTime, 
                                                turnAroundTime);
                
                // make sure there is a new process
                if (current != NULL)
                {
                    process = current->data;
                    process.waitingTime += calcWait(currentTime, process.lastActive);

                    while (process.waitingTime > currentTime)
                    {
                        currentTime++;
                        printf("<System time:\t%d>\t\tNo process is currently running.\n",
                         currentTime);
                    }

                    responseTime = calcResponse(process.arrival, currentTime, 
                                                responseTime);
                }

                else
                {
                    printf("<System time:\t%d>\t\tAll processes have finished!\n", 
                           currentTime);
                    break;
                }
            }
            
            printf("<System time:\t%d>\t\tprocess %d is running.\n", currentTime, 
                   process.pid);
            process.timeLeft--;
            workTime++;
        }

        currentTime++;
    }

    cpuSimulation = (((float)workTime / (float)currentTime) * 100);
    printf("--------------------------------------------------------------------------------\n");
    printf("Average CPU usage:\t\t%0.3f\n", cpuSimulation);
    printf("Average waiting time:\t\t%0.3f\n", waitTime/6);
    printf("Average response time:\t\t%0.3f\n", responseTime/6);
    printf("Average turn around time:\t%0.3f\n", turnAroundTime/6);
    printf("--------------------------------------------------------------------------------\n");
}

void srtf(node n, int c)
{
    struct Process process;
    int currentTime = 0;
    int workTime = 0;
    int turnAroundTime = 0;
    int execution = 0;
    float response = 0;
    float waitTime = 0;
    node tempNode = newNode();

    tempNode = rebuildNodeSorted(n, tempNode, currentTime);
    n = updateArrival(n, currentTime);
    process = tempNode->data;
    response = process.arrival;
    process.response = 1;
    //process.waitingTime = 0;

    while(c > 0)
    {
        if (process.arrival <= currentTime)
        {
            workTime++;
            printf("<system time    %d> process    %d is running \n", currentTime, process.pid);
            process.timeLeft--;
            currentTime++;

            if(process.timeLeft <= 0)
            {
                printf("<system time    %d> process    %d is finished.......\n", currentTime, process.pid);
                turnAroundTime = calcTurnAround(process.arrival, currentTime, turnAroundTime);
                waitTime += process.waitingTime;
                tempNode = removeNode(tempNode);
                c--;

                if (c > 0)
                {
                    while (tempNode == NULL)
                    {
                        printf("<System time:\t%d>\t\tNo process is currently running.\n", currentTime);
                        currentTime++;
                        tempNode = rebuildNodeSorted(n, tempNode, currentTime);
                        n = updateArrival(n, currentTime);
                    }

                    process = tempNode->data;
                    process.waitingTime += calcWait(currentTime, process.lastActive);
                    process.lastActive = currentTime;
                    execution = process.timeLeft;

                    if(process.response != 1)
                    {
                        response = calcResponse(process.arrival, currentTime, response);
                        process.response = 1;
                    }
                }

                else 
                {
                    printf("<System time:\t%d>\t\tAll processes have finished!\n", currentTime);
                    break;
                }
            }
            process.lastActive = currentTime;
            tempNode->data = process;
            tempNode = rebuildNodeSorted(n, tempNode, currentTime);
            n = updateArrival(n, currentTime);
            process = tempNode->data;
            process.waitingTime += calcWait(currentTime, process.lastActive);

            if (process.response != 1)
            {
                response = calcResponse(process.arrival, currentTime, response);
                process.response = 1;
            }
        }

        else 
            currentTime++;
    }
    float cpuSimultation = ((float) workTime / (float)currentTime) * 100;
    printf("-----------------------------------------------------------------------------------\n");
    printf("Average CPU usage:\t\t%0.3f\n", cpuSimultation);
    printf("Average waiting time:\t\t%0.3f\n", waitTime/6);
    printf("Average response time:\t\t%0.3f\n", response/6);
    printf("Average turn around time:\t%0.3f\n", (float)turnAroundTime/6);
    printf("-----------------------------------------------------------------------------------\n");
}

// Round Robbin
void rr(node head, int quantum, int tasks)
{
    int currentTime = 0;
    int workTime = 0;
    int turnAroundTime = 0;
    int waitTime = 0;
    int maxQuantum = quantum;
    int currentQuatum = quantum;
    node tempNode = newNode();
    tempNode = rebuildNode(head, tempNode, currentTime);
    tempNode = removeNode(tempNode);
    head = updateArrival(head, currentTime);
    struct Process p = tempNode->data;
    int responseTime = p.arrival;
    p.response = 1;
    int executionTime = p.timeLeft;

    while (tasks > 0)
    {
        if (p.arrival <= currentTime)
        {
            workTime++;
            printf("<system time    %d> process    %d is running\n", currentTime, p.pid);
            p.timeLeft--;
            currentTime++;
            currentQuatum--;

            if (p.timeLeft <= 0)
            {
                printf("<system time    %d> process    %d is finished.......\n", currentTime, p.pid);
                turnAroundTime = calcTurnAround(p.arrival, currentTime, turnAroundTime);
                waitTime += p.waitingTime;
                tempNode = removeNode(tempNode);
                tasks--;

                if(tasks > 0)
                {
                    while (tempNode == NULL)
                    {
                        printf("<system time    %d> no process is running\n", currentTime);
                        currentTime++;
                        tempNode = rebuildNode(head, tempNode, currentTime);
                        head = updateArrival(head, currentTime);
                    }

                    p = tempNode->data;
                    p.waitingTime = p.waitingTime + calcWait(currentTime, p.lastActive);
                    p.lastActive = currentTime;
                    executionTime = p.timeLeft;

                    if(p.response != 1)
                    {
                        responseTime = calcResponse(currentTime, p.arrival, responseTime);
                        p.response = 1;
                    }
                }
                else 
                {
                    printf("<system time    %d> All processes finish ....................\n", currentTime);
                    break;
                }
                currentQuatum = maxQuantum;
            }
            tempNode = rebuildNode(head, tempNode, currentTime);
            head = updateArrival(head, currentTime);

            if (currentQuatum == 0)
            {
                p.lastActive = currentTime;
                tempNode = removeNode(tempNode);
                tempNode = addNode(tempNode, p);
                p = tempNode->data;
                p.waitingTime += calcWait(currentTime, p.lastActive);
                executionTime = p.timeLeft;

                if (p.response != 1)
                {
                    responseTime = calcResponse(currentTime, p.arrival, responseTime);
                    p.response = 1;
                }
                currentQuatum = maxQuantum;
            }
        }
        else
        {
            currentTime++;
            tempNode = rebuildNode(head, tempNode, currentTime);
            head = updateArrival(head, currentTime);
        }
    }

    // hmmm for some reason my response time is correct except negative....
    responseTime = 0 - responseTime;
    float cpu = ((float)workTime/(float)currentTime)*100;
    printf("=================================================================================\n");
    printf("Average Cpu Usage          : %0.2f \n", cpu);
    printf("Average Waiting Time       : %0.2f \n", (float)waitTime/6);
    printf("Average Response Time      : %0.2f \n", (float)responseTime/6);
    printf("Average Turnaround Time    : %0.2f\n", (float)turnAroundTime/6);
    printf("=================================================================================\n");
}

int main(int argc, char *argv[])
{
    char cpuScheduleType[5];    // FCFS | RR | SRTF
    int quantum = -1;
    int runOption = -1;

    if (argc == 3)
    {
        // might need to use &cpuScheduleType
        sscanf(argv[2], "%s", cpuScheduleType);
        if (strcmp(cpuScheduleType, "FCFS") == 0)
        {
            printf("You have chosen the CPU Scheduling Algorithm: %s\n", 
                   cpuScheduleType);
            runOption = 0;
        }

        else if (strcmp(cpuScheduleType, "SRTF") == 0)
        {
            printf("You have chosen the CPU Scheduling Algorithm: %s\n", 
                   cpuScheduleType);
            runOption = 1;
        }

        else
        {
            // didn't choose FCFS | SRTF
            printf("Usage: CPUSchedulingSimulation [File] [FCFS | RR | SRTF] ");
            printf("[Quntum Time]\n");
            return 0;
        }
    }

    else if (argc == 4)
    {
        sscanf(argv[2], "%s", cpuScheduleType);
        sscanf(argv[3], "%d", &quantum);

        if (quantum < 1)
        {
            printf("You need to have a [Quantum] Value >= 1\n");
            return 0;
        }

        if (strcmp(cpuScheduleType, "RR") == 0)
        {
            printf("You have chosen the CPU Scheduling Algorithm: %s\n", 
                   cpuScheduleType);
            runOption = 2;
        }

        else
        {
             // didn't choose FCFS | SRTF
            printf("Usage: CPUSchedulingSimulation [File] [FCFS | RR | SRTF] ");
            printf("[Quntum Time]\n");
            return 0;
        }
    }

    else 
    {
        // didn't choose FCFS | SRTF
        printf("Usage: CPUSchedulingSimulation [File] [FCFS | RR | SRTF] ");
        printf("[Quntum Time]\n");
        return 0;
    }

    // open our input file
    FILE *proccesses = fopen(argv[1], "r");
    node head = newNode();

    //variables for the stuff in input file
    int pid, arrival, timeLeft;
    struct Process process;

    int count = 0; 

    // open our input file
    while (fscanf(proccesses, "%d %d %d", &pid, &arrival, &timeLeft) > 0)
    {
        process.pid = pid;
        process.arrival = arrival;
        process.timeLeft = timeLeft;
        process.response = -1;
        process.waitingTime = 0;
        process.lastActive = process.arrival;
        count++;
        head = addNode(head, process);
    }
    
    head = removeNode(head);
    fclose(proccesses);

    //printf("\n\nTEST:");
    //printf("%d %d", head->data.pid, head->data.timeLeft);
    printf("Total %d task have been read from %s. Press 'Enter' to start!", count,
           argv[1]);
    int c = getc(stdin);

    if(c == '\n')
    {
        if(runOption == 0)
            fcfs(head);
        else if (runOption == 1)
            srtf(head, count);
        else
            rr(head, quantum, count);
    }
    return 0; 
}

