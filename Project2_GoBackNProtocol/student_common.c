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

/*
* calculate CRC on that character
*/

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


/*
* Calculates the CRC checksum for a given packet
*/
int calculateChecksum(struct pkt packet) {
  int temp;
  char pkt[sizeof(packet)];
  int pktSize = sizeof(pkt);

  packet.checksum = 0;
  memcpy(pkt, &packet, sizeof(packet));
  temp = calculateCharacter(pkt,sizeof(pkt));
  return temp;
}
