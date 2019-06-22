// first OpenVG program
// Anthony Starks (ajstarks@gmail.com)
// Adapted for paeryn's fork by paeryn
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
//#include "shapes.h"

int main() {
    int width, height;
    char s[3];
 
    // Request a window size of 600x360 with top-left at 20,20
    initWindowSize(20, 20, 600, 360);
    init(&width, &height);

    Start(width, height);                   // Start the picture
    Background(0, 0, 0);                    // Black background
    Fill(44, 77, 232, 1);                   // Big blue marble
    Circle(width / 2, 0, width);            // The "world"

    Fill(255, 255, 255, 1);                 // White text
    TextMid(width / 2, height / 2, "hello, world", SerifTypeface, width / 10);  // Greetings
    End();                                  // End the picture
    WindowOpacity(128); // Make the window half opacity
                // Can now see what is behind it   
    fgets(s, 2, stdin);                     // look at the pic, end with [RETURN]
    End();                               // finish, Graphics cleanup
    exit(0);
}
