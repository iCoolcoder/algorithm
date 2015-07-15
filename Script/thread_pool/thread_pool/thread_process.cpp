#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "thread_process.h"


void ThreadProcess::Process0(void* arg)
{
    printf("thread %u is starting process %s\n", pthread_self(), arg);
    usleep(100 * 1000);
}
void ThreadProcess::Process1(void* arg)
{
    printf("thread %u is starting process %s\n", pthread_self(), arg);
    usleep(100 * 1000);
}

void ThreadProcess::Process2(void* arg)
{
    printf("thread %u is starting process %s\n", pthread_self(), arg);
    usleep(100 * 1000);
}
