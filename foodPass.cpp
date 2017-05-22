#include <iostream>
#include <sched.h>
#include <time.h>
#include <pthread.h>
#include <string>
#include "sem.h"

using namespace std ;

//const int numTrivets = 6 ;
const int numTrivets = 3 ;
const int numDiners = numTrivets - 1 ;
const int maxDishNames = 13 ;
//const int numDishNames = 13 ;
const int numDishNames = 5 ;

int trivet[numTrivets] ;

string dishName[maxDishNames];

//create arrays of full and empty to the length of numTrivets
sim_semaphore full[numTrivets];

sim_semaphore empty[numTrivets];


pthread_t child_t[numTrivets] ;


int checking ;


pthread_mutex_t stdoutLock ;

struct threadIdType
{
   int id ;
};

void init()
{
  int index ;

  srandom(time((time_t *) 0));

  checking = 0 ;


  if ( 0!=pthread_mutex_init(&stdoutLock, NULL) )
  {  cout << "MUTEX INITIALIZATION FAILURE!" << endl;
     exit(-1) ;}


  for (index=0; index<numTrivets; index++) trivet[index]=0;


  for (index=0; index<numTrivets; index++) full[index]=create_sim_sem(0);

  for (index=0; index<numTrivets; index++) empty[index]=create_sim_sem(1);



  dishName[0]="no dish";
  dishName[1]="vegetable soup" ;
  dishName[2]="bread and butter" ;
  dishName[3]="beets and chickpeas" ;
  dishName[4]="hardboiled eggs" ;
  dishName[5]="calf tongue" ;
  dishName[6]="baked potato" ;
  dishName[7]="string beans" ;
  dishName[8]="rack of lamb" ;
  dishName[9]="salad" ;
  dishName[10]="coffee" ;
  dishName[11]="flan" ;
  dishName[numDishNames-1]="check" ;

}

void delayAsMuchAs (int limit)
{
  int time, step;
  time=(int)random()%limit;
  for (step=0;step<time;step++) sched_yield() ;
}


void * Server(void * ignore)
{

  int i, j, delayLimit=100 ;

  for (i=1; i<numDishNames; i++)
  {


    delayAsMuchAs(delayLimit);


      wait_sem(empty[0]);


    trivet[0]=i; // put dish #i onto trivet #0.
    pthread_mutex_lock(&stdoutLock) ;
    cout << "Server places " << dishName[trivet[0]]
         << " on trivet #0." << endl ;
    pthread_mutex_unlock(&stdoutLock);


      signal_sem(full[0]);


  }
  pthread_exit ((void *)0) ;
}

void * Diner(void * postnPtr)
{

  int position = ((threadIdType *) (postnPtr))->id ;
  int i, j, delayLimit=100 ;

  for (i=1; i<numDishNames; i++)
  {

    delayAsMuchAs(delayLimit);


    wait_sem(full[position]);


    /* I declare what I am doing */
    pthread_mutex_lock(&stdoutLock) ;
    cout << "Diner number "<< position ;
    if (i<numDishNames-1) cout << " enjoys ";
    else if (position<numDiners-1) cout << " examines " ;
         else cout << " examines and pays " ;

    cout << dishName[trivet[position]] << endl ;
    pthread_mutex_unlock(&stdoutLock);


    delayAsMuchAs(delayLimit);

      wait_sem(empty[position+1]);


    pthread_mutex_lock(&stdoutLock) ;
    cout << "Diner number "<< position << " moves "
         << dishName[trivet[position]] << " from trivet #"
         << position << " to trivet #" << position+1 << endl;
    pthread_mutex_unlock(&stdoutLock);
       /* transfer the dish on my left to trivet on my right */
    trivet[position+1]=trivet[position] ;
      /* mark trivet on my left as empty */
    trivet[position]=0;

    signal_sem(empty[position]);
    signal_sem(full[position+1]);


  }
  pthread_exit ((void *)0) ;
}

void * Busser (void * ignore)
{

  int i, j, delayLimit=100 ;

  for (i=1; i<numDishNames; i++)
  {

        /* I delay a random time before I "feel like" bussing another
	   dish.*/

    delayAsMuchAs(delayLimit);

      wait_sem(full[numTrivets-1]);


    pthread_mutex_lock(&stdoutLock) ;
    cout << "Busser removes "
         << dishName[trivet[numTrivets-1]] << " from trivet #"
	 << numTrivets-1<< "." << endl ;
    pthread_mutex_unlock(&stdoutLock);
    trivet[numTrivets-1]=0; // remove the dish.


    signal_sem(empty[numTrivets-1]);


  }
  return (void *) 0 ;
}

int main()
{
  init();

  cout << endl << endl;
  cout << "Welcome to the restaurant!" << endl ;
  cout << numDiners << " will be dining." << endl ;
  cout << "The meal will consist of " << numDishNames-2
       << " dishes." << endl;
  cout << "Bon appetit!" << endl ;
  cout << endl << endl;

  int i;

   threadIdType * idPtr ;

  for (i=0; i<numDiners; i++)
  {

      idPtr = new threadIdType ; /* allocate memory for struct */
      idPtr->id = i ;  /* records current index as the child's ID */

     if (0!=pthread_create(&child_t[i], NULL, Diner, (void *) idPtr))
        {cout << "THREAD CREATION FAILURE!" << endl; exit(-1) ;}

     if (0!=pthread_detach(child_t[i]))
        {cout << "THREAD DETACHMENT FAILURE!" << endl ; exit(-1) ;}
  }

     if (0!=pthread_create(&child_t[numDiners], NULL, Server, (void *) 0))
        {cout << "THREAD CREATION FAILURE!" << endl; exit(-1) ;}

     if (0!=pthread_detach(child_t[numDiners]))
        {cout << "THREAD DETACHMENT FAILURE!" << endl ; exit(-1) ;}

  Busser((void *)0) ;

  cout << endl << endl;
  cout << "Thank you for coming!" << endl ;
  cout << endl << endl;

  return 0 ;

}
