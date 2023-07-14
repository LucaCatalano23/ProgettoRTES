/** @file PresentazioneEsame.c
 * 
 * @brief A description of the module’s purpose. 
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "PresentazioneEsame.h"
#include "emQueue.h"

typedef struct {
    double a;
    double b;
    int num;
} Temp_t;

void shuffle(void *array[], size_t n) {
    srand((unsigned int)time(NULL)); 
    if (n > 1) {
        size_t i;
        for (i = n - 1; i > 0; i--) {
            size_t j = i + (size_t)rand() / (RAND_MAX / (n - i) + 1);;
            void *t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

void *my_sem_init(const size_t count, const char *name) {
    sem_unlink(name);
    void *retval = sem_open(name, O_CREAT | O_EXCL, S_IRWXU, (int)count);
    return retval;
}

static unsigned int randomInt(const unsigned int lower, const unsigned int upper) {
    return ((unsigned int)rand() % (upper - lower + 1)) + lower;
}

static void pausetta(const unsigned int lower, const unsigned int upper) {
    unsigned int time2Wait = randomInt(lower, upper);
    usleep(time2Wait * 1000);
}

static emQueueHandle_t queue;
static sem_t *sem;

void *writer(void *arg) {
    int nb_writer = *(int*)arg;

    for(int i = 0; i < N_SCRITTURE; i++) {
        Temp_t elem;
        elem.a = 0.0;
        elem.b = elem.a * 2.0;
        elem.num = randomInt(0, 100);
        printf("Writer[%d] writing on buffer...\r\n", nb_writer);
        emQueueReturn_t retval = emQueue_Put(queue, &elem, 0);
        switch (retval) {
        case em_QueueFull:
            printf("Writer[%d] could not put element in the buffer: buffer is full\r\n", nb_writer);
            i--;
            break;
        case em_True:
            printf("Writer[%d] inserted the element: %d; %f; %f\r\n", nb_writer, elem.num, elem.b, elem.a);
            break;
        default:
            printf("Writer[%d]: error on emQueue_Put()\r\n", nb_writer);
            break;
        }
        pausetta(MIN_MS_PAUSA, MAX_MS_PAUSA);
    }

    return NULL;
}

void *reader(void *arg) {
    int nb_reader = *(int*)arg;
    int nb_cycles = N_SCRITTURE;
    while( (emQueue_IsEmpty(queue, 0) != em_True) || (nb_cycles > 0) ) {
        nb_cycles--;
        printf("Reader[%d] is reading the buffer...\r\n", nb_reader);
        Temp_t elem;
        emQueueReturn_t retval = emQueue_Get(queue, &elem, N_QUEUE_PRIORITY);
        switch(retval) {
            case em_QueueEmpty:
                printf("Reader[%d] could not read the buffer: buffer is empty.\r\n", nb_reader);
                break;
            case em_True:
                printf("Reader[%d] read: %d; %f; %f\r\n", nb_reader, elem.num, elem.b, elem.a);
                break;
            default:
                printf("Reader[%d]: error on emQueue_Get()\r\n", nb_reader);
                break;
        }
        pausetta(MIN_MS_PAUSA, MAX_MS_PAUSA);
    }

    return NULL;
}

void enQueue_example_01(void) {
    srand((unsigned int)time(NULL));
    pthread_t thread[N_SCRITTORI + N_LETTORI] = {0} ;
    pthread_attr_t attr = {0};
    pthread_attr_init(&attr);
    size_t nb_elements = L_BUFFER;
    
    (void)sem;
    char sem_name[16];
    snprintf(sem_name, 16, "sem01");

    queue = emQueue_New(nb_elements, sizeof(Temp_t), sem_name, N_QUEUE_PRIORITY);

    int temp[N_LETTORI];
    for(int i = 0; i < N_SCRITTORI; i++) {
        temp[i] = 1000 * (i + 1);
        pthread_create(&thread[i], &attr, writer, (void*)&temp[i]);
    }
    for(int i = 0; i < N_LETTORI; i++) {
        temp[i] += (1000 * N_SCRITTORI);
        pthread_create(&thread[i], &attr, reader, (void*)&temp[i]);
    }

    for(int i = 0; i < N_SCRITTORI + N_LETTORI; i++) {
        pthread_join(thread[i], NULL);
    }

    pthread_attr_destroy(&attr);

}

void deQueue_example_02(void) {
    return;
}


/*** end of file ***/
