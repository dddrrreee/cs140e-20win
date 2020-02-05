### cycle-based software uart code.

This is hopefully fast.  We need this code to talk to the ESP8266 wireless device.
   1. Implement the four unimplemented software UART routines in `sw-uart.c`.
   2. There are four tests ordered from easiest to hardest.  
      You run them using `pi-cat` and entering characters and a newline.

If I have time I will implement the Unix-side support so we can test these in 
the fake environment and cross-check.
