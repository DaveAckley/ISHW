#include "Arduino.h"         // For millis()
#include "Packets.h"         // For PacketBuffer, etc
#include "FaceQueue.h"       // For faceQueues[]
#include "EventGen.h"     

#define OVERFLOW_MILLIS 44740  // This isn't quite right but going to usec or clks isn't worth it to me
unsigned int myMillis() {
  static unsigned int lastMillis = millis(); 
  static unsigned int totMillis = 0;
  static unsigned int calls = 0;
  unsigned int curMillis = millis();
  ++calls;

  if (curMillis < lastMillis) totMillis += OVERFLOW_MILLIS;
  totMillis += curMillis-lastMillis; // subtraction might underflow but it's okay

  lastMillis = curMillis;
  return totMillis;
}

// Generate some kind of bogus 'semi-MFM-ish' communications workload.
//
// As far as communications go, there are basically three kinds of
// events in the MFM: 
//
// (1) Ones that are completely internal and require no communication,
// (2) Ones near an edge that require communicating with one other
//     tile, and 
// (3) Ones near a corner that require communicating with
//     three other tiles.
//
// Here we skip (1) and model (2) and (3) by picking a random face.
// If it's an edge we imagine about have the event window will need to
// be transmitted to one other tile, and if it's a corner we imagine
// about three quarters of the event window will need to be
// transmitted to three other tiles.
//
// A major bogosity of this code is that it ignores the MFM event
// locking protocol.  So this code is perfectly happy to have two
// neighboring tiles blasting updates at each other simultaneously,
// when actually they wouldn't because only one side would hold the
// lock between them at any given point.

bool eventProcessingInitted = false;
void eventProcessing() {
  static unsigned long eventCount = 0;
  static unsigned long startMillis = myMillis();

  unsigned long now = myMillis();  // Call for millis frequently to catch rollovers....

  if (!eventProcessingInitted) {  // WORKAROUND: Some kind of undiagnosed static initialization problem ;(
    startMillis = myMillis();
    eventProcessingInitted = true;
  }

  static unsigned long badWords = 0;
  static unsigned long badPackets = 0;

  // First, process inbound packets.  Here we spend O(n) resources per
  // face checking the data of up to one n-word packet, and then toss
  // the thing.

  for (int face = NT; face < FACE_COUNT; ++face) {
    PacketBuffer * pb = faceQueues[face].inbound.remove();

    if (pb) {
      // Kill time and mem b/w proportional to packet size
      bool badPacket = false;
      for (unsigned int w = 0; w < pb->trailer.length; ++w) {
        if (pb->words[w] != (w&0xf)*0x11111111) {
          ++badWords;
          badPacket = true;
        }
      }
      if (badPacket) ++badPackets;

      // Done
      deletePacketBuffer(pb);
    }
  }

  // Second, check if there's previous outbound stuff that hasn't
  // shipped yet, and block if so.  This is a very bogus take on event
  // processing.

  for (int face = NT; face < FACE_COUNT; ++face) {
    if (!faceQueues[face].outbound.isEmpty()) { 
      return;      // Blocked waiting for shipment, try again later
    }
  }
  
  // Sooner or later, we will find no more packets inbound or
  // outbound.  At that point we'll declare the 'event' is over.  We
  // increment the eventCount, and every so often, report some event
  // statistics.

  const unsigned int PERIOD = 25000;
  if (++eventCount % PERIOD == 0) {
    unsigned long totMillis = now-startMillis;

    Serial.print(totMillis/1000.0);
    Serial.print("s: ");
    Serial.print(eventCount/1000);
    Serial.print("Kevt, ");
    Serial.print(eventCount*1000.0/totMillis);
    Serial.print(" ev/s");

#if PACKET_QUEUE_STATS
    Serial.print(". ");
    unsigned long faceCount = 0;
    unsigned long packetsInjected = 0;
    unsigned long wordsInjected = 0;
    unsigned long packetsExtracted = 0;
    unsigned long wordsExtracted = 0;
    for (int f = NT; f < FACE_COUNT; ++f) {

      // Assume face is unconnected if never rcvd packet
      if (faceQueues[f].inbound.packetsIn == 0) continue;

      ++faceCount;
      packetsInjected += faceQueues[f].inbound.packetsIn;
      wordsInjected += faceQueues[f].inbound.wordsIn;

      packetsExtracted += faceQueues[f].outbound.packetsOut;
      wordsExtracted += faceQueues[f].outbound.wordsOut;
    }
    if (faceCount > 0) {
      Serial.print(faceCount);
      Serial.print(" active chnls. ");
      Serial.print(packetsInjected*1000.0/totMillis);
      Serial.print(",");
      Serial.print(wordsInjected*1.0/totMillis);
      Serial.print(" pk,Kwd/s in; ");
      Serial.print(packetsExtracted*1000.0/totMillis);
      Serial.print(",");
      Serial.print(wordsExtracted*1.0/totMillis);
      Serial.print(" pk,Kwd/s out; ");
      Serial.print(badPackets);
      Serial.print(",");
      Serial.print(badWords);
      Serial.print(" bad pk,wd");
    }
#endif /* PACKET_QUEUE_STATS */

    Serial.println();
  }
  

  // Finally, we start up another event by selecting a random face.
  int eventFace = random(FACE_COUNT);
  int firstFaceM = eventFace;
  int lastFaceM = eventFace;
  if (eventFace&1) {
    firstFaceM += FACE_COUNT-1;    // back one (in mod)
    lastFaceM += FACE_COUNT+1;      // ahead one (ditto)
  }

  for (int fm = firstFaceM; fm<=lastFaceM; ++fm) {
    unsigned int f = fm%FACE_COUNT;
    unsigned int wordCount =  (f&1)?111:74;  // 3/4 or 1/2 of 96 bit * 49 site event window
    while (wordCount > 0) {
      int plen = wordCount > PACKET_MAX_WORDS ? PACKET_MAX_WORDS : wordCount;
      wordCount -= plen;
      PacketBuffer * pb = newPacketBuffer();
      for (unsigned int w = 0; w < wordCount; ++w) {
        pb->words[w] = (w&0xf)*0x11111111;  // sixteen data patterns, including 32 1's
      }
      pb->trailer.length = wordCount;

      faceQueues[f].insertOutboundBG(pb);
    }
  }
}


