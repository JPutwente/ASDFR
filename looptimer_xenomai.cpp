#include <iostream>
#include <signal.h>
#include <fstream>
#include <sys/syscall.h>


// Define timer and signal handler
timer_t timer;
sigset_t mask;

// Define global variable to monitor the loop time
double loop_time = 0;

std::ofstream myfile;


// function to monitor the global variable
void* writevalue(void* arg)
{
    myfile.open("looptimerlogxenomai.csv", std::ios_base::app);
    myfile << loop_time;
    myfile << "\n";
    myfile.close();
    //std::cout << "Loop took: " << loop_time << " ms." << std::endl;

    return NULL;
}

void work(int signum)
{

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
    //std::cout << "Loop took: " << (end_time.tv_nsec - start_time.tv_nsec)/1000000 << " ms." << std::endl;

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

void* thread_function(void* arg)
{
    // Define structures
    struct sigaction action;
    struct sigevent event;
    struct itimerspec timer_spec;

    // Create a timer
    event.sigev_notify = SIGEV_THREAD_ID;
    event.sigev_notify_thread_id = syscall(__NR_gettid);
    event.sigev_signo = SIGALRM;
    event.sigev_value.sival_ptr = &timer;
    timer_create(CLOCK_MONOTONIC, &event, &timer);

    // Arm timer to 1 ms
    timer_spec.it_interval.tv_sec = 0;
    timer_spec.it_interval.tv_nsec = 1000000;
    timer_spec.it_value.tv_sec = 0;
    timer_spec.it_value.tv_nsec = 1000000;
    timer_settime(timer, 0, &timer_spec, NULL);

    // Set up signal handler
    sigemptyset(&mask);
    sigaddset(&mask, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    action.sa_flags = 0;
    action.sa_handler = work;
    sigemptyset(&action.sa_mask);
    sigaction(SIGRTMIN, &action, NULL);

    // Wait for signals
    int sig;
    while (sigwait(&mask, &sig) == 0);

    return NULL;
}

int main()
{
    // Spawn thread
    pthread_t thread;
    if (pthread_create(&thread, NULL, thread_function, NULL) != 0)
    {
        std::cout << "Error creating thread_function thread." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Wait for thread to finish
    if (pthread_join(thread, NULL) != 0)
    {
        std::cout << "Error joining thread." << std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}