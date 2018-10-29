//
// Generated file, do not edit! Created by nedtool 4.6 from src/node/communication/routing/mopso_C/MOPSORoutingPacket.msg.
//

#ifndef _MOPSOROUTINGPACKET_M_H_
#define _MOPSOROUTINGPACKET_M_H_

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
 * Struct generated from src/node/communication/routing/mopso_C/MOPSORoutingPacket.msg:8 by nedtool.
 */
struct nodeData
{
    nodeData();
    int id;
    int serialNo;
    double energy;
};

void doPacking(cCommBuffer *b, nodeData& a);
void doUnpacking(cCommBuffer *b, nodeData& a);

/**
 * Enum generated from <tt>src/node/communication/routing/mopso_C/MOPSORoutingPacket.msg:15</tt> by nedtool.
 * <pre>
 * enum MOPSORoutingPacket_Type
 * {
 * 
 *     MOPSO_ROUTING_ADV_PACKET = 1;
 *     MOPSO_ROUTING_JOIN_PACKET = 2;
 *     MOPSO_ROUTING_TDMA_PACKET = 3;
 *     MOPSO_ROUTING_DATA_PACKET = 4;
 *     MOPSO_ROUTING_FIND_NEIGHBOURS_PACKET = 5;
 * }
 * </pre>
 */
enum MOPSORoutingPacket_Type {
    MOPSO_ROUTING_ADV_PACKET = 1,
    MOPSO_ROUTING_JOIN_PACKET = 2,
    MOPSO_ROUTING_TDMA_PACKET = 3,
    MOPSO_ROUTING_DATA_PACKET = 4,
    MOPSO_ROUTING_FIND_NEIGHBOURS_PACKET = 5
};

/**
 * Class generated from <tt>src/node/communication/routing/mopso_C/MOPSORoutingPacket.msg:24</tt> by nedtool.
 * <pre>
 * packet MOPSORoutingPacket extends RoutingPacket
 * {
 *     nodeData info;
 *     int numOfPackets;
 *     int MOPSORoutingPacketKind @enum(MOPSORoutingPacket_Type);
 *     int schedule[];
 * }
 * </pre>
 */
class MOPSORoutingPacket : public ::RoutingPacket
{
  protected:
    nodeData info_var;
    int numOfPackets_var;
    int MOPSORoutingPacketKind_var;
    int *schedule_var; // array ptr
    unsigned int schedule_arraysize;

  private:
    void copy(const MOPSORoutingPacket& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const MOPSORoutingPacket&);

  public:
    MOPSORoutingPacket(const char *name=NULL, int kind=0);
    MOPSORoutingPacket(const MOPSORoutingPacket& other);
    virtual ~MOPSORoutingPacket();
    MOPSORoutingPacket& operator=(const MOPSORoutingPacket& other);
    virtual MOPSORoutingPacket *dup() const {return new MOPSORoutingPacket(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual nodeData& getInfo();
    virtual const nodeData& getInfo() const {return const_cast<MOPSORoutingPacket*>(this)->getInfo();}
    virtual void setInfo(const nodeData& info);
    virtual int getNumOfPackets() const;
    virtual void setNumOfPackets(int numOfPackets);
    virtual int getMOPSORoutingPacketKind() const;
    virtual void setMOPSORoutingPacketKind(int MOPSORoutingPacketKind);
    virtual void setScheduleArraySize(unsigned int size);
    virtual unsigned int getScheduleArraySize() const;
    virtual int getSchedule(unsigned int k) const;
    virtual void setSchedule(unsigned int k, int schedule);
};

inline void doPacking(cCommBuffer *b, MOPSORoutingPacket& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, MOPSORoutingPacket& obj) {obj.parsimUnpack(b);}


#endif // ifndef _MOPSOROUTINGPACKET_M_H_
