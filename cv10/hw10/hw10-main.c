#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <assert.h>

#include <termios.h> 
#include <unistd.h>  // for STDIN_FILENO

#include <pthread.h>

#include "prg_serial_nonblock.h"
#include "messages.h"
#include "event_queue.h"

#define SERIAL_READ_TIMOUT_MS 500 //timeout for reading from serial port

// shared data structure
typedef struct {
   bool quit;
   int fd; // serial port file descriptor
} data_t;

pthread_mutex_t mtx;
pthread_cond_t cond;

void call_termios(int reset);

void* input_thread(void*);
void* serial_rx_thread(void*); // serial receive buffer

bool send_message(data_t *data, message *msg);

// - MAIN ----------------------------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
int main(int argc, char *argv[])
{
   data_t data = { .quit = false, .fd = -1};
   const char *serial = argc > 1 ? argv[1] : "/dev/ttyACM0";
   data.fd = serial_open(serial);

   if (data.fd == -1) {
      fprintf(stderr, "ERROR: Cannot open serial port %s\n", serial);
      exit(100);
   }

   enum { INPUT, SERIAL_RX, NUM_THREADS };
   const char *threads_names[] = { "Input", "Serial In" };

   void* (*thr_functions[])(void*) = { input_thread, serial_rx_thread};

   pthread_t threads[NUM_THREADS];
   pthread_mutex_init(&mtx, NULL); // initialize mutex with default attributes
   pthread_cond_init(&cond, NULL); // initialize mutex with default attributes

   call_termios(0);

   for (int i = 0; i < NUM_THREADS; ++i) {
      int r = pthread_create(&threads[i], NULL, thr_functions[i], &data);
      fprintf(stderr, "INFO: Create thread '%s' %s\n", threads_names[i], ( r == 0 ? "OK" : "FAIL") );
   }

   // example of local variables for computation and messaging
   
   struct {
      uint16_t chunk_id;
      uint16_t nbr_tasks;
      uint16_t task_id;
      bool computing;
   } computation = { 0, 0, 0, false };
     
   message msg;
   
   bool quit = false;
   while (!quit) {
      // example of the event queue
      event ev = queue_pop();
      if (ev.source == EV_KEYBOARD) {
         msg.type = MSG_NBR;
         // handle keyboard events
         switch(ev.type) {
            case EV_GET_VERSION:
               { // prepare packet for get version
                  msg.type = MSG_GET_VERSION;
                  fprintf(stderr, "INFO: Get version requested\n");
               }
               break;
 			           
            case EV_COMPUTE:
            	{
            		if (computation.computing){
            			fprintf(stderr, "WARN: New computation requested but it is discarded due on ongoing computation\n\r");
            		}else {
            			computation.computing = true;
            			computation.nbr_tasks = ev.data.param;
            			
            			msg.type = MSG_COMPUTE;
            			msg.data.compute.chunk_id = computation.chunk_id;
            			msg.data.compute.nbr_tasks = ev.data.param;
            			fprintf(stderr, "INFO: New computation chunk id: %d no. of tasks: %d\n\r", msg.data.compute.chunk_id, msg.data.compute.nbr_tasks);
            			computation.chunk_id++;
            		}
            	}
            	break;
            case EV_RESET_CHUNK:
            	{
            	    if (computation.computing){
						fprintf(stderr, "WARN: Chunk reset request discarded, it is currently computing\n\r");
                  	}else {
                    	computation.chunk_id  = 0;
                    	fprintf(stderr, "INFO: Chunk reset request\n\r");                  	
                  	}
            	}
            	break;
            case MSG_ABORT:
            	{
            		if (!computation.computing){
            			fprintf(stderr, "WARN: Abort requested but it is not computing\n\r");
            		} else {
            			msg.type = MSG_ABORT;
            			computation.computing = false;            			
            		}
            	}
				break;              
            default:
               break;
         }
         if (msg.type != MSG_NBR) { // message has been set
            if (!send_message(&data, &msg)) {
               fprintf(stderr, "ERROR: send_message() does not send all bytes of the message!\n");
            }
         }
      } else if (ev.source == EV_NUCLEO) { // handle nucleo events
         if (ev.type == EV_SERIAL) {
            message *msg = ev.data.msg;
            switch (msg->type) {
               case MSG_STARTUP:
                  {
                     char str[STARTUP_MSG_LEN+1];
                     for (int i = 0; i < STARTUP_MSG_LEN; ++i) {
                        str[i] = msg->data.startup.message[i];
                     }
                     str[STARTUP_MSG_LEN] = '\0';
                     fprintf(stderr, "INFO: Nucleo restarted - '%s'\n", str);
                  }
                  break;
               case MSG_VERSION:
                  if (msg->data.version.patch > 0) {
                     fprintf(stderr, "INFO: Nucleo firmware ver. %d.%d-p%d\n", msg->data.version.major, msg->data.version.minor, msg->data.version.patch);
                  } else {
                     fprintf(stderr, "INFO: Nucleo firmware ver. %d.%d\n", msg->data.version.major, msg->data.version.minor);
                  }
                  break;
                  
               case MSG_OK:
                  {
                  	fprintf(stderr, "INFO: Receive ok from Nucleo\r\n");                    
                  }
                  break;
               case MSG_ERROR:
               	  {
               	  	fprintf(stderr, "WARN: Receive error from Nucleo\r\n");               	  
               	  }
               	  break;
               case MSG_ABORT:
                  {
                  	fprintf(stderr, "INFO: Abort from Nucleo\r\n");
                  	if (!computation.computing){
                  		fprintf(stderr, "WARN: Abort requested but it is not computing\n\r");
                  	}else {
                  		computation.computing = false;                	
                  	}
                  }
                  break;
               case MSG_COMPUTE_DATA:
               	  {
               	  	if (computation.computing){
               	  		fprintf(stderr, "INFO: New data chunk id: %d, task id: %d - results %d\r\n", msg->data.compute_data.chunk_id, msg->data.compute_data.task_id, msg->data.compute_data.result);
               	  	} else {
               	  		fprintf(stderr, "WARN: Nucleo sends new data without computing \r\n");
               	  	}
               	  }   
                  break;
               case MSG_DONE:
               	  {
               	  	fprintf(stderr, "INFO: Nucleo reports the computation is done computing: %d\r\n", computation.computing);
               	  	computation.computing = false;
               	  }
               	  break;                  
               default:
                  break;
            }
            if (msg) {
               free(msg);
            }
         } else if (ev.type == EV_QUIT) {
            quit = true;
         } else {
            // ignore all other events
         }
      }

   } // end main quit
   queue_cleanup(); // cleanup all events and free allocated memory for messages.
   for (int i = 0; i < NUM_THREADS; ++i) {
      fprintf(stderr, "INFO: Call join to the thread %s\n", threads_names[i]);
      int r = pthread_join(threads[i], NULL);
      fprintf(stderr, "INFO: Joining the thread %s has been %s\n", threads_names[i], (r == 0 ? "OK" : "FAIL"));
   }
   serial_close(data.fd);
   call_termios(1); // restore terminal settings
   return EXIT_SUCCESS;
}
//---------------------------------END OF MAIN-----------------------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

