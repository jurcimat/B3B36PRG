
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>

#include <unistd.h>
#include "prg_serial_nonblock.h"

void* keyboard_reader(void *v);
void* printer(void *v);
void* serial_read(void *v); 
void* period_counter(void *v);

void set_raw(_Bool set)
{
   if (set) {
      if (system("stty raw -echo") == -1){
      	printf("Terminal is not going to switch to RAW MODE!\n");
      }  // enable raw, disable echo
   } else {
      if (system("stty -raw echo") == -1){
      	printf("Terminal is not going to switch back!\n");
      } // disable raw, enable echo
   }
}

typedef struct {
	char* button;
	char last_recieved;
	char last_sent;
	int counter;
	int period;
	char pressed_char;
	_Bool quit;

} data_set;

data_set data;

pthread_mutex_t mtx;
pthread_cond_t condvar;
int fd;
   
int main(int argc, char *argv[])
{
   int ret = 0;
   const char *serial = argc > 1 ? argv[1] : "/dev/ttyACM0"; //switch /ttyACM0 if it is not working
   fd = serial_open(serial);
   if (fd != -1) { // read from serial port
   		set_raw(true); // set the raw mode
   		pthread_mutex_init(&mtx, NULL);
   		pthread_cond_init(&condvar, NULL);
   		data.counter = 0;
   		data.period = 0;
   		data.quit = false;
   		data.last_sent = '?';
   		data.last_recieved = '?';
   		data.button = "OFF";
   		pthread_t thrs[4];
   		pthread_create(&thrs[0], NULL, keyboard_reader, NULL);
   		pthread_create(&thrs[1], NULL, printer, NULL);
   		pthread_create(&thrs[2], NULL, serial_read, NULL);
   		pthread_create(&thrs[3], NULL, period_counter, NULL);
   		for (int i = 0; i < 3; ++i) {
   			pthread_join(thrs[i], NULL);
   		}
   serial_close(fd);
   set_raw(false);
   printf("\n");
   } else {
   		fprintf(stderr, "ERROR: Cannot open device %s\n", serial);
   }
   return ret;
}

void* keyboard_reader(void *v)
{
   while(1){
   	pthread_mutex_lock(&mtx);
   	if (data.quit == true){
   		pthread_mutex_unlock(&mtx);
   		return 0;
   	}
   	char c;
	pthread_mutex_unlock(&mtx);
    int res = serial_getc_timeout(0 , 1000, &c);
    if ((res != 0) && ( c == 'i' || c == 's' || c == 'e' || c == 'h' || c == 'b' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == 'q')){
    	
    	pthread_mutex_lock(&mtx);
    	if (c == 'q'){
    		data.quit = true;
    	}
        data.last_sent = c;
        data.last_recieved = '?';
        serial_putc(fd, c);
    	pthread_cond_signal(&condvar);
        pthread_mutex_unlock(&mtx);
   } 
  }
}

void* printer(void *v) 
{
   //printf("\r------START-OF-PRINTER-------\n");
   while(1){
    pthread_mutex_lock(&mtx);
   // printf("\rPRINTER IS LOCKED\n");
   	if (data.quit == true){
   		pthread_mutex_unlock(&mtx);
   		//printf("\rPRINTER IS UNLOCKED\n");
   	//	printf("\r-------END-OF-PRINTER-----------\n");
   		return 0;
   	}
   	//printf("\rTHREAD 2 IS WAITING FOR COND\n");
   	pthread_cond_wait(&condvar, &mtx);
   	//printf("\rPRINTER RECIEVED CONDVAR 2\n");
   	printf("\rLED %3s send: '%c' received: '%c', T = %4d ms, ticker = %4d", data.button, data.last_sent, data.last_recieved, data.period, data.counter);
    fflush(stdout);
   	pthread_mutex_unlock(&mtx);
   	//printf("\rPRINTER IS UNLOCKED\n");
  }
}

void* serial_read(void *v) 
{
   char c; 
   while (1) {
      
	  pthread_mutex_lock(&mtx);
	  if (data.quit == true){
	  	pthread_mutex_unlock(&mtx);
	  	return 0;
	  }
      pthread_mutex_unlock(&mtx);
      int res = serial_getc_timeout(fd, 1000, &c);
        if ((res != -1) && ( c == 'i' || c == 'a' || c == 'b' || c == 'h' || c == 'x' || c == 'o' )){
            pthread_mutex_lock(&mtx);
            switch(c)
            {
                case 'b':
                    data.quit = true;
                    data.last_recieved = 'b';
                    break;
                case 'a':
                    if (data.last_sent == 'e')
                    {
                        data.last_recieved = 'a';
                        data.button = "off";
                    }
                    else if (data.last_sent == 's')
                    {
                        data.last_recieved = 'a';
                        data.button = "on";
                    }
                    break;
                case 'h':
                    data.last_recieved = 'h';
                    break;
                case 'i':
                    data.last_recieved = 'i';
                    break;
                case 'x':
                    data.last_recieved = 'x';
                    data.counter++;
                    break;
                case 'o':
                    data.last_recieved = 'o';
                    break;

            }
            pthread_cond_signal(&condvar);
            pthread_mutex_unlock(&mtx);
        }
   }
}

void* period_counter(void *v) 
{
   while(1){
    pthread_mutex_lock(&mtx);
   	if (data.quit == true){
   		pthread_mutex_unlock(&mtx);
   		return 0;
   	}
   	pthread_mutex_unlock(&mtx);
   	usleep(5000000);
   	pthread_mutex_lock(&mtx);
   	data.period = (data.counter != 0) ? 2500 / data.counter : 0;
   	data.counter = 0;
   	pthread_cond_wait(&condvar, &mtx);
   	pthread_mutex_unlock(&mtx);
  }
}

