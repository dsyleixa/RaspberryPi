    #include <stdio.h>
    #include <pthread.h>
    #include <wiringPi.h>

    typedef struct
    {
        int start;
        int end;
        int step;
    }data;

    int isPrime(long int number)
    {
        long int i;
        for(i=2; i<number; i++)
        {
            if(number % i == 0)
            {
                //not a prime
                return 0;
            }
        }
        return number;
    }

    int calcPrimes(int start, int stop, int step)
    {
        int c=0;
        long int s;
        for(s=start; s<=stop; s+=step)
        {
            if (isPrime(s) > 0)
            {
                c++;
            }
        }
        return c;
    }

    void* thread1Go(void *)
    {
        int c;
        c=calcPrimes(3, 100000, 8); //stepping 8 numbers for 4 cores
        printf("thread1 found %d primes.\n",c);
        return NULL;
    }


    void* thread2Go(void *)
    {
        int c;
        c=calcPrimes(5, 100000, 8); //starting thread 2 at the next odd
                                    //number jumping 8 spaces for 4 cores
        printf("thread2 found %d primes.\n",c);
        return NULL;
    }

    void* thread3Go(void *)
    {
        int c;
        c=calcPrimes(7, 100000, 8); //starting thread 2 at the next odd
                                    //number and jumping 8 spaces
        printf("thread3 found %d primes.\n",c);
        return NULL;
    }

    void* thread4Go(void *)
    {
        int c;
        c=calcPrimes(9, 100000, 8); //starting thread 2 at the next odd
                                    //number and jumping 8 spaces
        printf("thread3 found %d primes.\n",c);
        return NULL;
    }

   
    int main()
    {
		unsigned long  timerms;
		  
        printf("Calculate Prime Numbers\n");
        printf("==================================================\n\n");
        
        timerms=millis(); 
        //create the threads
        pthread_t thread0, thread1, thread2, thread3;
        
        pthread_create(&thread0, NULL, thread1Go, NULL);
        pthread_create(&thread1, NULL, thread2Go, NULL);
        pthread_create(&thread2, NULL, thread3Go, NULL);
        pthread_create(&thread3, NULL, thread4Go, NULL);
        
        
        //wait for threads to join before exiting
        pthread_join(thread0, NULL);
        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
        pthread_join(thread3, NULL);
        
        timerms=millis()-timerms;
        
        printf("runtime= %ld", timerms);

        return 0;
    }
