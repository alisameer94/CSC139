/* Oscar Esparza
** Professor Cheng
** CSC 139 - Operating Systems Principles
** Assignment 4 - Virtual Memory Simulation
*/ 

#include <stdio.h>
#include <string.h> //strcmp()
#include <stdlib.h> //sscanf()

struct Frame{
    int last, next, value;
};

void fifo(int pages, int frames, int requests, struct Frame *list);
void lru(int pages, int frames, int requests, struct Frame *list);
void opt(int pages, int frames, int requests, struct Frame *list);

int main(int argc, char const *argv[]){
    int flag, frames, pages, requests;
    int requestPage;
    char scheduleType[5];

    if(argc != 3){
        printf("Error, Incorrect Number of Arguments\n");
        return 0;
    }

    sscanf(argv[2], "%s", scheduleType);
    //*********************** Find the schendule type**************************** 
    if(strcmp(scheduleType, "FIFO") == 0) flag = 0;
    else if(strcmp(scheduleType, "LRU") == 0) flag = 1;
    else if(strcmp(scheduleType, "OPT") == 0) flag = 2;
    else {
        printf("ERROR INPUT\nExample Input: ./vms input1.txt FIFO\n");
        return 0;
    }
    //*************************Open File & Get Info*****************************
    FILE *info = fopen(argv[1], "r");
    fscanf(info, "%d %d %d", &pages, &frames, &requests);
    //printf("Pages: %d\nFrames: %d\nRequests: %d\n", pages, frames, requests);
    printf("(%s)\n", scheduleType);

    struct Frame list[requests]; // how many frames will we have? 
    //int test = 0;
    for(int i = 0; fscanf(info, "%d", &requestPage) > 0; i++){
        list[i].value = requestPage;
        list[i].last = -999;
        list[i].next = -999; 
        //test++;
    }
    
    fclose(info);
    /*for(int i = 0; i < test; i++){
        printf("%d ", list[i].value);
    }*/

    //*************************Simulate Virtual Memory***************************
    if(flag == 0) fifo(pages, frames, requests, list);
    else if(flag == 1) lru(pages, frames, requests, list);
    else if(flag == 2) opt(pages, frames, requests, list);
    return 0;
}

void fifo(int pages, int frames, int requests, struct Frame *list){
    int currentRequest, foundFlag;
    int frameCount, faultCount = 0;
    int holdFrame[frames];

    for(int i = 0; i <= (requests - 1); i++){
        foundFlag = 0;
        currentRequest = list[i].value;

        for(int j = 0; j <= (frames - 1); j++){
            if(currentRequest == holdFrame[j]){
                printf("Page %d already in Frame %d\n", currentRequest, j);
                foundFlag = 1;
            }
        }

        if(foundFlag == 0){
            if((holdFrame[frameCount] > 0) && (holdFrame[frameCount] <= pages)){
                 printf("Page %d unloaded from Frame %d, Page %d loaded into Frame %d\n",
                 holdFrame[frameCount], frameCount, currentRequest, frameCount);
                 holdFrame[frameCount] = currentRequest;
                 frameCount++;
                 faultCount++;

                 if(frameCount >= frames)
                    frameCount = 0;
            }

            else {
                holdFrame[frameCount] = currentRequest;
                printf("Page %d loaded into frame %d\n", currentRequest, frameCount);
                 frameCount++;
                 faultCount++;

                 if(frameCount >= frames)
                    frameCount = 0;
            }
        }
    }
    printf("%d page faults\n", faultCount);
}

