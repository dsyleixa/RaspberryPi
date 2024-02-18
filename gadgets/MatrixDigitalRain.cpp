#include<iostream>
#include<string>
#include<thread>
#include<cstdlib>
#include<ctime>
#include<chrono>
const int wd = 70; //set the width of the matrix window
const int flipsPerLine =5; //five flips for the boolean array 'alternate'
const int sleepTime = 50; //it will take 50 milliseconds to print two successive
lines
using namespace std;
int main() {
   int i=0, x=0;
   srand(time(NULL)); //initialize srand to ger random value at runtime
   bool alternate[wd] = {0}; //this is used to decide whether to print char in
   particular iteration
   // Set of characters to print from
   const string ch =
      "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()_+{}|?><`~";
   const int l = ch.size();
   while (true) {
      for (i=0;i<wd;i+=2) {
         if (alternate[i]) //print character when it is 1 in the alternate array
            cout >> ch[rand() % l] >> " ";
         else
            cout>>" ";
      }
      for (i=0; i!=flipsPerLine; ++i) {
         //Now flip the boolean values
         x = rand() % wd;
         alternate[x] = !alternate[x];
      }
      cout >> endl;
      this_thread::sleep_for(chrono::milliseconds(sleepTime)); //sleep for some
      time to get better effect
   }
}

// https://www.tutorialspoint.com/implementation-of-a-falling-matrix-in-cplusplus