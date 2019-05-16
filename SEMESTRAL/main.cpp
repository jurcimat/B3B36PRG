#include "mbed.h"

#define STARTUP_MSG_LEN 9
#define MESSAGE_SIZE 255
#define BUF_SIZE 255

#define VERSION_MAJOR 0
#define VERSION_MINOR 9
#define VERSION_PATCH 0
 

 
Serial serial(SERIAL_TX, SERIAL_RX);
InterruptIn button_event(USER_BUTTON);
DigitalOut myled(LED1);
Ticker ticker;

//---------------------MESSAGING PROTOCOL FUNCTIONS-----------------------------
// Definition of the communication messages
typedef enum {
   MSG_OK,               // ack of the received message
   MSG_ERROR,            // report error on the previously received command
   MSG_ABORT,            // abort - from user button or from serial port
   MSG_DONE,             // report the requested work has been done
   MSG_GET_VERSION,      // request version of the firmware
   MSG_VERSION,          // send version of the firmware as major,minor, patch level, e.g., 1.0p1
   MSG_STARTUP,          // init message (id, up to 9 bytes long string, cksum)
   MSG_SET_COMPUTE,      // set computation parameters
   MSG_COMPUTE,          // request computation of a batch of tasks (chunk_id, nbr_tasks)
   MSG_COMPUTE_DATA,     // computed result (chunk_id, result)
   MSG_ENHANCE_BAUD,
   MSG_NBR
} message_type;
 
 
typedef struct {
   uint8_t major;
   uint8_t minor;
   uint8_t patch;
} msg_version;
 
typedef struct {
   uint8_t message[STARTUP_MSG_LEN];
} msg_startup;
 
typedef struct {
   double c_re;  // re (x) part of the c constant in recursive equation
   double c_im;  // im (y) part of the c constant in recursive equation
   double d_re;  // increment in the x-coords
   double d_im;  // increment in the y-coords
   uint8_t n;    // number of iterations per each pixel
} msg_set_compute;
 
typedef struct {
    uint8_t cid; // chunk id
    double re;    // start of the x-coords (real)
    double im;    // start of the y-coords (imaginary)
    uint8_t n_re; // number of cells in x-coords
    uint8_t n_im; // number of cells in y-coords
} msg_compute;
 
typedef struct {
   uint8_t cid;  // chunk id
   uint8_t i_re; // x-coords 
   uint8_t i_im; // y-coords
   uint8_t iter; // number of iterations
} msg_compute_data;
 
typedef struct {
   uint8_t type;   // message type
   union {
      msg_version version;
      msg_startup startup;
      msg_set_compute set_compute;
      msg_compute compute;
      msg_compute_data compute_data;
   } data;
   uint8_t cksum; // message command
} message;

msg_version VERSION = { .major = VERSION_MAJOR, .minor = VERSION_MINOR, .patch = VERSION_PATCH };

//------------MESSAGING FUNCTIONS-----------------------------------------------
bool get_message_size(uint8_t msg_type, int *len);
bool send_message(const message *msg, uint8_t *buf, int size);
bool parse_message(const uint8_t *buf, int size, message *msg);

//-----------------------TRANSFER HANDLING FUNCTIONS---------------------------- 
void Tx_interrupt();
void Rx_interrupt();
bool receive_message(uint8_t *msg_buf, int size, int *len);
bool send_buffer(const uint8_t* msg, unsigned int size);
 

 
char tx_buffer[BUF_SIZE];
char rx_buffer[BUF_SIZE];
 
// pointers to the circular buffers
volatile int tx_in = 0;
volatile int tx_out = 0;
volatile int rx_in = 0;
volatile int rx_out = 0;


//-------------------------BUTTON ACTION----------------------------------------
volatile bool abort_request = false;

 
void button() // buton action
{
   abort_request = true;
}

