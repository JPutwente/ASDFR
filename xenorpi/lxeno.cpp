#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <sys/syscall.h>

void* print_time (void*);

timer_t timer1;
pthread_t thread;

double loop_time = 0;

std::ofstream myfile;

int main (int argc, char **argv)
{
     pthread_create(&thread, NULL, print_time, NULL);

     pthread_join(thread, NULL);
     exit (EXIT_SUCCESS);
}

void* writevalue(void* arg)
{
    myfile.open("looptimerlogxenomai.csv", std::ios_base::app);
    myfile << loop_time;
    myfile << "\n";
    myfile.close();
    std::cout << "Loop took: " << loop_time << " ms." << std::endl;

    return NULL;
}

void work(int sig)
{

    std::cout << "here 3" << std::endl;
    // Initialise start time
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Do computational work
    double sum = 0;
    for(int i = 0; i < 5990; i++)
    {
        sum = i + rand();
    }

    // Initialise end time
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    // Log loop timing
    loop_time = (end_time.tv_nsec - start_time.tv_nsec)/1000000;

    pthread_t logthread;
    if (pthread_create(&logthread, NULL, writevalue, NULL) != 0)
    {
        std::cout << "Error creating thread_function thread." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (pthread_join(logthread, NULL)  != 0)
    {
        std::cout << "Error joining thread." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void* print_time (void* args)
{
     struct timespec tp;
     char buffer [80];
     struct itimerspec new_value, old_value;
     struct sigaction action;
     struct sigevent sevent;
     sigset_t set;
     int signum = SIGALRM;

     sevent.sigev_notify = SIGEV_THREAD_ID;
     sevent.sigev_notify_thread_id = syscall(__NR_gettid);
     sevent.sigev_signo = signum;

    action.sa_flags = 0;
    action.sa_handler = work;

     sigemptyset(&set);
     sigaddset(&set, signum);
     sigprocmask(SIG_BLOCK, &set, NULL);

     timer_create (CLOCK_MONOTONIC, &sevent, &timer1);

     new_value.it_interval.tv_sec = 0;
     new_value.it_interval.tv_nsec = 1000000;
     new_value.it_value.tv_sec = 0;
     new_value.it_value.tv_nsec = 1000000;

     timer_settime(timer1, 0, &new_value, &old_value);

     while(1){
         /* wait for signal (1 ns) */
         sigwait(&set, &signum);
         clock_gettime(CLOCK_MONOTONIC, &tp);
         work;
         std::cout << tp.tv_nsec/1000000 << std::endl;
     }
}