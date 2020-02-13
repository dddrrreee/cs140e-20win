software uart binary.
   1. you have to do an update and a make clean in `libpi`.
   2. it will transmit on pin 20 and receive on pin 21.
   3. it communicates with the pi program that it runs using the 
      process control block stuff in `libpi/cs140e-src`
