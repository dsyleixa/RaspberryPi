/*   
   pthread framework

*/ 
 
// (C) dsyleixa 2015, 2016
// freie Verwendung fuer private Zwecke
// fuer kommerzielle Zwecke nur nach Genehmigung durch den Autor.
// Programming language: gcc  C/C++, Geany IDE
// protected under the friendly Creative Commons
// Attribution-NonCommercial-ShareAlike 3.0 Unported License
// http://creativecommons.org/licenses/by-nc-sa/3.0/
 


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <errno.h>

#include <pthread.h>


#define  byte  uint8_t


//  semaphores
volatile uint8_t   _TASKS_ACTIVE_= 1;




//*************************************************************//*************************************************************
//                             tasks                                                           tasks     
//*************************************************************//*************************************************************



void* thread0Name (void* ) {   

    while(_TASKS_ACTIVE_) {
     
    }      
    return NULL;
}


//*************************************************************//*************************************************************


void* thread1Name(void *) {    
  
   while(_TASKS_ACTIVE_) {   
            
   }
   return NULL;
}


//*************************************************************//*************************************************************


void* thread2Name (void* ) 
   
   while(_TASKS_ACTIVE_) {      
 
   }
   return NULL;
}




//*************************************************************//*************************************************************



void* thread3Name (void* ) { 
      
   while(_TASKS_ACTIVE_) {   
 
   }
   return NULL;
}


//*************************************************************//*************************************************************



void* thread4Name (void* ) {     
  
    while(_TASKS_ACTIVE_) {   
 
   }
    return NULL;
}


//*************************************************************//*************************************************************


void* thread5Name(void *) {

   while(_TASKS_ACTIVE_) {   
     
   }  
   return NULL;
}

//*************************************************************//*************************************************************

void* thread6Name (void* ) {       
       
   while(_TASKS_ACTIVE_) {   
     
   }
   return NULL;
}  
      



//*************************************************************//*************************************************************
//                           main()                            //                            main()
//*************************************************************//*************************************************************


int main() {
    int   iores;
    
    void  *retval0=NULL, *retval1=NULL, *retval2=NULL, *retval3=NULL, *retval4=NULL, *retval5=NULL, *retval6=NULL;
    
    sleep(1);


    
    pthread_t thread0, thread1, thread2, thread3, thread4, thread5, thread6;
    struct  sched_param  param;

    pthread_create(&thread0, NULL, thread0Name, NULL);    // low priority task: 
    param.sched_priority = 20;
    pthread_setschedparam(thread0, SCHED_RR, &param);
   
    pthread_create(&thread1, NULL, thread1Name, NULL);    // low priority:  
    param.sched_priority = 20;
    pthread_setschedparam(thread1, SCHED_RR, &param);
   
    pthread_create(&thread2, NULL, thread2Name, NULL);    // medium  priority:  
    param.sched_priority = 40;
    pthread_setschedparam(thread2, SCHED_RR, &param);
   
    pthread_create(&thread3, NULL, thread3Name, NULL);    // highest priority:  
    param.sched_priority = 80;
    pthread_setschedparam(thread3, SCHED_FIFO, &param);
   
    pthread_create(&thread4, NULL, thread4Name, NULL);    // medium priority:   
    param.sched_priority = 40;
    pthread_setschedparam(thread4, SCHED_RR, &param);
 
    pthread_create(&thread5, NULL, thread5Name, NULL);    // medium priority:  
    param.sched_priority = 40;
    pthread_setschedparam(thread5, SCHED_RR, &param);
    
    pthread_create(&thread6, NULL, thread6Name, NULL);    // medium priority:  
    param.sched_priority = 40;
    pthread_setschedparam(thread6, SCHED_RR, &param);
 


    while(_TASKS_ACTIVE_) { delay(1); }
     
    // wait for threads to join before exiting
    pthread_join(thread0, &retval0);
    pthread_join(thread1, &retval1);
    pthread_join(thread2, &retval2);
    pthread_join(thread3, &retval3);
    pthread_join(thread4, &retval4);
    pthread_join(thread5, &retval5);
    pthread_join(thread6, &retval6);
   
   
    exit(0);
}

//======================================================================
//======================================================================
