#include <sys/timex.h>
#include <iostream>

#define println(str) cout << str << endl


using namespace std;

int main()
{
    ntptimeval dummy;

    int result = ntp_gettime(&dummy); 

    if (result == TIME_OK) {
        println("internet time ok");
    }
    else if (result == TIME_ERROR) {
        println("internet time error");
    }
    else {
        println("different time issue");
    }

    return 0;
}


