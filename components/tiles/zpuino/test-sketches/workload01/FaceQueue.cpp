#include "FaceQueue.h"
#include "Arduino.h"  // For noInterrupts(), interrupts()

FaceQueue faceQueues[FACE_COUNT];

void FaceQueue::insertOutboundBG(PacketBuffer * pb) {
  noInterrupts();             // Brute force for now
  outbound.insert(pb);
  interrupts();
}

PacketBuffer * FaceQueue::removeInboundBG() {
  noInterrupts();             // Ditto
  PacketBuffer * ret = inbound.remove();
  interrupts();
  return ret;
}

