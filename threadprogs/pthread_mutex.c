#include <pthread.h>      // POSIX pthread multithreading, link flag: -pthread

pthread_mutex_t  mutexBP; // Mutex variable

int main() {

    // *SNIP*   
    pthread_mutex_init (&mutexBP, NULL);   // init a mutex 
    // *SNIP*

    pthread_mutex_destroy(&mutexBP);

    return 0;
}



// Und dann im Quellcode vor/hinter jeweils allen zu sch�tzenden statements:
   pthread_mutex_lock (&mutexBP);       // lock the following variable operations by the mutex
   // (statements)
   pthread_mutex_unlock (&mutexBP);     // release the mutex to write/read by different threads

//



/*
Mutex-Attribute

Was passiert, wenn ein Thread versucht, einen Mutex zu setzen, den er selber schon fr�her gesetzt hat? Was passiert, wenn ein Thread versucht, einen Mutex freizugeben, den ein anderer Thread gesetzt hat? Nun, dies wird vom aktuell g�ltigen Mutex-Attribut bestimmt, welches bei der Initialisierung �bergeben wird. Bei den LinuxThreads gibt es davon nur drei St�ck:
Mutex-Attribut	Ergebnis, wenn ein Thread versucht, einen von ihm gesetzten Mutex abermals zu setzen.
PTHREAD_MUTEX_FAST_NP	Der Thread blockiert sich selber und bleibt h�ngen, bis der Proze� beendet wird. Dies ist die Default-Einstellung, die ein Mutex bekommt, wenn man bei seiner Initialisierung f�r das Attribut-Argumente NULL �bergibt.
PTHREAD_MUTEX_ERRORCHECK_NP	pthread_mutex_lock gibt einen Fehler zur�ck, der Thread l�uft aber weiter.
PTHREAD_MUTEX_RECURSIVE_NP	Der Thread setzt den Mutex "abermals", d.h. der Mutex mu� jetzt mehrmals wieder freigegeben werden, bevor andere Threads nicht mehr blockiert werden. Dies ist manchmal ganz praktisch, wenn man mehrere Funktionen hat, die auf globale Variablen zugreifen, und die sich gegenseitig aufrufen.
Mutex-Attribut	Ergebnis, wenn ein Thread versucht, einen von einem anderen Thread gesetzten Mutex freizugeben.
PTHREAD_MUTEX_FAST_NP	Der Mutex wird freigegeben.
PTHREAD_MUTEX_ERRORCHECK_NP	pthread_mutex_unlock gibt eine Fehlermeldung zur�ck. Der Zustand des Mutex bleibt unver�ndert.
PTHREAD_MUTEX_RECURSIVE_NP	Der Mutex wird freigegeben.

Mutex-Attribute setzen und auslesen kann man mit dem mehr oder weniger selbsterkl�renden Befehlen

int pthread_mutexattr_init (pthread_mutexattr_t *attr);
int pthread_mutexattr_setkind_np (pthread_mutexattr_t *attr, int kind);
int pthread_mutexattr_getkind_np (const pthread_mutexattr_t *attr, int *kind);

wobei die Integer-Variable kind einen der drei obigen Werte hat, und der �bergebene Zeiger attr auf einen allokierten Speicherbereich zeigen mu�.
*/