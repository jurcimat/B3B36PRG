#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <assert.h>

#include <termios.h> 
#include <unistd.h>  // for STDIN_FILENO

#include <pthread.h>
#include <math.h> // using for float absolute value
 
#include "prg_serial_nonblock.h"
#include "messages.h"
#include "event_queue.h"
#include "window.h"


#define SERIAL_READ_TIMOUT_MS 500 //timeout for reading from serial port

// Predefined constants for creating deafult picture
#define WIDTH 320
#define HEIGHT 240
#define DEF_N 60
#define START_R -0.5
#define START_I -0.5
#define END_R 0.5
#define END_I 0.5
#define C_R -0.4
#define C_I 0.6
#define CHUNK_SIZE_RE 160
#define CHUNK_SIZE_IM 2

// data structure used for storing values of computation
typedef struct {
	uint8_t cid;            // chunk_id
	uint8_t n;              // number of iterations
	uint8_t chunk_size_re;     // size of chunk
	uint8_t chunk_size_im;
	double real_iterative;
	double imag_iterative;  // current complex number
	double real_start;          
	double imag_start;      // start point complex number
	double real_end;
	double imag_end;		// ending point of complex number
	double real_constant;
	double imag_constant;   // complex constant c
	bool computing;         // true or false whether NUCLEO is computing
	bool enhanced_baud;
}computation;

// shared data structure
typedef struct {
   bool quit;
   int fd; // serial port file descriptor
} data_t;
// Custom made functions
bool send_message(data_t *data, message *msg);
double compute_difference(double start, double end, int pixels);
double compute_start_chunk_re(image* img, computation parameters);
double compute_start_chunk_im(image* img, computation parameters);
image* compute_at_CPU(image* img, computation parameters, bool change_c);
uint8_t compute_fractal(double real_c, double imag_c, double real_input, double imag_input, uint8_t iterations);
image* computeCPUanimation(image* img,computation parameters);





pthread_mutex_t mtx;
pthread_cond_t cond;

void call_termios(int reset);

