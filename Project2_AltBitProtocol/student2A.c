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

/*
* A_output(message), where message is a structure of type msg, containing
* data to be sent to the B-side. This routine will be called whenever the
* upper layer at the sending side (A) has a message to send. It is the job
* of your protocol to insure that the data in such a message is delivered
* in-order, and correctly, to the receiving side upper layer.
*/


void A_output(struct msg message) {

  /* add to queue second time*/
  if( currState == 1 ) {
    Enqueue(message.data);
    return;
  }

  /* create packet */
  createPacketA(message);

  /* end timer */
  stopTimer(AEntity);

  /* start new timer */
  startTimer(AEntity, TIMEOUT);

  /* send to layer 3 */
  tolayer3(AEntity, aPacket);

  /* change current state to 1 for next loop */
  currState = 1;
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
  char emptystr[20];
  memset(emptystr, '\0', sizeof(emptystr));

  /* not empty payload */
  if( !strcmp(packet.payload, emptystr) ) {

    /* checksum is correct */
    if( packet.checksum == calculateChecksum(packet) ) {

      /* ACK and Seq numbers are correct */
      if( packet.acknum == 1 && packet.seqnum == aPacket.seqnum ) {
        currState = 0;

        /* send out message */
        sendMessageA(packet);
      }
    }
  }
  return;

}

/*
* A_timerinterrupt()  This routine will be called when A's timer expires
* (thus generating a timer interrupt). You'll probably want to use this
* routine to control the retransmission of packets. See starttimer()
* and stoptimer() in the writeup for how the timer is started and stopped.
*/

void A_timerinterrupt() {

  /* stop timer */
  stopTimer(AEntity);
  currState = 0;

  /* makes the message to be sent */
  createMessageA();
}

/* The following routine will be called once (only) before any other    */
/* entity A routines are called. You can use it to do any initialization */

void A_init() {
  /* setup */
  aPacket.seqnum = 0;
  aPacket.acknum = 1;
  /* initial state */
  currState = 0;
}
