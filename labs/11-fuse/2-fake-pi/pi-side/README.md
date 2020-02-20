Trivial little virtual machine loop on the pi that gets command from the unix side,
executes them and then (if needed) replies and does the next one.  You can 
control most things on the pi using the PUT32 and GET32 messages, other than the 
UART itself (which we are communicating over).
