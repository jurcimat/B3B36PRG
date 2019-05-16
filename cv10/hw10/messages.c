/*
 * File name: messages.c
 * Date:      2017/04/15 11:22
 * Author:    Jan Faigl
 */

#include "messages.h"

// - function  ----------------------------------------------------------------
bool get_message_size(uint8_t msg_type, int *size)
{
   bool ret = true;
   switch(msg_type) {
      case MSG_OK:
      case MSG_ERROR:
      case MSG_ABORT:
      case MSG_DONE:
      case MSG_GET_VERSION:
         *size = 2; // 2 bytes message - id + cksum
         break;
      case MSG_STARTUP:
         *size = 2 + STARTUP_MSG_LEN;
         break;
      case MSG_VERSION:
         *size = 2 + 3 * sizeof(uint8_t); // 2 + major, minor, patch
         break;
      case MSG_COMPUTE:
         *size = 2 + 2 * sizeof(uint16_t); // 2 + chunk_id (16bit) + nbr_tasks (16bit)
         break;
      case MSG_COMPUTE_DATA:
	 *size = 2 + 2 * sizeof(uint16_t) + 1; // 2 + chunk_id (16bit) + task_id (16bit) result (8bit)
	 break;
      default:
	 ret = false;
	 break;
   }
   return ret;
}

// - function  ----------------------------------------------------------------
bool fill_message_buf(const message *msg, uint8_t *buf, int size, int *len)
{
   if (!msg || size < sizeof(message) || !buf) {
      return false;
   }
   // 1st - serialize the message into a buffer
   bool ret = true;
   *len = 0;
   switch(msg->type) {
      case MSG_OK:
      case MSG_ERROR:
      case MSG_ABORT:
      case MSG_DONE:
      case MSG_GET_VERSION:
         *len = 1;
         break;
      case MSG_STARTUP:
         for (int i = 0; i < STARTUP_MSG_LEN; ++i) {
            buf[i+1] = msg->data.startup.message[i];
         }
         *len = 1 + STARTUP_MSG_LEN;
         break;
      case MSG_VERSION:
         buf[1] = msg->data.version.major;
         buf[2] = msg->data.version.minor;
         buf[3] = msg->data.version.patch;
         *len = 4;
         break;
      case MSG_COMPUTE:
         buf[1] = (uint8_t)(msg->data.compute.chunk_id >> 8); // hi - chunk_id
         buf[2] = (uint8_t)msg->data.compute.chunk_id;        // lo - chunk_id
         buf[3] = (uint8_t)(msg->data.compute.nbr_tasks >> 8);// hi - nbr_tasks
         buf[4] = (uint8_t)msg->data.compute.nbr_tasks;       // lo - nbr_tasks
         *len = 5;
         break;
      case MSG_COMPUTE_DATA:
         buf[1] = (uint8_t)(msg->data.compute_data.chunk_id >> 8);// hi - chunk_id
         buf[2] = (uint8_t)msg->data.compute_data.chunk_id;       // lo - chunk_id
         buf[3] = (uint8_t)(msg->data.compute_data.task_id >> 8); // hi - task_id
         buf[4] = (uint8_t)msg->data.compute_data.task_id;        // lo - task_id
         buf[5] = msg->data.compute_data.result; // result
         *len = 6;
         break;
      default: // unknown message type
         ret = false;
         break;
   }
   // 2nd - send the message buffer
   if (ret) { // message recognized
      buf[0] = msg->type;
      buf[*len] = 0; // cksum
      for (int i = 0; i < *len; ++i) {
         buf[*len] += buf[i];
      }
      buf[*len] = 255 - buf[*len]; // compute cksum
      *len += 1; // add cksum to buffer
   }
   return ret;
}

// - function  ----------------------------------------------------------------
bool parse_message_buf(const uint8_t *buf, int size, message *msg)
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
         case MSG_COMPUTE: // type + chunk_id + nbr_tasks
            msg->data.compute.chunk_id = (buf[1] << 8) | buf[2];
            msg->data.compute.nbr_tasks = (buf[3] << 8) | buf[4];
            break;
         case MSG_COMPUTE_DATA:  // type + chunk_id + task_id + result
            msg->data.compute_data.chunk_id = (buf[1] << 8) | buf[2];
            msg->data.compute_data.task_id = (buf[3] << 8) | buf[4];
            msg->data.compute_data.result = buf[5];
	    break;
         default: // unknown message type
            ret = false;
            break;
      } // end switch
   }
   return ret;
}

/* end of messages.c */
