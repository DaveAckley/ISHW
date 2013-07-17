#ifndef _FACEQUEUE_H_
#define _FACEQUEUE_H_

#include "Packets.h"

struct FaceQueue {
  PacketQueue inbound;
  PacketQueue outbound;

  void insertInboundIL(PacketBuffer * pb) {   // Called at Interrupt Level
    inbound.insert(pb);
  }
  void insertOutboundBG(PacketBuffer * pb) ;  // Called by BackGround processing

  PacketBuffer * removeOutboundIL() {      // Called at Interrupt Level
    return outbound.remove();
  }
  PacketBuffer * removeInboundBG() ;       // Called by BackGround processing
};

enum FaceCode { NT = 0, NE, ET, SE, ST, SW, WT, NW, FACE_COUNT };

extern FaceQueue faceQueues[FACE_COUNT];

#endif /* _FACEQUEUE_H_ */
