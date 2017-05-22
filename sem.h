
typedef int* id_num_type ;


typedef struct sim_PCB
{
    id_num_type       id_num ;  /* Unique id of thread. */
    int               in_Q ;     /* mark if in Q */
    struct sim_PCB *next ; /* For linked list of sim_PCB's in
                                                        sim_semaphore. */
} sim_PCB ;

/* Data space for simulated semaphore object. */

typedef struct sim_sem_data
{


    pthread_mutex_t   lock ;    /* Built-in pthread devices */
    pthread_cond_t    cond ;    /* for synchronization.     */
    int value ; /* Semaphore "counter" or "value". */
    sim_PCB           *Qfront;
    sim_PCB           *Qrear ;
} sim_sem_data ;

/* Simulated seamphore object */

typedef sim_sem_data *sim_semaphore ;



sim_semaphore create_sim_sem(int  init_val) ;



void wait_sem (sim_semaphore sem ) ;


void signal_sem (sim_semaphore sem) ;
