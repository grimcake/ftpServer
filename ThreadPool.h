#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>


struct job
{
    void* (*callback_function)(void *arg);
    void *arg;
    struct job *next;
};

class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();
    ThreadPool* tp_init(int thread_num, int queue_max_num);
    int tp_addtask(ThreadPool* pool, void* (*callback_function)(void *arg), void *arg);

    int thread_num;  //线程池中开启线程的个数
    int queue_max_num; //队列中最大job的个数
    struct job *head;  //指向job的头指针
    struct job *tail;  //指向job的尾指针
    pthread_t *pthreads;   //线程池中所有线程的ID
    pthread_mutex_t mutex;  //互斥信号量
    pthread_cond_t queue_empty;  //队列为空的条件变量
    pthread_cond_t queue_not_empty;  //队列不为空的条件变量
    pthread_cond_t queue_not_full;   //队列不为满的条件变量
    int queue_cur_num;   //队列当前的job个数
    int pool_close;      //线程池是否已经关闭

private:

};

