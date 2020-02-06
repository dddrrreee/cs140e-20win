## GPIO interrupt code.

Implement the various routines in `gpio-int.c`.

You should run the two fake programs:
  1. `simple-gpio-test`.   My log for this is in `simple-gpio-test.log`

  2. `full-gpio-test`.  My log for this is in `full-gpio-test.log`.

I used memory barriers around each different device (the interrupt
controller and the GPIO code).  If you disagree (and some of you will!)
let me know!

When you are done, either put your code with the rest in `libpi/gpio.c` or
copy it to `libpi/my-src`.
