#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdatomic.h>
#include <stdlib.h>

typedef struct params{
    int thread_id;
    int process_id;
}params;

int numberThreadsProcess6;
int totalNumberThreadsProcess6Left;

pthread_mutex_t mutex_process_6;
pthread_mutex_t mutex_2_process_6;
pthread_mutex_t mutex_3_process_6;

pthread_cond_t cond_process_6;
pthread_cond_t cond_2_process_6;
pthread_cond_t cond_3_process_6;

sem_t sem_process_6;

int thread_4_4_start;
int thread_4_2_end;


pthread_cond_t cond_process_4;
pthread_mutex_t mutex_process_4;

int ok;
int oprire;

void* process4_thread_3(void *param)
{
    params *s = (params*)param;
    sem_t *global1 = sem_open("Blajan_George_semafor_1", O_CREAT, 0644, 1);
                sem_t *global2 = sem_open("Blajan_George_semafor_2", O_CREAT, 0644, 1);
                sem_wait(global1);
                info(BEGIN, s->process_id, s->thread_id);
                info(END, s->process_id, s->thread_id);
                sem_post(global2);
                sem_close(global1);
                sem_close(global2);
                sem_unlink("Blajan_George_semafor_1");

                return NULL;
}

void* process9(void* param)
{
    params *s=(params*)param;
    if(s->thread_id == 1) {
                sem_t *global = sem_open("Blajan_George_semafor_1", O_CREAT, 0644, 1);
                info(BEGIN, s->process_id, s->thread_id);
                info(END, s->process_id, s->thread_id);
                sem_post(global);
                sem_close(global);

        } else if(s->thread_id == 2) {
                sem_t *global = sem_open("Blajan_George_semafor_2", O_CREAT, 0644, 1);
                sem_wait(global);
                info(BEGIN, s->process_id, s->thread_id);
                info(END, s->process_id, s->thread_id);
                sem_close(global);
                sem_unlink("Blajan_George_semafor_2");

        }
        else{
       info(BEGIN, s->process_id, s->thread_id);
       info(END, s->process_id, s->thread_id);
        }
        return NULL;

}

void* process4(void* param)
{
    params *s=(params*)param;
    pthread_mutex_lock(&mutex_process_4);

    int thread_id=s->thread_id;
    int process_id=s->process_id;

    if(thread_id == 2 && process_id == 4)
    {
        while(thread_4_4_start == 0)
        pthread_cond_wait(&cond_process_4,&mutex_process_4);
    }
    
    if(thread_id == 4 && process_id == 4)
    thread_4_4_start = 1;

    info(BEGIN,process_id,thread_id);

    pthread_cond_signal(&cond_process_4);

    if(thread_id == 4 && process_id == 4)
    {
        while(thread_4_2_end == 0)
        pthread_cond_wait(&cond_process_4,&mutex_process_4);

    }

    if(thread_id == 2 && process_id == 4)
    thread_4_2_end = 1;


    info(END,process_id,thread_id);

    pthread_cond_signal(&cond_process_4);
    pthread_mutex_unlock(&mutex_process_4);


    return NULL;
}


void* process_6_thread(void *param)
{

    sem_wait(&sem_process_6);
    info(BEGIN,6,(int)(long)param);
    pthread_mutex_lock(&mutex_process_6);
    numberThreadsProcess6++;
    pthread_cond_broadcast(&cond_process_6);
    pthread_mutex_unlock(&mutex_process_6);

    if((int)(long)param == 10)
    {
        pthread_mutex_lock(&mutex_process_6);
        ok = 1;
        while(numberThreadsProcess6 != 4)
        {
            pthread_cond_wait(&cond_process_6,&mutex_process_6);
        }
        info(END,6,(int)(long)param);
        totalNumberThreadsProcess6Left++;
        ok = 2;
        numberThreadsProcess6--;
        pthread_cond_broadcast(&cond_2_process_6);
        pthread_cond_broadcast(&cond_3_process_6);
        pthread_mutex_unlock(&mutex_process_6);
    }
    else{
        pthread_mutex_lock(&mutex_process_6);
        if(ok == 1)
        {
            while(ok != 2)
            {
                pthread_cond_wait(&cond_2_process_6,&mutex_process_6);
            }

        }
        totalNumberThreadsProcess6Left++;
        if(totalNumberThreadsProcess6Left >= 43)
        {
            pthread_mutex_lock(&mutex_2_process_6);
            while(ok == 0)
            {
                pthread_mutex_unlock(&mutex_2_process_6);
                pthread_cond_wait(&cond_3_process_6,&mutex_process_6);
            }
            pthread_mutex_unlock(&mutex_2_process_6);

        }
        info(END,6,(int)(long)param);
        numberThreadsProcess6--;
        pthread_mutex_unlock(&mutex_process_6);

    }
    sem_post(&sem_process_6);

    return NULL;

}

