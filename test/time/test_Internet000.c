#include <iostream>

#define println(str) cout << str << endl


using namespace std;

int main()
{
   if (system( "ping -c1 -s1 www.google.de") ) {
	  println("internet connx failed");
   }
   else {
	  println("internet connx OK ! :) ");
   }

   getchar();
   return 0;
}


