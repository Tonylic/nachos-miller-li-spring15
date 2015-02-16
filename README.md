# nachos-miller-li-spring15
4. (a)Three things may cause time to advance:
1). interrupts are turned on or off
2). a user program instruction is executed
3). when nothing is on the ready queue.

(b) Thread should execute its constructor before the routine passed in to Fork()

(c) To perform a context-swicth, all registers in the old thread has to be saved. After that, new values would be loaded into registers from the context block of the next thread. In Nachos, the routine SWITCH would complete this work.

Switching CPU from one thread to another causes the context switch. Specifically, When invoking methods Thread::Yield and Thread::Sleep, if other thread is waiting, then context-switch would occur. 

The state would be stored in the thread context block. In Nachos impelementation, Thread::stackTop and Thread::machineState store stack pointer and other registers respectively. 