// - function CALL TERMIOS -----------------------------------------------------------------
void call_termios(int reset)
{
   static struct termios tio, tioOld;
   tcgetattr(STDIN_FILENO, &tio);
   if (reset) {
      tcsetattr(STDIN_FILENO, TCSANOW, &tioOld);
   } else {
      tioOld = tio; //backup 
      cfmakeraw(&tio);
      tio.c_oflag |= OPOST;
      tcsetattr(STDIN_FILENO, TCSANOW, &tio);
   }
}

// - function INPUT THREAD------------------------------------------------------------------
void* input_thread(void* d)
{
   data_t *data = (data_t*)d;
   bool end = false;
   int c;
   event ev = { .source = EV_KEYBOARD };
   while ( !end && (c = getchar())) {
      ev.type = EV_TYPE_NUM;
      switch(c) {
         case 'g': // get version
            ev.type = EV_GET_VERSION;
            break;
         case 'q':
            end = true;
            break;
         case '1':
         	ev.type = EV_COMPUTE;
         	ev.data.param = 10;
         	break;
         case '2':
         	ev.type = EV_COMPUTE;
         	ev.data.param = 20;
         	break;
         case '3':
         	ev.type = EV_COMPUTE;
         	ev.data.param = 30;
         	break;
         case '4':
         	ev.type = EV_COMPUTE;
         	ev.data.param = 40;
         	break;
         case '5':
         	ev.type = EV_COMPUTE;
         	ev.data.param = 50;
         	break;
         case 'a':
         	ev.type = EV_ABORT;
         	break;
         case 'r':
         	ev.type = EV_RESET_CHUNK;
         	break;
         default: // discard all other keys
            break;
      }
      if (ev.type != EV_TYPE_NUM) { // new event 
      	queue_push(ev);
      }
      pthread_mutex_lock(&mtx);
      end = end || data->quit; // check for quit
      data->quit = end;
      pthread_mutex_unlock(&mtx);
   }
   ev.type = EV_QUIT;
   queue_push(ev);
   fprintf(stderr, "INFO: Exit input thread %p\n", (void*)pthread_self());
   return NULL;
}

