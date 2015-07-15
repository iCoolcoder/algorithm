#include <pthread.h>
#include <stdlib.h>
#include "thread_pool.h"
#include "thread_process.h"
#include "command.h"

int main()
{
    ThreadPool thread_pool;
    thread_pool.InitializeThreads();
    Command command;
    char arg[8] = { 0 };
    for (int i = 1; i <= 1000; ++i)
    {
        command.set_cmd(i % 3);
        sprintf(arg, "%d", i);
        command.set_arg(arg);
        thread_pool.AddWork(command);
    }
    sleep(10); // ���ڲ����̳߳�����
    thread_pool.ThreadDestroy();
    return 0;
}

bool ThreadPool::bshutdown_ = false;
int ThreadPool::icurr_thread_num_ = THREAD_NUM;
std::vector<Command> ThreadPool::command_;
std::map<pthread_t, int> ThreadPool::thread_id_map_;
pthread_mutex_t ThreadPool::command_mutex_ = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ThreadPool::command_cond_ = PTHREAD_COND_INITIALIZER;

void ThreadPool::InitializeThreads()
{
    for (int i = 0; i < THREAD_NUM; ++i)
    {
        pthread_t tempThread;
        pthread_create(&tempThread, NULL, ThreadPool::Process, NULL);
        thread_id_map_[tempThread] = 0;
    }
}

void* ThreadPool::Process(void* arg)
{
    ThreadProcess threadprocess;
    Command command;
    while (true)
    {
        pthread_mutex_lock(&command_mutex_);
        // ����߳���Ҫ�˳������ʱ�˳�
        if (1 == thread_id_map_[pthread_self()])
        {
            pthread_mutex_unlock(&command_mutex_);
            printf("thread %u will exit\n", pthread_self());
            pthread_exit(NULL);
        }
        // ���̲߳���Ҫ�˳���û����Ҫ���������ʱ����Ҫ���ݵ������ݣ�����Ҫ����ȴ��ź�
        if (0 == command_.size() && !bshutdown_)
        {
            if (icurr_thread_num_ >  THREAD_NUM)
            {
                DeleteThread();
                if (1 == thread_id_map_[pthread_self()])
                {
                    pthread_mutex_unlock(&command_mutex_);
                    printf("thread %u will exit\n", pthread_self());
                    pthread_exit(NULL);
                }
            }
            pthread_cond_wait(&command_cond_, &command_mutex_);
        }
        // �̳߳���Ҫ�رգ��ر����е������߳��˳�
        if (bshutdown_)
        {
            pthread_mutex_unlock(&command_mutex_);
            printf("thread %u will exit\n", pthread_self());
            pthread_exit(NULL);
        }
        // ����̳߳ص�����߳��������ڳ�ʼ�߳������������Ҫ����
        if (icurr_thread_num_ < command_.size())
        {
            AddThread();
        }
        // ��������ȡ����������
        std::vector<Command>::iterator iter = command_.begin();
        command.set_arg(iter->get_arg());
        command.set_cmd(iter->get_cmd());
        command_.erase(iter);
        pthread_mutex_unlock(&command_mutex_);
        // ��ʼҵ����
        switch (command.get_cmd())
        {
        case 0:
            threadprocess.Process0(command.get_arg());
            break;
        case 1:
            threadprocess.Process1(command.get_arg());
            break;
        case 2:
            threadprocess.Process2(command.get_arg());
            break;
        default:
            break;
        }
    }
    return NULL; // ��ȫΪ����������(eclipse��д�Ĵ��룬����ܷ���)
}

void ThreadPool::AddWork(Command command)
{
    bool bsignal = false;
    pthread_mutex_lock(&command_mutex_);
    if (0 == command_.size())
    {
        bsignal = true;
    }
    command_.push_back(command);
    pthread_mutex_unlock(&command_mutex_);
    if (bsignal)
    {
        pthread_cond_signal(&command_cond_);
    }
}

void ThreadPool::ThreadDestroy(int iwait)
{
    while (0 != command_.size())
    {
        sleep(abs(iwait));
    }
    bshutdown_ = true;
    pthread_cond_broadcast(&command_cond_);
    std::map<pthread_t, int>::iterator iter = thread_id_map_.begin();
    for (; iter != thread_id_map_.end(); ++iter)
    {
        pthread_join(iter->first, NULL);
    }
    pthread_mutex_destroy(&command_mutex_);
    pthread_cond_destroy(&command_cond_);
}

void ThreadPool::AddThread()
{
    if (((icurr_thread_num_*ADD_FACTOR) < command_.size())
        && (MAX_THREAD_NUM != icurr_thread_num_))
    {
        InitializeThreads();
        icurr_thread_num_ += THREAD_NUM;
    }
}

void ThreadPool::DeleteThread()
{
    int size = icurr_thread_num_ - THREAD_NUM;
    std::map<pthread_t, int>::iterator iter = thread_id_map_.begin();
    for (int i = 0; i<size; ++i, ++iter)
    {
        iter->second = 1;
    }
}
