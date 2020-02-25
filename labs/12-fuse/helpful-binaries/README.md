Binaries to help testing:
   1. `hello.bin` simple hello.
   2. `kernel.img` pi-side bootloader code.

When I:
    cp 5-replay/code/trace ~/bin/pi-trace
    rehash
    pi-trace my-install ./hello.bin
    cksum log.interpose.txt 
    3373930713 21574 log.interpose.txt
