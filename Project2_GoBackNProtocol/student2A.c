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

struct pkt aPacket;
int currState;

struct pkt win_cont[50000];
int win_seq = 1;
int win_base = 1;
int win_size = 8;


/*
* A_output(message), where message is a structure of type msg, containing
* data to be sent to the B-side. This routine will be called whenever the
* upper layer at the sending side (A) has a message to send. It is the job
* of your protocol to insure that the data in such a message is delivered
* in-order, and correctly, to the receiving side upper layer.
*/

/* creates the packet */
void createPacket(struct msg message, struct pkt outPacket){
  outPacket.acknum = 0;
  outPacket.seqnum = win_seq;
  strncpy(outPacket.payload, message.data, 20);
  outPacket.checksum = calculateChecksum(outPacket);
  win_cont[win_seq] = outPacket;
}

void sendPacket(struct pkt outPacket){

  /* sequence number must be less than base and size */
  if(win_seq < (win_base + win_size)) {
    tolayer3(AEntity, outPacket);

    /* base must equal sequence */
    if(win_base == win_seq) {
      startTimer(AEntity, TIMEOUT);
    }

  }
  /* add packet to the sequence */
  win_seq++;
}

void A_output(struct msg message) {
  struct pkt outPacket;
  /* create 'outPacket' containing 'message' */
  createPacket(message, outPacket);
  /* send outPacket */
  sendPacket(outPacket);
}


/*
* A_input(packet), where packet is a structure of type pkt. This routine
* will be called whenever a packet sent from the B-side (i.e., as a result
* of a tolayer3() being done by a B-side procedure) arrives at the A-side.
* packet is the (possibly corrupted) packet sent from the B-side.
*/

/* STEP 1: Check if ACK only or not
* STEP 2: Check checksum
* STEP 3: Check if valid ACK
* STEP 4: Set variables and clear buffer if anything exists there
* STEP 5: Send the message
*/



void A_input(struct pkt packet) {

  /* checksum match */
  if(packet.checksum == calculateChecksum(packet)) {

    /* checks if no more packets to send */
    if( win_base >= ( packet.seqnum + 1 ) ) {
      if( ( packet.seqnum + 1 ) == win_seq ) {
        stopTimer(AEntity);
      }
    }

    else {
      /* updates sequence to be the new base */
      win_base = packet.seqnum + 1;

      /* just stop timer */
      if( win_base == win_seq ){
        stopTimer(AEntity);
      }

      else {
        /* restart timer */
        stopTimer(AEntity);
        startTimer(AEntity, TIMEOUT);
      }
    }
  }
}

/*
* A_timerinterrupt()  This routine will be called when A's timer expires
* (thus generating a timer interrupt). You'll probably want to use this
* routine to control the retransmission of packets. See starttimer()
* and stoptimer() in the writeup for how the timer is started and stopped.
*/
void A_timerinterrupt() {
  int pkt = win_base;

  /* when packet is less than base and max size */
  for( ; pkt < (win_base + win_size) ; pkt++)
  {
    tolayer3(AEntity, win_cont[pkt]);
  }
  /* start timer */
  startTimer(AEntity, TIMEOUT);
}


/* The following routine will be called once (only) before any other    */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {

}
