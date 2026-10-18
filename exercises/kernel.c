
// kmain is a convention, but can't compile on its own
// because needs main()
// not sure why this is required


void kmain(void) {


    // - saysvolatile means the compiler doesn't remove this declaration
    // cuz it's not used in our code
    // - unsigned cuz only positive
    // - short is 2 bytes (vs int 4 bytes)

    // The cast says "treat this number as an address." C won't put a raw
    //  number into a pointer without it.

    volatile unsigned short *cell =  (volatile unsigned short *)0xB8000;

    *cell = 0x024B;

}
