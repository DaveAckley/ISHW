#ifndef _PACKETS_H_
#define _PACKETS_H_

#ifndef PACKET_QUEUE_STATS
#define PACKET_QUEUE_STATS 1  /* For now, default to having stats */
#endif

void initPackets() ;

struct PacketBuffer; // Forward

struct PacketTrailer {
  unsigned char length;
  unsigned char reserved[3];
  PacketBuffer * next;

  PacketTrailer() : length(0), next(0) { }
};

#define PACKET_MAX_WORDS ((256-sizeof(PacketTrailer))/sizeof(unsigned long))

struct PacketBuffer {
  unsigned long words[PACKET_MAX_WORDS];
  PacketTrailer trailer;
};

struct PacketQueue {
  PacketBuffer * first;
  PacketBuffer * last;

#if PACKET_QUEUE_STATS
  unsigned long packetsIn, packetsOut, wordsIn, wordsOut;
#endif

  void insert(PacketBuffer *) ;
  PacketBuffer * remove() ;
  bool isEmpty() { return first==0; }

  PacketQueue() : first(0), last(0)
#if PACKET_QUEUE_STATS
    , packetsIn(0), packetsOut(0), wordsIn(0), wordsOut(0)
#endif
 { }
};

PacketBuffer * newPacketBuffer() ;
void deletePacketBuffer(PacketBuffer *) ;

#endif /* _PACKETS_H_ */
