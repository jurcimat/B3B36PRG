/*
 * File name: messages.h
 * Date:      2017/04/15 11:21
 * Author:    Jan Faigl
 */

#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <stdint.h>
#include <stdbool.h>

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



#define STARTUP_MSG_LEN 9

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

// return the size of the message in bytes
bool get_message_size(uint8_t msg_type, int *size);

// fill the given buf by the message msg (marhaling);
bool fill_message_buf(const message *msg, uint8_t *buf, int size, int *len);

// parse the message from buf to msg (unmarshaling)
bool parse_message_buf(const uint8_t *buf, int size, message *msg);


#endif

/* end of messages.h */
