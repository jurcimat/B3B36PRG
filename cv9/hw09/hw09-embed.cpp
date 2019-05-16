#include "mbed.h"
 
Serial serial(SERIAL_TX, SERIAL_RX);
 
DigitalOut myled(LED1);
Ticker ticker;
 
void tick()
{
    myled = !myled;
    (myled == 1) ? serial.putc('x') : serial.putc('o');

} 

int main()
{
    serial.baud(115200);
    for (int i = 0; i < 5*2; ++i) { // 5x zablikání LED s periodou 50 ms
        myled = !myled;
        wait(0.05);
    }
    serial.putc('i');
    float periods[] = { 0.05, 0.1, 0.2, 0.5, 1.0 };
    myled = 0;
    while (1) {
        while(!serial.readable()) {
            //aktivní čekání na přijetí bytu
        }
        int c = serial.getc();
        switch(c) {
            case 's':
                myled = 1;
                ticker.detach();
                serial.putc('a');
                break;
            case 'e':
                myled = 0;
                ticker.detach();
                serial.putc('a');
                break;
            case 'h':
                serial.putc('h');
                break;
            case 'b':
                ticker.detach();
                serial.putc('b');
                exit(0);
            default:
                if (c >= '1' && c <= '5') {
                    ticker.attach(tick, periods[c-'1']);
                    serial.putc('a');
                    }
                break;
        }
        while (!serial.writeable()) {}
    }
}