void* input_thread(void*);
void* serial_rx_thread(void*); // serial receive buffer

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
   
  
   computation parameters = { .cid = -1, .n = DEF_N, .real_start = START_R, .imag_start = START_I, .real_end = END_R, .imag_end = END_I,
   			.enhanced_baud = false,	.real_constant = C_R, .imag_constant = C_I, .computing = false, .chunk_size_re = CHUNK_SIZE_RE, .chunk_size_im = CHUNK_SIZE_IM};  
   message msg;
   image* img = window_init(WIDTH, HEIGHT, parameters.chunk_size_re, parameters.chunk_size_im);
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
            		if (parameters.computing){
            			fprintf(stderr, "WARN: New computation requested but it is discarded due to ongoing computation\n\r");
            		}else {
            			parameters.computing = true;
            			parameters.cid++;
            			
            			msg.type = MSG_COMPUTE;
            			msg.data.compute.cid = parameters.cid;
            			                                                                                               
            			msg.data.compute.re = compute_start_chunk_re(img, parameters);
            			msg.data.compute.im = compute_start_chunk_im(img, parameters);
            			msg.data.compute.n_re = parameters.chunk_size_re;
            			msg.data.compute.n_im = parameters.chunk_size_im;
            			
            			fprintf(stderr, "INFO: New computation chunk id: %d with chunk size %d X %d\n\r", msg.data.compute.cid, parameters.chunk_size_re, 																																				parameters.chunk_size_im);
            		}
            	}
            	break;
            
	    	case EV_SHOW_ANIMATION:
	    		if (parameters.computing){
					fprintf(stderr, "WARN: PC is not able to show animation during computation\r\n");
				}else{
					fprintf(stderr, "INFO: Animation has been created\r\n");
					pthread_mutex_lock(&mtx);
					img = computeCPUanimation(img, parameters);
					pthread_mutex_unlock(&mtx);
				}
				break;
            case EV_RESET_CHUNK:
            	{
            	    if (parameters.computing){
						fprintf(stderr, "WARN: Chunk reset request discarded, it is currently computing\n\r");
                  	}else {
                    	parameters.cid  = -1;
                    	fprintf(stderr, "INFO: Chunk reset request accepted\n\r");                  	
                  	}
            	}
            	break;
            case EV_OPTIONS:
            	{
            	    if (parameters.computing){
						fprintf(stderr, "WARN: Options reset discarded due to ongoing computation\n\r");
                  	}else {
						pthread_mutex_lock(&mtx);
						call_termios(1);
      					fprintf(stderr, "INFO: Options reset accepted waiting for parameters from user please press ENTER\n");
      					setbuf(stdin, NULL);
      					fprintf(stderr, "INFO: Please type Re(a)(in double): \n");
      					double xR;
      					scanf("%lf",&xR);
      					fprintf(stderr, "INFO: Please type Im(a)(in double): \n");
      					double aI;
      					scanf("%lf",&aI);
      					fprintf(stderr, "INFO: Please type Re(b)(in double): \n");
      					double bR;
      					scanf("%lf",&bR);
      					fprintf(stderr, "INFO: Please type Im(b)(in double): \n");
      					double bI;
      					scanf("%lf",&bI);
      					fprintf(stderr, "INFO: Please type constant Re(c)(in double): \n");
      					double cR;
      					scanf("%lf",&cR);
      					fprintf(stderr, "INFO: Please type constant Im(c)(in double): \n");
      					double cI;
      					scanf("%lf",&cI);
      					fprintf(stderr, "INFO: Please type iteration limit(in integer): \n");
      					int n;
      					scanf("%d",&n);
      					parameters.cid = -1;
      					parameters.n = (uint8_t)n;
      					parameters.real_start = xR;
      					parameters.imag_start = aI;
      					parameters.real_end = bR;
      					parameters.imag_end = bI;
   						parameters.real_constant = cR;
   						parameters.imag_constant = cI;
   						parameters.computing = false; 
      					call_termios(0);
      					pthread_mutex_unlock(&mtx);           	
                  	}
            	}
            	break;
            case EV_SHOW_PARAMETERS:
            	{
					fprintf(stderr,"INFO: showing parameters\r\n");
            		fprintf(stderr,"cid = %d\r\n",parameters.cid);
            		fprintf(stderr,"n = %d\r\n",parameters.n);
            		fprintf(stderr,"real_start = %lf\r\n",parameters.real_start);
            		fprintf(stderr,"imag_start = %lf\r\n",parameters.imag_start); 
            		fprintf(stderr,"real_end = %lf\r\n",parameters.real_end); 
            		fprintf(stderr,"imag_end = %lf\r\n",parameters.imag_end);
            		fprintf(stderr,"real_constant = %lf\r\n",parameters.real_constant);   
            		fprintf(stderr,"imag_constant = %lf\r\n",parameters.imag_constant);
            		fprintf(stderr,"computing = %d\r\n",parameters.computing);
            		fprintf(stderr,"enhanced_baud = %d\r\n",parameters.enhanced_baud);
            		fprintf(stderr,"chunk_size_re = %d\r\n",parameters.chunk_size_re);
            		fprintf(stderr,"chunk_size_im = %d\r\n",parameters.chunk_size_im);                  			                 			
            		
            	}
				break;
            case MSG_ABORT:
            	{
            		if (!parameters.computing){
            			fprintf(stderr, "WARN: Abort requested but it is not computing\n\r");
            		} else {
            			msg.type = MSG_ABORT;
            			parameters.computing = false;            			
            		}
            	}
				break;
			case EV_SET_COMPUTE:
				{
					if (parameters.computing){
						fprintf(stderr, "WARN: Set computation requested but discarded due to ongoing computation\n\r");
					} else {
						parameters.real_iterative = compute_difference(parameters.real_start, parameters.real_end, img->width);
						parameters.imag_iterative = (-1)*compute_difference(parameters.imag_start, parameters.imag_end, img->height);
						msg.type = MSG_SET_COMPUTE;
						msg.data.set_compute.c_re = parameters.real_constant;
						msg.data.set_compute.c_im = parameters.imag_constant;
						msg.data.set_compute.d_re = parameters.real_iterative;
						msg.data.set_compute.d_im = parameters.imag_iterative;
						msg.data.set_compute.n = parameters.n;
						
						fprintf(stderr, "INFO: Computation parameters were set\n\r");
					}
				}
				break;
			case EV_COMPUTE_CPU:
				{
					if(parameters.computing){
						fprintf(stderr, "INFO: Computation requested on PC was discarded due to ongoing computation on Nucleo\n\r");
					} else {
						fprintf(stderr, "INFO: PC successfully computed image\n\r");
						img = compute_at_CPU(img, parameters, false);
						window_refresh(img);
					}
				}
				break;
			case EV_CLEAR_BUFFER:
	    	
	    		if (parameters.computing){
					fprintf(stderr,"WARN: Clear buffer request discarderd, abort computation at first\n\r");
		
				}else{
					img = window_black_screen(img);
					parameters.cid = -1;
					fprintf(stderr,"INFO: Buffer was cleared, chunk ID was reset\n\r");
				}
				break;
			case EV_ENHANCE_BAUD:
	    		if (parameters.computing){
					fprintf(stderr, "WARN: Nucleo is able to save image only when it is not computing\r\n");
				}
				else{
					if (parameters.enhanced_baud){
						fprintf(stderr, "INFO: Baud rate has been already enhanced\r\n");
					}else{
					msg.type= MSG_ENHANCE_BAUD;
					parameters.enhanced_baud = true;
					fprintf(stderr, "INFO: Baud rate has been enhanced\r\n");
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
            if (msg.type ==MSG_ENHANCE_BAUD){	
				printf ("Data fd: %d\n", data.fd);
				pthread_mutex_lock(&mtx);
				data.fd = serial_open_enhanced("/dev/ttyACM0");
				printf ("Data fd: %d\n", data.fd);
				pthread_mutex_unlock(&mtx);
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
                  	if (!parameters.computing){
                  		fprintf(stderr, "WARN: Abort requested but it is not computing\n\r");
                  	}else {
                  		parameters.computing = false;                	
                  	}
                  }
                  break;
               case MSG_COMPUTE_DATA:
               	  {
               	  	if (parameters.computing){
               	  		if(parameters.cid != msg->data.compute_data.cid){
               	  			fprintf(stderr, "ERROR: Recieved computed data have different CID than requested.\n\r");
               	  		}else {
               	  			int chunks_in_row = img->width/parameters.chunk_size_re;
               	  			int x = msg->data.compute_data.i_re + (msg->data.compute_data.cid % chunks_in_row) * parameters.chunk_size_re;
               	  			int y = msg->data.compute_data.i_im + (msg->data.compute_data.cid / chunks_in_row) * parameters.chunk_size_im;
               	  			//fprintf(stderr, "Currently computing for x:= %d and y:= %d\n\r",x,y);
               	  			if (x < 0 || y < 0 || x > img->width || y > img->height){
               	  				fprintf(stderr, "ERROR: Recieved computed data don't fit into image margins.\n\r");
               	  			} else {
               	  				img = set_pixel_by_iter(img, parameters.cid, x, y, msg->data.compute_data.iter, parameters.n);
               	  				//fprintf(stderr, "inside main - case recieved compute data\r\n");
               	  			}
               	  			
               	  		}
               	  		
               	  		
               	  		
               	  		
               	  	} else {
               	  		fprintf(stderr, "WARN: Nucleo sends new data without computing \r\n");
               	  	}
               	  }   
                  break;	
               case MSG_DONE:
               	  {
               	  	if (parameters.computing){
               	    	fprintf(stderr, "INFO: Nucleo send chunk with ID: %d\r\n", parameters.cid);
               	  		parameters.computing = false;
               	  		window_refresh(img);
               	  		
               	  		if (parameters.cid != (img->nbr_of_chunks - 1)){
               	  			//fprintf(stderr, "nbr_of_chunks: %d", img->nbr_of_chunks);
               	  			fprintf(stderr, "INFO: PC sending request for next chunk to Nucleo\r\n");
               	  			event ev = { .source = EV_KEYBOARD, .type = EV_COMPUTE};
               	  			queue_push(ev);
               	  		} else {
               	  			fprintf(stderr, "INFO: Nucleo computed last chunk\r\n");
							parameters.computing = false;
							parameters.cid = 0;
               	  		}
               	  	
               	  	}
               	  	fprintf(stderr, "INFO: Nucleo reports the computation is done computing: %d\r\n", parameters.cid);
               	  	parameters.computing = false;
               	  }
               	  break;                  
               default:
                  break;
            }
            if (msg) {
               free(msg);
            }
         } else if (ev.type == EV_QUIT) {
           	if (parameters.enhanced_baud){
				msg.type = MSG_ENHANCE_BAUD;
            	if (!send_message(&data, &msg)) {
               		fprintf(stderr, "ERROR: send_message() does not send all bytes of the message!\n");
            	}		
            }
            quit = true;
            
         } else {
            // ignore all other events
         }
      }

   } // end main quit
   queue_cleanup(); // cleanup all events and free allocated memory for messages.
   free_image(img);
   close_image();
   
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
         case 'a':
         	ev.type = EV_ABORT;
         	break;
         case 'r':
         	ev.type = EV_RESET_CHUNK;
         	break;
         case 'c':
         	ev.type = EV_COMPUTE_CPU; 
         	break;
         case 'm':
         	ev.type = EV_SHOW_ANIMATION; 
         	break;
         case 'b':
         	ev.type = EV_ENHANCE_BAUD; 
         	break;
         case 'l':
         	ev.type = EV_CLEAR_BUFFER; 
         	break;
         case 's':
         	ev.type = EV_SET_COMPUTE; 
         	break;
         case 'o':
         	ev.type = EV_OPTIONS; 
         	break;
         case 'i':
         	ev.type = EV_SHOW_PARAMETERS; 
         	break;
         case '\r':
         	ev.type = EV_COMPUTE; 
         	//ev.data.param = c - '0';
         	break;
         case 'h':
         	fprintf(stderr, "INFO: User manual \r\n");
         	fprintf(stderr, "----- KEY 'a' = Abort computation as command from PC\r\n");
         	fprintf(stderr, "----- KEY 'q' = Quits application from terminal\r\n");
         	fprintf(stderr, "----- KEY 'r' = Requests chunk reset\r\n");
         	fprintf(stderr, "----- KEY 'c' = Computes image from computer\r\n");
         	fprintf(stderr, "----- KEY 's' = Sends computation parameters to Nucleo\r\n");
         	fprintf(stderr, "----- KEY 'ENTER' = Starts computation\r\n");
         	fprintf(stderr, "----- KEY 'o' = Set options for computation\r\n");
         	fprintf(stderr, "----- KEY 'i' = Get information about current paramters of computation\r\n");
         	fprintf(stderr, "----- KEY 'l' = Clean buffer (image window)\r\n");
         	fprintf(stderr, "----- KEY 'm' = Show animation compted in PC from inputed parameters\r\n");
         	fprintf(stderr, "----- KEY 'b' = Enhance baud rate\r\n");
         	//fprintf(stderr, "----- KEY '' = \r\n");
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
      		fprintf(stderr,"ERROR: Something was read but nothing has been recieved!!!!\r\n");      	
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


