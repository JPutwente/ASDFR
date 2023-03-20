#include <iostream>
#include <signal.h>
#include <native/timer.h>
#include <native/task.h>

// Define timer and signal handler
RT_TIMER timer;
RTIME period = 1000000; // 1 millisecond in nanoseconds

void work(void* arg)
{
    // Initialise start time
    RTIME start_time = rt_timer_read();

    // Do computational work
    double sum = 0;
    for(int i = 0; i < 100000; i++)
    {
        sum += i + rand();
    }

    // Initialise end time
    RTIME end_time = rt_timer_read();

    // Log loop timing
    std::cout << "Loop took: " << (end_time - start_time)/1000000 << " ms." << std::endl;
}

void* thread_function(void* arg)
{
    // Create a periodic task
    RT_TASK task;
    rt_task_create(&task, "periodic_task", 0, 99, 0);
    rt_task_set_periodic(&task, TM_NOW, period);

    // Set up signal handler
    signal(SIGRTMIN, work);

    // Wait for signals
    int sig;
    while (1)
    {
        rt_task_wait_period(NULL);
        raise(SIGRTMIN);
    }

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
