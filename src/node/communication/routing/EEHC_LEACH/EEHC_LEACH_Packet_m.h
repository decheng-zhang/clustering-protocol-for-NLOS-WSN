//
// Generated file, do not edit! Created by nedtool 4.6 from src/node/communication/routing/EEHC_LEACH/EEHC_LEACH_Packet.msg.
//

#ifndef _EEHC_LEACH_PACKET_M_H_
#define _EEHC_LEACH_PACKET_M_H_

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0406
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include "RoutingPacket_m.h"
// }}

/**
 * Enum generated from <tt>src/node/communication/routing/EEHC_LEACH/EEHC_LEACH_Packet.msg:26</tt> by nedtool.
 * <pre>
 * enum EEHC_LEACH_Packet_Type
 * {
 * 
 *     EEHC_LEACH_ADV_PACKET = 1;
 *     EEHC_LEACH_JOIN_PACKET = 2;
 *     EEHC_LEACH_TDMA_PACKET = 3;
 *     EEHC_LEACH_DATA_PACKET = 4;
 * }
 * </pre>
 */
enum EEHC_LEACH_Packet_Type {
    EEHC_LEACH_ADV_PACKET = 1,
    EEHC_LEACH_JOIN_PACKET = 2,
    EEHC_LEACH_TDMA_PACKET = 3,
    EEHC_LEACH_DATA_PACKET = 4
};

/**
 * Class generated from <tt>src/node/communication/routing/EEHC_LEACH/EEHC_LEACH_Packet.msg:34</tt> by nedtool.
 * <pre>
 * packet EEHC_LEACH_Packet extends RoutingPacket
 * {
 *     int EEHC_LEACH_PacketKind @enum(EEHC_LEACH_Packet_Type);
 *     int schedule[];
 *     int numOfPackets;
 * }
 * </pre>
 */
class EEHC_LEACH_Packet : public ::RoutingPacket
{
  protected:
    int EEHC_LEACH_PacketKind_var;
    int *schedule_var; // array ptr
    unsigned int schedule_arraysize;
    int numOfPackets_var;

  private:
    void copy(const EEHC_LEACH_Packet& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const EEHC_LEACH_Packet&);

  public:
    EEHC_LEACH_Packet(const char *name=NULL, int kind=0);
    EEHC_LEACH_Packet(const EEHC_LEACH_Packet& other);
    virtual ~EEHC_LEACH_Packet();
    EEHC_LEACH_Packet& operator=(const EEHC_LEACH_Packet& other);
    virtual EEHC_LEACH_Packet *dup() const {return new EEHC_LEACH_Packet(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getEEHC_LEACH_PacketKind() const;
    virtual void setEEHC_LEACH_PacketKind(int EEHC_LEACH_PacketKind);
    virtual void setScheduleArraySize(unsigned int size);
    virtual unsigned int getScheduleArraySize() const;
    virtual int getSchedule(unsigned int k) const;
    virtual void setSchedule(unsigned int k, int schedule);
    virtual int getNumOfPackets() const;
    virtual void setNumOfPackets(int numOfPackets);
};

inline void doPacking(cCommBuffer *b, EEHC_LEACH_Packet& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, EEHC_LEACH_Packet& obj) {obj.parsimUnpack(b);}


#endif // ifndef _EEHC_LEACH_PACKET_M_H_
