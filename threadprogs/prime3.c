    #include <stdio.h>
    #include <pthread.h>
    //#include <wiringPi.h>

    typedef struct
    {
        unsigned long start;
        unsigned long end;
        unsigned long step;
    }data;

    
    // ---------------------------------------
    // Prime calculation functions  

    unsigned long isPrime(unsigned long number)
    {
        unsigned long i;
        for(i=2; i <= (number/2); i++)
        {
            if(number % i == 0)
            {
                //not a prime
                return 0;
            }
        }
        return number;
    }

    unsigned long calcPrimes(unsigned long start, unsigned long stop, unsigned long step)
    {
        unsigned long c=0;
        unsigned long p;
        for(p=start; p<=stop; p+=step)
        {
            if (isPrime(p) > 0)
            {
                fprintf(stderr, "%lu\n", p);
                c++;
            }
        }
        return c;
    }


    // ---------------------------------------
    // functions to be called by either thread

    void* thread0func(void *)
    {
        unsigned long c;
        c=calcPrimes(3, 100000, 8); //stepping 8 numbers for 4 cores
        fprintf(stderr, "thread1 found %lu primes.\n\n",c);
        return NULL;
    }


    void* thread1func(void *)
    {
        unsigned long c;
        c=calcPrimes(5, 100000, 8); //starting thread 2 at the next odd
                                    //number jumping 8 spaces  
        fprintf(stderr, "thread2 found %lu primes.\n\n",c);
        return NULL;
    }

    void* thread2func(void *)
    {
        unsigned long c;
        c=calcPrimes(7, 100000, 8); //starting thread 2 at the next odd
                                    //number and jumping 8 spaces
        fprintf(stderr, "thread3 found %lu primes.\n\n",c);
        return NULL;
    }

    void* thread3func(void *)
    {
        unsigned long c;
        c=calcPrimes(9, 100000, 8); //starting thread 2 at the next odd
                                    //number and jumping 8 spaces
        fprintf(stderr, "thread4 found %lu primes.\n\n",c);
        return NULL;
    }


    // ---------------------------------------
    // main()

    int main()
    {
		unsigned long  timerms;
		  
        printf("Calculate Prime Numbers\n");
        fprintf(stderr, "==================================================\n\n");
        
        timerms=millis(); 
        //create the threads
        pthread_t thread0, thread1, thread2, thread3;
        
        pthread_create(&thread0, NULL, thread0func, NULL);
        pthread_create(&thread1, NULL, thread1func, NULL);
        pthread_create(&thread2, NULL, thread2func, NULL);
        pthread_create(&thread3, NULL, thread3func, NULL);
        
        
        //wait for threads to join before exiting
        pthread_join(thread0, NULL);
        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
        pthread_join(thread3, NULL);
        
        timerms=millis()-timerms;
        
        fprintf(stderr, "runtime= %ld", timerms);

        return 0;
    }
