#include "Packets.h"

#define BUFFER_COUNT 100
static PacketBuffer buffers[BUFFER_COUNT] __attribute__((aligned(256)));

static PacketQueue _freeList;

void initPackets() {
  for (int i = 0; i < BUFFER_COUNT; ++i) {
    buffers[i].trailer.next = 0;
    deletePacketBuffer(&buffers[i]);
  }
}

PacketBuffer * newPacketBuffer() {
  return _freeList.remove();
}

void deletePacketBuffer(PacketBuffer * pb) {
  _freeList.insert(pb);
}

void PacketQueue::insert(PacketBuffer * pb) {
  if (first==0) first = pb;
  if (last==0) last = pb;
  else { 
    last->trailer.next = pb;
    last = pb;
  }
  last->trailer.next = 0;

#if PACKET_QUEUE_STATS
  ++packetsIn;
  wordsIn += pb->trailer.length;
#endif

}

PacketBuffer * PacketQueue::remove() {
  if (first==0) return 0;
  PacketBuffer * ret = first;
  if (first==last) {
    first = last = 0;
  } else {
    first = first->trailer.next;
  }
  ret->trailer.next = 0;

#if PACKET_QUEUE_STATS
  ++packetsOut;
  wordsOut += ret->trailer.length;
#endif

  return ret;
}
