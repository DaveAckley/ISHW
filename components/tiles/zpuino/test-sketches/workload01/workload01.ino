#include "Packets.h"   // For initPackets()
#include "EventGen.h"  // For eventProcessing()
#include "FaceQueue.h" // For faceQueues[]

void setup() {
  Serial.begin(115200);
  Serial.println("Workload01");

  initPackets();

  // Set up interrupt processing here.  See pseudocode and code below.
  //
  // For a device transmitting, say, South (code 'ST'), attach
  // supplyOutbound(faceQueues[ST] to the device's packet-needed TX
  // interrupt, and attach handleInbound(faceQueues[ST]) to the
  // device's packet-available RX interrupt.

}

/*
  Interrupt handling pseudocode

    // Called when an inbound packet has been completely received
    void ISHW_class::handleRXInterrupt() {
      int faceCode = this->getFaceCode();
      PacketBuffer * pb = (PacketBuffer *) this->getInterruptRXPointer();
      handleInbound(faceQueue[faceCode], pb); // code below
    }

    // Called when an outbound packet has been completely transmitted
    void ISHW_class::handleTXInterrupt() {
      PacketBuffer * oldpb = this->getJustFinishedTXPointer();
      if (oldpb) deletePacketBuffer(oldpb);  // Return TX'd packetbuffer to pool

      int faceCode = this->getFaceCode();
      PacketBuffer * pb = supplyOutbound(faceQueue[faceCode]);  // code below
      if (pb) this->setNextTXPointer(pb);
      // else device idles
      //
      // Note that some other code (not running at interrupt level)
      // must know how to prime the TX pump when the TX side has gone
      // idle and another outbound packet is produced.
    }
*/

void handleInbound(FaceQueue& fq, PacketBuffer * pb) {
  fq.insertInboundIL(pb);
}

void supplyOutbound(FaceQueue& fq) {
  PacketBuffer * pb = fq.removeOutboundIL();
  if (pb) {
    // Supply pb to the device.  Here, just for a demo, we are
    // pretending the packet we are about to send just arrived on the
    // same face

    handleInbound(fq, pb);
  }
}

void loop() {

  eventProcessing();  // Do business

  // Fake stub covering the missing IO devices and interconnect
  for (int f = NT; f < FACE_COUNT; ++f) {
    supplyOutbound(faceQueues[f]);
  }
}
