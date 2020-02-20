Go into `example-pi-programs` and make sure you can compile and run
    my-install example-pi-programs/0-reboot
    my-install example-pi-programs/2-blink

Then make sure that making at this level and sending the fake `2-blink.fake`
works (this uses our stuff).

Then comment out `gpio.c` in the Makefile and start building your fake versions.
    
Then work through the rest of the programs, sort of ordered by difficulty:

    example-pi-programs/1-put32.c   
    example-pi-programs/2-blink.c    
    example-pi-programs/3-bug.c  
    example-pi-programs/4-bug.c
    example-pi-programs/5-bug.c