int main()
{
    init();
    info(BEGIN, 1, 0);

    sem_unlink("Blajan_George_semafor_1");
    sem_unlink("Blajan_George_semafor_2");

    sem_t* global1 = sem_open("Blajan_George_semafor_1", O_CREAT, 0644, 1);
    sem_t* global2 = sem_open("Blajan_George_semafor_2", O_CREAT, 0644, 1);

    sem_wait(global1);
    sem_wait(global2);

    sem_close(global1);
    sem_close(global2);

    if(fork() == 0)
    {info(BEGIN,2,0);
    if(fork() == 0)
    {info(BEGIN,3,0);
    pid_t pid4;
    pid_t pid5;
    pid4=fork();
    if(pid4 == 0)
    {
        info(BEGIN,4,0);

         params thread_param[4];
                pthread_t tids[4];

                pthread_cond_init(&cond_process_4,NULL);
                pthread_mutex_init(&mutex_process_4,NULL);

                for(int i=0;i<4;i++)
                {
                  thread_param[i].process_id=4;
                  thread_param[i].thread_id=(i+1);
                }

                for(int i=0;i<4;i++){
                if(i == 2)
                pthread_create(&tids[i],NULL,process4_thread_3,&thread_param[i]);
                else
                pthread_create(&tids[i],NULL,process4,&thread_param[i]);
                }

                for(int i=0;i<4;i++)
                pthread_join(tids[i],NULL);


                pthread_cond_destroy(&cond_process_4);
                pthread_mutex_destroy(&mutex_process_4);


        info(END,4,0);
    }
    else
    {
        pid5=fork();
        if(pid5 == 0)
        {
            info(BEGIN,5,0);
            if(fork() == 0)
            {
                info(BEGIN,6,0);

                pthread_t tids[46];
                

                        pthread_cond_init(&cond_process_6,NULL);
                        pthread_cond_init(&cond_2_process_6,NULL);
                        pthread_cond_init(&cond_3_process_6,NULL);
                        pthread_mutex_init(&mutex_process_6,NULL);
                        pthread_mutex_init(&mutex_2_process_6,NULL);
                        pthread_mutex_init(&mutex_3_process_6,NULL);
                        sem_init(&sem_process_6,0,4);


                        for(int i=0;i<46;i++)
                        pthread_create(&tids[i],NULL,process_6_thread,(void*)(long)(i+1));


                if(fork() == 0)
                {
                    info(BEGIN,7,0);
                    pid_t pid8;
                    pid_t pid9;
                    pid8=fork();
                    if(pid8 == 0)
                    {
                        info(BEGIN,8,0);

                        info(END,8,0);
                    }
                    else
                    {
                        pid9=fork();
                        if(pid9 == 0)
                        {
                            info(BEGIN,9,0);

                            pthread_t tids[6];
                                params thread_param[6];


                                for(int i=0;i<6;i++)
                                {
                                    thread_param[i].process_id=9;
                                    thread_param[i].thread_id=(i+1);
                                }

                                for(int i=0;i<6;i++)
                                pthread_create(&tids[i],NULL,process9,&thread_param[i]);

                                for(int i=0;i<6;i++)
                                pthread_join(tids[i],NULL);

                            info(END,9,0);
                        }
                        else
                        {
                            waitpid(pid8,NULL,0);
                            waitpid(pid9,NULL,0);
                            info(END,7,0);
                        }
                        
                    }
                }
                else{
                
                        for(int i=0;i<46;i++)
                        pthread_join(tids[i],NULL);

                        wait(NULL);

                        pthread_mutex_destroy(&mutex_process_6);
                        pthread_mutex_destroy(&mutex_2_process_6);
                        pthread_mutex_destroy(&mutex_3_process_6);
                        pthread_cond_destroy(&cond_process_6);
                        pthread_cond_destroy(&cond_2_process_6);
                        pthread_cond_destroy(&cond_3_process_6);
                        sem_destroy(&sem_process_6);
                info(END,6,0);
                }
            }else{
            wait(NULL);
            info(END,5,0);
            }
        }
        else
        {
            waitpid(pid4,NULL,0);
            waitpid(pid5,NULL,0);
            info(END,3,0);
        }
    }
    }
    else{
    wait(NULL);
    info(END,2,0);
    }
    }
    else
    {
        wait(NULL);
        info(END,1,0);
    }
}