//---function to COMPUTE DIFFERENCE (iterative step between complex numbers)----------------

double compute_difference(double start, double end, int pixels){
	double diff = fabs(start - end);
	pixels = (double)pixels;
	return (diff/pixels);	
}

double compute_start_chunk_re(image* img, computation parameters){
	int chunks_in_row = img->width/parameters.chunk_size_re;
	int number_of_chunk_in_row = parameters.cid % chunks_in_row;
	return parameters.real_start + (number_of_chunk_in_row*parameters.chunk_size_re)*parameters.real_iterative;
}

double compute_start_chunk_im(image* img, computation parameters){
	int chunks_in_row = img->width/parameters.chunk_size_re;
	int number_of_chunk_in_column = parameters.cid/ chunks_in_row;
	return parameters.imag_end + (number_of_chunk_in_column*parameters.chunk_size_im) *parameters.imag_iterative;
}
//----function to compute sets in PC--------------------------------------------------------------------
image* compute_at_CPU(image* img, computation parameters, bool change_c){
	double r_start, i_start;
	r_start = parameters.real_start;
	i_start = parameters.imag_end;
	for (int y = img->height-1; y >= 0; y--){
		for (int x = 0; x < img->width; x++){
			uint8_t iter = compute_fractal(parameters.real_constant, parameters.imag_constant,
				r_start + x*parameters.real_iterative, i_start + y*parameters.imag_iterative, parameters.n);
			double t = (double)iter/(double)parameters.n;
			double R = 9 * (1 - t) * t * t * t * 255;
			double G = 15 * (1 - t) * (1 - t) * t * t * 255;
			double B = 8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255;	
			img->data[y*(img->width) + x].R = (uint8_t)R;
			img->data[y*(img->width) + x].G = (uint8_t)G;
			img->data[y*(img->width) + x].B = (uint8_t)B;
			if (change_c){
				parameters.real_constant += 0.01;
				parameters.imag_constant += 0.01;
			}
		}
	}
	return img;
}
image* computeCPUanimation(image* img,computation parameters){
	double cR,cI;
	cR = parameters.real_constant;
	cI = parameters.imag_constant;
	for (int i = 0; i < 1000; i++){
		img = compute_at_CPU(img, parameters, false);
		parameters.real_constant += 0.001;
		parameters.imag_constant -= 0.001;
		window_refresh(img);
		sleep(0.1);
	}
	parameters.real_constant = cR;
	parameters.imag_constant = cI;
	return img;
}

//---------------------FRACTAL COMPUTING FUNCTION-------------------------------

uint8_t compute_fractal(double real_c, double imag_c, double real_input, double imag_input, uint8_t iterations){
    double new_real, new_imag, old_real, old_imag;
    uint8_t counter = 0;
    new_real = real_input;
    new_imag = imag_input;
    
    while ((new_real * new_real + new_imag * new_imag) < 4 && counter < iterations){
        old_real = new_real;
        old_imag = new_imag;
        new_real = old_real * old_real - old_imag * old_imag + real_c;
        new_imag = 2 * old_real * old_imag + imag_c;
        counter++;        
   
    }
    return counter;          
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


/* end of main.c */
