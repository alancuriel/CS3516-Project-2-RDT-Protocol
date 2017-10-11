#include <stdio.h>
#include <stdlib.h>
#include "project2.h"
#include <string.h>


/* ***************************************************************************
ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

This code should be used for unidirectional or bidirectional
data transfer protocols from A to B and B to A.
Network properties:
- one way network delay averages five time units (longer if there
are other messages in the channel for GBN), but can be larger
- packets can be corrupted (either the header or the data portion)
or lost, according to user-defined probabilities
- packets may be delivered out of order.

Compile as gcc -g project2.c student2.c -o p2
**********************************************************************/



/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
/*
* The routines you will write are detailed below. As noted above,
* such procedures in real-life would be part of the operating system,
* and would be called by other procedures in the operating system.
* All these routines are in layer 4.
*/

// source: http://stackoverflow.com/questions/23478417/cyclic-redundancy-check-calculation-c-to-c-sharp-code-conversion-not-working

struct pkt aPacket;
int lastPacket;


/* Node Structs */

struct Node* front = NULL;
struct Node* rear = NULL;

struct Node {
  char data[20];
  struct Node* next;
};


/* Enqueue */

void Enqueue(char x[20]) {
  struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
  strncpy(temp->data, x, 20);
  temp->next = NULL;
  if(front == NULL && rear == NULL){
    front = rear = temp;
    return;
  }
  rear->next = temp;
  rear = temp;
  return;
}

/* Dequeue */

char *Dequeue() {
  struct Node* temp = front;
  if(front == NULL) {
    return NULL;
  }
  if(front == rear) {
    front = rear = NULL;
  } else {
    front = front->next;
  }
  return temp->data;
}

/* Calculate the characters */

unsigned short calculateCharacter(char* buff, int pktSize){
  int i,j;
  unsigned short cword = 0;
  unsigned short ch;

  for (i = 0; i < pktSize; i++) {
    ch = buff[i] << 8;
    for (j = 0; j < 8; j++) {
      if ((ch & 0x8000) ^ (cword & 0x8000)) {
        int temp = (cword <<= 1) ^ 4000;
        cword = temp;
      }
      else {
        cword <<= 1;
      }
      ch <<= 1;
    }
  }
  return cword;
}


/* Check the checksum of packet */

int calculateChecksum(struct pkt packet) {
  int temp;
  char pkt[sizeof(packet)];
  int pktSize = sizeof(pkt);
  packet.checksum = 0;
  memcpy(pkt, &packet, sizeof(packet));
  temp = calculateCharacter(pkt,sizeof(pkt));
  return temp;
}


/* creates packet */

void createPacketA(struct msg message){
    aPacket.seqnum = (aPacket.seqnum + 1) % 2;
    aPacket.acknum = 0;
    strncpy(aPacket.payload, message.data, 20);
    aPacket.checksum = calculateChecksum(aPacket);
}

/* creates message */

void createMessageA(){
    struct msg message;

    /* copies message into payload */
    strncpy(message.data, aPacket.payload, 20);

    /* fix sequence number */
    aPacket.seqnum = (aPacket.seqnum + 1) % 2;

    /* outputs message */
    A_output(message);
}

/* send message */

void sendMessageA(struct pkt packet){
    char *bufferText = Dequeue();

    /* DEBUG */
    if( DEBUGLEVEL > 1 ){
        printf("A->B: acknum: %d; seqnum: %d; oldmsg: %.20s; msg: %.20s, sending...\n",
               packet.acknum, packet.seqnum, aPacket.payload, bufferText);
    }

    /* when Dequeue contains characters */
    if( bufferText != NULL ) {
        struct msg msgSend;

        /* copy message contents */
        strncpy(msgSend.data, bufferText, 20);

        /* send message */
        A_output(msgSend);
    }

}

/* sends valid packet to layer 3 */
void sendMessageB(struct pkt ACKPacket){
    ACKPacket.acknum = 1;
    /* new packet is set to old */
    ACKPacket.seqnum = lastPacket;
    memset(ACKPacket.payload, '\0', sizeof(ACKPacket.payload));
    /* calculate checksum */
    ACKPacket.checksum = calculateChecksum(ACKPacket);
    /* send to layer 3 */
    tolayer3(BEntity, ACKPacket);
}
