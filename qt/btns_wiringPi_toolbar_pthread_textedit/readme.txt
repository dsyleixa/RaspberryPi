/*
* GPIO setup (BCM numbering):
* 23: Output (green LED + resistor) // switchable by widget buttons)
* 24: Input  (default: GPIO24->switch->GND)
* 25: Output (red LED + resistor)  // LED is blinking while program runs
*
* Input 24 can be switched from INPUT_PULLUP (default) to INPUT_PULLDOWN
* (beneath toolbar option Edit)
* when  INPUT_PULLDOWN  is activated, the switch has to be re-wired
* (GPIO24->switch->+3v3)
*
* ui->plainTextEdit1->setMaximumBlockCount(2000); // set in form editor
*
*/
