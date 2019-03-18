#include <iostream>
#include "ThreadPool.h"
#include <stdlib.h>
#include <unistd.h>


void *work(void *arg)
{
    char *p = (char *)arg;
    int i = 0;
    while(1)
    {
        printf("\nthreadpool callback function %d:%s\n",i++,p);
        sleep(1);
    }
}

int main()
{
    ThreadPool *pool = new ThreadPool(10, 10);
    char arg1[] = "arg1";
    char arg2[] = "arg2";
    char arg3[] = "arg3";

    pool->tp_addtask(work, (void *)&arg1);
    pool->tp_addtask(work, (void *)&arg2);
    pool->tp_addtask(work, (void *)&arg3);

    sleep(100);
    pool->tp_destroy();

    return 0;
}

