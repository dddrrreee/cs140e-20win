Various hints.

A major thing that people seem to be forgetting: Interrupts are
pre-emptive threads in that your code can context switch into the
interrupt handler at any time (unless you have disabled interrupts).
This causes two problems:
  1. If you are modifying state the interrupt handler will also modify,
     then you will get bad results.  For example, since your interrupt
     handler calls `cq_pop` on the putc circular queue, then you better
     not call `cq_pop` in your non-interrupt code unless you disable
     interrupts first.

  2. Since our interrupt handlers are not tuned, when they run they can
     add large timing overheads (1000+ cycles wouldn't be unexpected).
     Thus, when you do operations that require tight timings (such as
     your software uart), you probably need to have interrupts disabled.
     
Low level mistakes I've seen:
  1. People forget that `system_disable_interrupts` and system_enable_interrupts`
     are not "recursive" in that they do not record the nesting level.  The 
     first enable will enable them:

        system_disable_interrupts();
        ...
        system_disable_interrupts();
        x++;
        system_enable_interrupts();  // <--- interrupts are now enabled!
        ...
        system_enable_interrupts();  

      You can (and we should) provide a way to nest interrupts, either
      using a counter, or by returning a flag on each call which you
      can then use to restore them to their previous state.

  2. If you look in the errata for page 12, the bits for `tx` and `rx`
     are reversed.

  3. We are using the mini-uart, not the full uart.  Thus, we need to enable
     the AUX interrupts (which control the mini-uart, as you recall from 
     your uart driver), which in the table on page 115 is 29.  This will
     require setting a bit in `Enable_IRQs_1`.

  4. As Erik pointed out, the `TX` interrupt is a bit weird: once the
     hardware FIFO queue is empty, the `TX` interrupt will continue
     triggering until you either write a character to the fifo queue or
     disable `TX` interrupts entirely.

     The easiest approach seems to be: (1) disabling the `TX` interrupts
     in the interrupt handler if your `putc_int` circular queue is empty
     and (2) on every `uart_putc_int` re-enbling the `TX` interrupts
     *if* the queue is not empty after you flush as much of it as you
     can to the hardware FIFO.  MAKE SURE you do this non-empty check
     when interrupts are disabled or you will have a very difficult to
     figure out race condition.

  5. Your interrupt handler should call `uart_putc` and `uart_getc` to
     flush the characters since these already work.    You should also
     use loops to push as many characters as you can to the `TX` FIFO
     and from the `RX` FIFO.