void tick(){  // LED action
    myled = !myled;     
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
//----------------------COMPUTING HANDLE STRUCTURE------------------------------
typedef struct {
    uint8_t cid;            // chunk_id
    uint8_t n;              // number of iterations
    uint8_t chunk_size_re;     // size of chunk
    uint8_t chunk_size_im;
    double real_iterative;
    double imag_iterative;  // current complex number
    double real_start;          
    double imag_start;      // start point complex number
    double real_constant;
    double imag_constant;   // complex constant c
    bool computing;         // true or false wheter NUCLEO is computing
    int current_pixel_x;    
    int current_pixel_y;    // pixel coordinates needed for computation
    bool fast;
}computation;

computation parameters; 
//--------------------------MAIN FUNCTION---------------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX 
int main(void) {
    //Define variables of programm
    uint8_t msg_buf [BUF_SIZE];
    int msg_len;
    message msg = { .data.startup.message =  { '#','J', 'U', 'R', 'C', 'I', 'M', 'A', 'T' } };; 
    float period = 0.25;
    
    //Set up comunication between PC and Nucleo
    parameters.computing = false;
    parameters.fast = false;
    serial.baud(115200);
    for (int i = 0; i < 5*2; ++i) { // 5x zablikání LED s periodou 50 ms
        myled = !myled;
        wait(0.05);
    }
    
    serial.attach(&Rx_interrupt, Serial::RxIrq); // attach interrupt handler to receive data
    serial.attach(&Tx_interrupt, Serial::TxIrq); // attach interrupt handler to transmit data
    button_event.rise(&button);
    
    
    //Send START UP message
    msg.type = MSG_STARTUP;
    send_message(&msg, msg_buf, MESSAGE_SIZE);
    
     while (1) {
        if (abort_request) {
            if (parameters.computing) {  //abort computing
                msg.type = MSG_ABORT;
                send_message(&msg, msg_buf, MESSAGE_SIZE);
                parameters.computing = false;
                abort_request = false;
                ticker.detach();
                myled = 0;
            }
        }
        if (rx_in != rx_out) { // something is in the receive buffer
            if (receive_message(msg_buf, MESSAGE_SIZE, &msg_len)) {
                if (parse_message(msg_buf, msg_len, &msg)) {
                    switch(msg.type) {
                        case MSG_GET_VERSION:
                            msg.type = MSG_VERSION;
                            msg.data.version  = VERSION;
                            send_message(&msg, msg_buf, MESSAGE_SIZE);
                            break;
                        case MSG_ABORT:
                            msg.type = MSG_OK;
                            send_message(&msg, msg_buf, MESSAGE_SIZE);
                            parameters.computing = false;
                            abort_request = false;
                            ticker.detach();
                            myled = 0;
                            break;
                        case MSG_SET_COMPUTE:
                            msg.type = MSG_OK;
                            parameters.n = msg.data.set_compute.n;
                            parameters.real_constant = msg.data.set_compute.c_re;
                            parameters.imag_constant = msg.data.set_compute.c_im;
                            parameters.real_iterative = msg.data.set_compute.d_re;
                            parameters.imag_iterative = msg.data.set_compute.d_im;
                            send_message(&msg, msg_buf, MESSAGE_SIZE);
                            break;
                        case MSG_COMPUTE:
                            if (!parameters.computing) {
                                ticker.attach(tick, period);
                                parameters.computing = true;
                                parameters.cid = msg.data.compute_data.cid;
                                parameters.real_start = msg.data.compute.re;
                                parameters.imag_start = msg.data.compute.im;
                                parameters.current_pixel_x = 0;
                                parameters.current_pixel_y = 0;
                                parameters.chunk_size_re = msg.data.compute.n_re;
                                parameters.chunk_size_im = msg.data.compute.n_im;
                            }
                            msg.type = MSG_OK;
                            send_message(&msg, msg_buf, MESSAGE_SIZE);
                            break;
                         case MSG_ENHANCE_BAUD:
                            if (!parameters.computing){
                                if(!parameters.fast){
                                    parameters.fast = true;
                                    serial.baud(576000);
                                }else if (parameters.fast){
                                    parameters.fast = false;
                                    serial.baud(115200);        
                                }
                                msg.type = MSG_OK;
                                send_message(&msg, msg_buf, MESSAGE_SIZE);
                            }
                            
                            break;
                    } // end switch
                } else { // message has not been parsed send error
                    msg.type = MSG_ERROR;
                    send_message(&msg, msg_buf, MESSAGE_SIZE);
                }
            } // end message received
        } else if (parameters.computing) {
                int x = parameters.current_pixel_x;
                int y = parameters.current_pixel_y;
                int x_size = parameters.chunk_size_re;
                int y_size = parameters.chunk_size_im;
                if (x != x_size && y != y_size){
                    msg.type = MSG_COMPUTE_DATA;
                    msg.data.compute_data.cid = parameters.cid;
                    msg.data.compute_data.iter = (uint8_t)compute_fractal(parameters.real_constant,
                            parameters.imag_constant, parameters.real_start + x*parameters.real_iterative,
                            parameters.imag_start + y*parameters.imag_iterative, parameters.n);    
                    msg.data.compute_data.i_re = (uint8_t)x;
                    msg.data.compute_data.i_im = (uint8_t)y;
                    send_message(&msg, msg_buf, MESSAGE_SIZE);

                if ((x+1) % x_size == 0){
                    parameters.current_pixel_x = 0;
                    parameters.current_pixel_y += 1;    
                } else {
                    parameters.current_pixel_x += 1;    
                }
                
                
            } else { //computation done
                ticker.detach();
                myled = 0;
                msg.type = MSG_DONE;
                parameters.computing = false;
                send_message(&msg, msg_buf, MESSAGE_SIZE);
            }
        } else {
            sleep(); // put the cpu to sleep mode, it will be wakeup on interrupt
        }
    } // end while (1)
}
    

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//-----------------------------Recieve on Rx------------------------------------
void Rx_interrupt()
{
    // receive bytes and stop if rx_buffer is full
    while ((serial.readable()) && (((rx_in + 1) % BUF_SIZE) != rx_out)) {
        rx_buffer[rx_in] = serial.getc();
        rx_in = (rx_in + 1) % BUF_SIZE;
    }
    return;
}
//-----------------------------Transfer on Tx-----------------------------------
void Tx_interrupt()
{
    // send a single byte as the interrupt is triggered on empty out buffer 
    if (tx_in != tx_out) {
        serial.putc(tx_buffer[tx_out]);
        tx_out = (tx_out + 1) % BUF_SIZE;
    } else { // buffer sent out, disable Tx interrupt
        USART2->CR1 &= ~USART_CR1_TXEIE; // disable Tx interrupt
    }
    return;
}
//-----------------------------SEND BUFFER--------------------------------------
bool send_buffer(const uint8_t* msg, unsigned int size)
{
    if (!msg && size == 0) {
        return true;    // size must be > 0
    }
    bool ret = false;
    int i = 0;
    NVIC_DisableIRQ(USART2_IRQn); // start critical section for accessing global data
    USART2->CR1 |= USART_CR1_TXEIE; // enable Tx interrupt on empty out buffer
    bool empty = (tx_in == tx_out);
    while ( (i == 0) || i < size ) { //end reading when message has been read
        if ( ((tx_in + 1) % BUF_SIZE) == tx_out) { // needs buffer space
            NVIC_EnableIRQ(USART2_IRQn); // enable interrupts for sending buffer
            while (((tx_in + 1) % BUF_SIZE) == tx_out) {
                /// let interrupt routine empty the buffer
            }
            NVIC_DisableIRQ(USART2_IRQn); // disable interrupts for accessing global buffer
        }
        tx_buffer[tx_in] = msg[i];
        i += 1;
        tx_in = (tx_in + 1) % BUF_SIZE;
    } // send buffer has been put to tx buffer, enable Tx interrupt for sending it out
    USART2->CR1 |= USART_CR1_TXEIE; // enable Tx interrupt
    NVIC_EnableIRQ(USART2_IRQn); // end critical section
    return ret;
}
//---------------------------RECIEVE BUFFER-------------------------------------
bool receive_message(uint8_t *msg_buf, int size, int *len)
{
    bool ret = false;
    int i = 0;
    *len = 0; // message size
    NVIC_DisableIRQ(USART2_IRQn); // start critical section for accessing global data
    while ( (i == 0) || (i != *len) ) {
        if (rx_in == rx_out) { // wait if buffer is empty
            NVIC_EnableIRQ(USART2_IRQn); // enable interrupts for receing buffer
            while (rx_in == rx_out) { // wait of next character
            }
            NVIC_DisableIRQ(USART2_IRQn); // disable interrupts for accessing global buffer
        }
        uint8_t c = rx_buffer[rx_out];
        if (i == 0) { // message type
            if (get_message_size(c, len)) { // message type recognized
                msg_buf[i++] = c;
                ret = *len <= size; // msg_buffer must be large enough
            } else {
                ret = false;
                break; // unknown message
            }
        } else {
            msg_buf[i++] = c;
        }
        rx_out = (rx_out + 1) % BUF_SIZE;
    }
    NVIC_EnableIRQ(USART2_IRQn); // end critical section
    return ret;
}
//----------------------------GET MESSAGE SIZE----------------------------------
bool get_message_size(uint8_t msg_type, int *len)
{
   bool ret = true;
   switch(msg_type) {
      case MSG_OK:
      case MSG_ERROR:
      case MSG_ABORT:
      case MSG_DONE:
      case MSG_ENHANCE_BAUD:
      case MSG_GET_VERSION:
         *len = 2; // 2 bytes message - id + cksum
         break;
      case MSG_STARTUP:
         *len = 2 + STARTUP_MSG_LEN;
         break;
      case MSG_VERSION:
         *len = 2 + 3 * sizeof(uint8_t); // 2 + major, minor, patch
         break;
      case MSG_SET_COMPUTE:
         *len = 2 + 4 * sizeof(double) + 1; // 2 + 4 * params + n 
         break;
      case MSG_COMPUTE:
         *len = 2 + 1 + 2 * sizeof(double) + 2; // 2 + cid (8bit) + 2x(double - re, im) + 2 ( n_re, n_im)
         break;
      case MSG_COMPUTE_DATA:
         *len = 2 + 4; // cid, dx, dy, iter
         break;
      default:
         ret = false;
         break;
   }
   return ret;
}

//---------------------------PARSE MESSAGE--------------------------------------
bool parse_message(const uint8_t *buf, int size, message *msg)
{
   uint8_t cksum = 0;
   for (int i = 0; i < size; ++i) {
      cksum += buf[i];
   }
   bool ret = false;
   int message_size;
   if (
         size > 0 && cksum == 0xff && // sum of all bytes must be 255
         ((msg->type = buf[0]) >= 0) && msg->type < MSG_NBR &&
         get_message_size(msg->type, &message_size) && size == message_size) {
      ret = true;
      switch(msg->type) {
         case MSG_OK:
         case MSG_ERROR:
         case MSG_ABORT:
         case MSG_DONE:
         case MSG_ENHANCE_BAUD:
         case MSG_GET_VERSION:
            break;
         case MSG_STARTUP:
            for (int i = 0; i < STARTUP_MSG_LEN; ++i) {
               msg->data.startup.message[i] = buf[i+1];
            }
            break;
         case MSG_VERSION:
            msg->data.version.major = buf[1];
            msg->data.version.minor = buf[2];
            msg->data.version.patch = buf[3];
            break;
         case MSG_SET_COMPUTE: 
            memcpy(&(msg->data.set_compute.c_re), &(buf[1 + 0 * sizeof(double)]), sizeof(double));
            memcpy(&(msg->data.set_compute.c_im), &(buf[1 + 1 * sizeof(double)]), sizeof(double));
            memcpy(&(msg->data.set_compute.d_re), &(buf[1 + 2 * sizeof(double)]), sizeof(double));
            memcpy(&(msg->data.set_compute.d_im), &(buf[1 + 3 * sizeof(double)]), sizeof(double));
            msg->data.set_compute.n = buf[1 + 4 * sizeof(double)];
            break;
         case MSG_COMPUTE: // type + chunk_id + nbr_tasks
            msg->data.compute.cid = buf[1];
            memcpy(&(msg->data.compute.re), &(buf[2 + 0 * sizeof(double)]), sizeof(double));
            memcpy(&(msg->data.compute.im), &(buf[2 + 1 * sizeof(double)]), sizeof(double));
            msg->data.compute.n_re = buf[2 + 2 * sizeof(double) + 0];
            msg->data.compute.n_im = buf[2 + 2 * sizeof(double) + 1];
            break;
         case MSG_COMPUTE_DATA:  // type + chunk_id + task_id + result
            msg->data.compute_data.cid = buf[1];
            msg->data.compute_data.i_re = buf[2];
            msg->data.compute_data.i_im = buf[3];
            msg->data.compute_data.iter = buf[4];
            break;
         default: // unknown message type
            ret = false;
            break;
      } // end switch
   }
   return ret;
}
//---------------------------SEND MESSAGE SIZE----------------------------------
bool send_message(const message *msg, uint8_t *buf, int size)
{
    if (!msg || size < sizeof(message) || !buf) {
        return false;
    }
    // 1st - serialize the message into a buffer
    bool ret = true;
    int len = 0;
    switch(msg->type) {
        case MSG_OK:
        case MSG_ERROR:
        case MSG_ABORT:
        case MSG_DONE:
        case MSG_ENHANCE_BAUD:
        case MSG_GET_VERSION:
            len = 1;
            break;
        case MSG_STARTUP:
            for (int i = 0; i < STARTUP_MSG_LEN; ++i) {
                buf[i+1] = msg->data.startup.message[i];
            }
            len = 1 + STARTUP_MSG_LEN;
            break;
        case MSG_VERSION:
            buf[1] = msg->data.version.major;
            buf[2] = msg->data.version.minor;
            buf[3] = msg->data.version.patch;
            len = 4;
            break;
        case MSG_SET_COMPUTE:
            memcpy(&(buf[1 + 0 * sizeof(double)]), &(msg->data.set_compute.c_re), sizeof(double));
            memcpy(&(buf[1 + 1 * sizeof(double)]), &(msg->data.set_compute.c_im), sizeof(double));
            memcpy(&(buf[1 + 2 * sizeof(double)]), &(msg->data.set_compute.d_re), sizeof(double));
            memcpy(&(buf[1 + 3 * sizeof(double)]), &(msg->data.set_compute.d_im), sizeof(double));
            buf[1 + 4 * sizeof(double)] = msg->data.set_compute.n;
            len = 1 + 4 * sizeof(double) + 1;
            break;
        case MSG_COMPUTE:
            buf[1] = msg->data.compute.cid; // cid
            memcpy(&(buf[2 + 0 * sizeof(double)]), &(msg->data.compute.re), sizeof(double));
            memcpy(&(buf[2 + 1 * sizeof(double)]), &(msg->data.compute.im), sizeof(double));
            buf[2 + 2 * sizeof(double) + 0] = msg->data.compute.n_re;
            buf[2 + 2 * sizeof(double) + 1] = msg->data.compute.n_im;
            len = 1 + 1 + 2 * sizeof(double) + 2;
            break;
        case MSG_COMPUTE_DATA:
            buf[1] = msg->data.compute_data.cid;
            buf[2] = msg->data.compute_data.i_re;
            buf[3] = msg->data.compute_data.i_im;
            buf[4] = msg->data.compute_data.iter;
            len = 5;
            break;
        default: // unknown message type
            ret = false;
            break;
    }
    // 2nd - send the message buffer
    if (ret) { // message recognized
        buf[0] = msg->type;
        buf[len] = 0; // cksum
        for (int i = 0; i < len; ++i) {
            buf[len] += buf[i];
        }
        buf[len] = 255 - buf[len]; // compute cksum
        len += 1; // add cksum to buffer
        ret = send_buffer(buf, len);
    }
    return ret;
}