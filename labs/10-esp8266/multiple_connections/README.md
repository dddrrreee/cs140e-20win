Simple programs to ping pong back and forth.

Our AT firmware version:

    AT version:1.1.0.0(May 11 2016 18:09:56) 
    SDK version:1.5.4(baaeaebb) 
    compile time:May 20 2016 15:08:19 

## Code to communicate with an ESP8266 using AT commands.

The model:
  1. Issue a command and its expected output.
  2. Wait until the ESP echo's the literal command and then check that we got the
     output that we expected.
  3. When you have multiple connections, you can receive unexpected input, so step
     (2) should call out to auxilary routines to handle when this occurs.

     NOTE: these callouts create a "hold-and-wait" situation (one of the conditions
     needed for deadlock) where they cannot return to handle the current message
     until the callout completes.    Thus, if the callout also invokes the ESP
     it can deadlock if it gets a response to the first request since it can't
     return.  One solution is to just enqueue an event in the callout for later
     processing.  Or use threads. 

The two annoying parsing complications:
  1. The characters from the esp can trickle in at an arbitrary rate --- so we 
     can't assume we can get everything with a single `read` call.  
  2. We don't always know what we are matching against, so need a way to undo
     the input consumed from a failed match..

This is why we use the lexing code (`esp-lex.c`).  It stores characters
in a circular buffer as it receives them; if a match needs more iput,
it calls a client supplised routine to grab more.  Rollback is a matter
of just changing the tail pointer.


In order to allow this code to be run unaltered on both Unix and on our pi we do
the following:
   1. Our debugging and panic macros use the same names (as always).
   2. The input and output routines are supplied by the client --- these will be 
      different on Unix and pi, but have the same function signatures, so the 
      ESP code does not need to change.

Finally, the ESP code (or hardware) seems reasonably flakey and while the
`AT` commands have a document describing them, ESP behavior sometimes
isn't well specified.  As a result, you can get unexpected crashes
(fortunately the ESP output seems the same when this occurs so you can
string match it) and our examples for how to do some things are pulled
from blog posts and expriments.
