#include "ThreadPool.h"

ThreadPool::ThreadPool(int thread_num, int queue_max_num)
{
    tp_init(thread_num, queue_max_num);
}

ThreadPool::~ThreadPool()
{

}

void ThreadPool::tp_init(int thread_num, int queue_max_num)
{
    this->thread_num = thread_num;
    this->queue_max_num = queue_max_num;
    queue_cur_num = 0;
    head = nullptr;
    tail = nullptr;
    
    if(pthread_mutex_init(&(mutex), NULL) != 0)
    {
        printf("mutex fail\n");
        return;
    }
    if(pthread_cond_init(&(queue_empty), NULL) != 0)
    {
        printf("queue_empty fail\n");
        return;
    }
    if(pthread_cond_init(&(queue_not_empty), NULL) != 0)
    {
        printf("queue_not_empty fail\n");
        return;
    }
    if(pthread_cond_init(&(queue_not_full), NULL) != 0)
    {
        printf("queue_not_full fail\n");
        return;
    }
    
    pthreads = (pthread_t *)malloc(sizeof(pthread_t)*thread_num);
    if(pthreads == nullptr)
    {
        printf("malloc pthreads fail\n");
        return;
    }

    pool_close = 0;
    for(int i = 0; i<thread_num; i++)
    {
        pthread_create(&(pthreads[i]), NULL, ThreadPool::tp_fun, (void *)this);
    }
    
}

int ThreadPool::tp_destroy()
{
    pthread_mutex_lock(&(mutex));
    if(pool_close)
    {
        pthread_mutex_unlock(&(mutex));
        return -1;
    }

    while(queue_cur_num != 0)
    {
        pthread_cond_wait(&(queue_empty), &(mutex));
    }
    pool_close = 1;
    pthread_mutex_unlock(&(mutex));

    pthread_cond_broadcast(&(queue_not_empty));
    pthread_cond_broadcast(&(queue_not_full));

    for(int i = 0; i<thread_num; i++)
    {
        pthread_join(pthreads[i], NULL);
    }

    pthread_mutex_destroy(&(mutex));
    pthread_cond_destroy(&(queue_empty));
    pthread_cond_destroy(&(queue_not_empty));
    pthread_cond_destroy(&(queue_not_full));

    free(pthreads);

    struct job *p;
    while(head != nullptr)
    {
        p = head;
        head = p->next;
        free(p);
    }

    free(p);
}

int ThreadPool::tp_addtask(void* (*callback_function)(void *arg), void *arg)
{
    pthread_mutex_lock(&(mutex));
    while((queue_cur_num == queue_max_num) && !(pool_close))
    {
        pthread_cond_wait(&(queue_not_full), &(mutex));
    }
    if(pool_close)
    {
        pthread_mutex_unlock(&(mutex));
        return -1;
    }

    struct job *pjob = (struct job *)malloc(sizeof(struct job));
    if(pjob == nullptr)
    {
        pthread_mutex_unlock(&(mutex));
        return -1;
    }

    pjob->callback_function = callback_function;
    pjob->arg = arg;
    pjob->next = nullptr;

    if(head == nullptr)
    {
        head = tail = pjob;
        pthread_cond_broadcast(&(queue_not_empty));
    }
    else
    {
        tail->next = pjob;
        tail = pjob;
    }
    
    queue_cur_num++;
    pthread_mutex_unlock(&(mutex));
    return 0;
}

void* ThreadPool::tp_fun(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;
    struct job *pjob = nullptr;

    while(1)
    {

        pthread_mutex_lock(&(pool->mutex));
        while((pool->queue_cur_num == 0) && !(pool->pool_close))
        {
            pthread_cond_wait(&(pool->queue_not_empty), &(pool->mutex));
        }
        if(pool->pool_close)
        {
            pthread_mutex_unlock(&(pool->mutex));
            pthread_exit(NULL);
        }

        pool->queue_cur_num--;
        pjob = pool->head;
        if(pool->queue_cur_num == 0)
        {
            pool->head = pool->tail = nullptr;
        }
        else
        {
            pool->head = pjob->next;
        }
        
        if(pool->queue_cur_num == 0)
        {
            pthread_cond_signal(&(pool->queue_empty));
        }
        if(pool->queue_cur_num == pool->queue_max_num-1)
        {
            pthread_cond_broadcast(&(pool->queue_not_full));
        }

        pthread_mutex_unlock(&(pool->mutex));

        (*(pjob->callback_function))(pjob->arg);
        free(pjob);
        pjob = nullptr;
    }
}