void lru(int pages, int frames, int requests, struct Frame *list){
    int currentRequest, changeFrame, foundFlag, nextCounter, nextFound;
    struct Frame holdFrame[frames];
    int frameCount, frameFull, faultCount, k = 0;


    for(int i = 0; i <= (requests - 1); i++){
        foundFlag = 0;
        currentRequest = list[i].value;

        for(int j = 0; j <= (frames - 1); j++){
            if(currentRequest == holdFrame[j].value){
                printf("Page %d already in Frame %d\n", currentRequest, j);
                holdFrame[j].last = -1;
                foundFlag = 1;
            }
        }
        if(foundFlag == 0){
            if(frameFull == 1){
                frameCount = 0;
                changeFrame = 0;

                for(frameCount; frameCount <= (frames - 1); frameCount++){
                    if(holdFrame[frameCount].last > holdFrame[changeFrame].last)
                        changeFrame = frameCount;
                } //might not need this bracket

                printf("Page %d unloaded from Frame %d, Page %d loaded into Frame %d\n",
                holdFrame[changeFrame].value, changeFrame, currentRequest, changeFrame);
                holdFrame[changeFrame].value = currentRequest;
                holdFrame[changeFrame].last = -1;
                faultCount++;
            }

            else {
                holdFrame[frameCount].value = currentRequest;
                printf("Page %d loaded into Frame %d\n", currentRequest, frameCount);
                holdFrame[frameCount].last = -1;
                frameCount++;
                faultCount++;
                if(frameCount >= frames) frameFull = 1;
            }
        }
        for(int j = 0; j <= (frames -1); j++) holdFrame[j].last++;
    }
    printf("%d page faults\n", faultCount);
}

void opt(int pages, int frames, int requests, struct Frame *list)
{
    int j, k, currentRequest, foundFlag, nextFound;
    int frameCount, faultCount, nextCounter, framesFullAmt, changeFrame;
    struct Frame holdFrame[frames];

    frameCount = 0;
    faultCount = 0;
    framesFullAmt = 0;

    for(int i = 0; i <= (requests - 1); i++){
        foundFlag = 0;
        currentRequest = list[i].value;

        //for some reason if I initialize j here I get Segemntation Fault
        for (j = 0; j <= (frames - 1); j++) {
            if(currentRequest == holdFrame[j].value){
                printf("Page %d already in frame %d\n", currentRequest, j);
                k = i + 1;
                nextCounter = 1;
                nextFound = 0;
                for (k; k <= requests-1; k++){
                    if(holdFrame[frameCount].value == list[k].value){
                        nextFound = 1;
                        break;
                    }
                    nextCounter++;
                }

                if(nextFound == 1) nextCounter = -999;

                holdFrame[frameCount].next = nextCounter;
                foundFlag = 1;
            }
        }

        if(foundFlag == 0){
            if(framesFullAmt == 1){
                frameCount = 0;
                changeFrame = 0;
                for(frameCount; frameCount <= (frames - 1); frameCount++){
                    if(holdFrame[frameCount].next > holdFrame[changeFrame].next)    
                        changeFrame = frameCount;  
                }

                printf("Page %d unloaded from frame %d, page %d loaded into frame %d\n", 
                holdFrame[changeFrame].value, changeFrame, currentRequest, changeFrame);
                holdFrame[changeFrame].value = currentRequest;
                k = i + 1;
                nextCounter = 1;
                nextFound = 0;

                for(k; k <= requests - 1; k++){
                    if(holdFrame[frameCount].value == list[k].value){
                        nextFound = 1;
                        break;
                    }
                    nextCounter++;
                }

                if(nextFound == 1) nextCounter = -999;
                holdFrame[frameCount].next = nextCounter;
                faultCount++;
            }

            else{
                holdFrame[frameCount].value = currentRequest;
                printf("Page %d loaded into frame %d\n", currentRequest, frameCount);
                k = i + 1;
                nextCounter = 1;
                nextFound = 0;

                for(k; k<= requests-1; k++){
                    if(holdFrame[frameCount].value == list[k].value){
                        nextFound = 1;
                        break;
                    }
                    nextCounter++;
                }

                if(nextFound == 1) nextCounter = -999;
                holdFrame[frameCount].next = nextCounter;
                frameCount++;
                faultCount++;
                if(frameCount >= frames) framesFullAmt = 1;
            }
        }
    }
    printf("%d page faults\n", faultCount);
}