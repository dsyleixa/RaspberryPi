#include <iostream>
using namespace std;
#include <sys/time.h>

int main()
{
  timeval start, end;
  gettimeofday(&start, 0);
  //...
  gettimeofday(&end, 0);
  cout << start.tv_sec << ':' << start.tv_usec << endl;
  cout << end.tv_sec << ':' << end.tv_usec << endl;
}