// - function RX THREAD -> FOR READING FROM SERIAL------------------------------------------------
void* serial_rx_thread(void* d) 
{ // read bytes from the serial and puts the parsed message to the queue
   data_t *data = (data_t*)d;
   uint8_t msg_buf[sizeof(message)]; // maximal buffer for all possible messages defined in messages.h
   event ev = { .source = EV_NUCLEO, .type = EV_SERIAL, .data.msg = NULL };
   bool end = false;
   unsigned char c;
   //my variables
   bool reading_process = false;
   int size = 0;
   int count = 0;
   message *msg;
      
   while (serial_getc_timeout(data->fd, SERIAL_READ_TIMOUT_MS, &c) > 0) {}; // discard garbage

   while (!end) {
      int r = serial_getc_timeout(data->fd, SERIAL_READ_TIMOUT_MS, &c);
      if (r > 0) { // character has been read
         // TODO you may put your code here
      	if (reading_process){
        	msg_buf[count++] = (uint8_t)c;
        	if (count - size == 0){
        		msg = (message*)malloc(sizeof(message));
        		if (parse_message_buf(msg_buf, size, msg)){
        			ev.data.msg = msg;
        			count = 0;
        			reading_process = false;
        			queue_push(ev);
        		}else{
        			fprintf(stderr, "ERROR: Cannot parse message buffer, message type %d\n\r",msg_buf[0]);
        		}
        	
        	}
         
        }else if (!reading_process){
        	bool check = !get_message_size((uint8_t)c, &size);
        	if (check){
        		fprintf(stderr, "ERROR: Unknown message type has been received 0x%x\n - '%c'\r", c, c);
        	}
        	reading_process = true;
        	msg_buf[count++] = (uint8_t)c;
        }         
         
         
         
      } else if (r == 0) { //read but nothing has been received
      	// TODO you may put your code here
      	if (reading_process){
      		fprintf(stderr,"ERROR: Something was read but nothing has been recieved!!!!\n");      	
      	}        
      } else {
         fprintf(stderr, "ERROR: Cannot receive data from the serial port\n");
         end = true;
      }
      pthread_mutex_lock(&mtx);
      end = end || data->quit;
      pthread_mutex_unlock(&mtx);
   }
   ev.type = EV_QUIT;
   queue_push(ev);
   fprintf(stderr, "INFO: Exit serial_rx_thread %p\n", (void*)pthread_self());
   return NULL;
}

// - function SEND MESSAGE-----------------------------------------------------------------
bool send_message(data_t *data, message *msg) 
{
   int message_size = sizeof(message);
   uint8_t msg_buf[message_size];
   int size;
   if (fill_message_buf(msg, msg_buf, message_size, &size)){
   		for (int iter = 0; iter < size; iter++){
   			serial_putc(data->fd, msg_buf[iter]);
   		} 
   		return true;
   }   
   
   return false;
}

/* end of hw10-main.c */
