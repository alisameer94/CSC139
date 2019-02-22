/* Oscar Esparza
** CSC 139 
** Assignment 1 : Observing Linux Behavior
** 17 February 2019
**
** PART B: [./a.out]
** CPU type & Model
** Kernel Version
** Uptime in dd:hh:mm:ss
** Date & Hostname
**
** PART C [./a.out -s]
** Time in user, system mode, & idle
** Disk requests
** Context switches
** Time last booted
** Processes created since last boot
**
** PART D [./a.out -l 2 60]
** memory configured into this computer
** memory currently available
** list of load averages
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>   //getimeofday() 
#include <time.h>
#include <unistd.h> // sleep()

//void cpuType(void);

int main(int argc, char *argv[])
{
    FILE *file;
    time_t rawTime;
    struct timeval timeValue;
    struct tm * realTime;
    char buffer[100];       // makes sure we have enough space
    float upTime, idleTime; // originally in secounds
    int secs, mins, hrs, days;
    char repTypeName[16];
    int isStandard, isShort, isLong;  // flags for which part to use
    char dash, character;
    char trash0[100], trash1[100], trash2[100], trash3[100], trash4[100];
    char user[100], sys[100], idle[100]; //cpu time in user, sys, idle 
    // for part c
    int interval, duration; 
    int correctFlag;
    char reads[100];
    char writes[100];
    float tempLoad = 0;
    int iteration = 0;

    /******************CPU type & model***************/
    void cpuType()
    {
        // error detection
        if((file = fopen("/proc/cpuinfo", "r")) == NULL)
        {
            printf("Error getting CPU TYPE & MODEL\n");
            return;
        }

        // skip the next 3 lines in /proc/cpuinfo
        for (int i = 0; i < 4; i++)
            fgets(buffer, 100, file);
        //get cpu info
        printf("%s", fgets(buffer, 100, file));
        printf("\n");
        fclose(file);
    }

    /******************Kernal Version*****************/
    void kernalVersion()
    {
        // error detection
        if((file = fopen("/proc/version", "r")) == NULL)
        {
            printf("Error getting KERNAL VERSION\n");
            return;
        }

        // get kernal version
        printf("Kernal Version : ");
        printf("%s", fgets(buffer, 100, file));
        printf("\n");
    }

    /**************Uptime in dd:hh:mm:ss**************/
    void uptime()
    {
        // error detection
        if((file = fopen("/proc/uptime", "r")) == NULL)
        {
            printf("Error getting UPTIME\n");
            return;
        }

        fscanf(file, "%f %f", &upTime, &idleTime);
        // Some math to get dd:hh:mm:ss
        if(upTime < 86400)
            days = 0;
        else
        {
            days = (int)(upTime / (60 * 60 * 24));
            // remove days in secs from running total since it is accounted for 
            //now
            upTime -= days * (60 * 60 * 24); 
        }

        if(upTime < 3600)
            hrs = 0;
        else 
        {
            hrs = (int)(upTime / (60 * 60));
            // remove hours in secs from running total since it is accounted for 
            //now
            upTime -= hrs * (60 * 60);
        }
        
        mins = (int)(upTime / 60);
        // remove minutes in secs from running total since it is accounted for now
        upTime -= mins * 60;
        // casting as an int to remove decimal
        secs = (int)(upTime);

        printf("\nUptime(d:h:m:s) : %i:%i:%i:%i\n\n", days, hrs, mins, secs);
    }

    /******************************DATE*************************/
    void date(void)
    {
        gettimeofday(&timeValue, NULL);
        rawTime = timeValue.tv_sec;

        time(&rawTime);
        realTime = localtime(&rawTime);

        strftime(buffer, 30, "%m-%d-%Y %T.", realTime);
        printf("Time : ");
        printf("%s%ld\n", buffer, timeValue.tv_usec);
    }

    /******************************Host*************************/
    void hostname(void)
    {
        // error detection
        if ((file = fopen("/proc/sys/kernel/hostname", "r")) == NULL)
        {
            printf("Error getting HOSTNAME\n" );
            return;
        }

        printf("\nHostname : ");
        printf("%s", fgets(buffer, 100, file));
        printf("\n");
    }

    /**********Time in user, system mode, & idle**************/
    void cpuUserSysIdle(void)
    {
        // error detection
        if((file = fopen("/proc/stat", "r")) == NULL)
        {
            printf("Error getting CPU TIME in USER, SYS & IDLE");
            return;
        }

        printf("Time CPU has Spend in :\n");
        fscanf(file, "%s %s %s %s %s", trash0, user, trash1, sys, idle);
        printf("User Mode : %s\n", user);
        printf("System Mode : %s\n", sys);
        printf("Idle : %s\n", idle);
    }
    
    /************************Disk requestse***********************/
    void diskRequests(void)
    {
        correctFlag = 1;
        char sda[100] = "sda"; // line that has correct info
        char str[100];
        if((file = fopen("/proc/diskstats", "r")) == NULL);
        {
            printf("\nError getting DISK REQUESTS\n");
            return;
        }
        
        // loop until you get to correct line
        while (correctFlag)
        {
            fscanf(file, "%s", str);
            correctFlag = strcmp(sda, str);

            // on the correct line so grab stuff
            if(correctFlag == 0)
            {
                fscanf(file, "%s", reads);
                fscanf(file, "%s", trash0);
                fscanf(file, "%s", trash1);
                fscanf(file, "%s", trash2);
                fscanf(file, "%s", trash3);
                fscanf(file, "%s", trash4);
                fscanf(file, "%s", writes);
                
                printf("\nReads: %s\nWrites: %s\n\n", reads, writes);
            }
        }
    }

    //**********************Context switches*******************
    void contextSwitch(void)
    {
        char test[100], ctxt[100];
        int flag = 1;

        if((file = fopen("/proc/stat", "r")) == NULL)
        {
            printf("\nError getting CONTECT SWITCHES\n");
            return;
        }

        while(flag)
        {
            fscanf(file, "%s", test);
            flag = strcmp(test, "ctxt");
            if(!flag)
            {
                fscanf(file, "%s", ctxt);
                printf("\nNumber of context switches: ");
                printf("%s\n", ctxt);
            }
        }
    }

    //**********************Last Boot*******************
    void lastBoot(void)
    {
        return;
    }

    //**********************Processes Created*******************
    void proccessCreated(void)
    {
        char test[100], proc[100];
        int flag = 1;

        if((file = fopen("/proc/stat", "r")) == NULL)
        {
            printf("\nError getting PROCESS CREATED\n");
            return;
        }

        while(flag)
        {
            fscanf(file, "%s", test);
            flag = strcmp(test, "processes");
            if(!flag)
            {
                fscanf(file, "%s", proc);
                printf("Number of processes since boot: ");
                printf("%s\n", proc);
            }
        }
    }

    //**********************Total Memory*******************
    void totalMemory(void)
    {
        char mem[100];
        if((file = fopen("/proc/meminfo", "r")) == NULL)
        {
            printf("\nError getting TOTAL MEMORY\n");
            return;
        }
        fscanf(file, "%s", trash0); //holds "memtotal:"
        fscanf(file, "%s", mem);
        printf("\nTotal memory available : %s kB\n", mem);
    }

    //**********************Total Memory*******************
    void availableMemory(void)
    {
        char mem[100];
        if((file = fopen("/proc/meminfo", "r")) == NULL)
        {
            printf("\nError getting AVAILABLE MEMORY\n");
            return;
        }
        fscanf(file, "%s", trash0); //holds "memtotal:"
        fscanf(file, "%s", trash1);
        fscanf(file, "%s", trash2);
        fscanf(file, "%s", trash2);
        fscanf(file, "%s", mem);
        printf("\nTotal free memory : %s kB\n", mem);
    }

    void sampleLoadAvg()
    {
        if ((file = fopen("/proc/loadavg","r")) == NULL)
        {
            printf( "Error getting LOAD AVERAGE\n" );
            return;
        }
        
        fscanf(file, "Load avg: %f", &tempLoad);
        printf("%f", tempLoad);
        printf("\n");
        //tempLoad = 1;
        //fclose(file);
    }

    // PART B by default
    strcpy(repTypeName, "Standard");

    // Check if there is an argument in cmd line
    if(argc > 1)
    {
        // get the -s or -l
        sscanf(argv[1], "%c%c", &dash, &character);
        if(dash != '-')
        {
            fprintf(stderr, "usage: observer [-s][-l int dur]\n");
            return 1;
        }

        if(character == 's')
            strcpy(repTypeName, "Short");
        
        if(character == 'l')
        {
            strcpy(repTypeName, "Long");
            interval = atoi(argv[2]);
            duration = atoi(argv[3]);
        }
    }

    //set flags
    isStandard = strcmp(repTypeName, "Standard");
    isShort = strcmp(repTypeName, "Short");
    isLong = strcmp(repTypeName, "Long");

    if(isStandard == 0)
    {
        cpuType();
        kernalVersion();
        uptime();
        date();
        hostname();
    }

    if(isShort == 0)
    {
        cpuUserSysIdle();
        diskRequests();
        contextSwitch();
        lastBoot();
        uptime(); //is this what he meant?
        proccessCreated();
    }

    if(isLong == 0)
    {
        totalMemory();
        availableMemory();
        printf("Load Averages : \n");
        while (iteration < duration)
        {
            sleep(interval);
            sampleLoadAvg();
            iteration += interval;
        }
    }
}
