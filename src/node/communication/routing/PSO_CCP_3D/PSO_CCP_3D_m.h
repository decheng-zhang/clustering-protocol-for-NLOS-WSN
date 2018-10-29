//
// Generated file, do not edit! Created by nedtool 4.6 from src/node/communication/routing/PSO_CCP_3D/PSO_CCP_3D.msg.
//

#ifndef _PSO_CCP_3D_M_H_
#define _PSO_CCP_3D_M_H_

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
 * Struct generated from src/node/communication/routing/PSO_CCP_3D/PSO_CCP_3D.msg:8 by nedtool.
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
 * Enum generated from <tt>src/node/communication/routing/PSO_CCP_3D/PSO_CCP_3D.msg:16</tt> by nedtool.
 * <pre>
 * enum PSO_CCP_3D_Packet_Type
 * {
 * 
 *     PSO_CCP_3D_ADV_PACKET = 1;
 *     PSO_CCP_3D_JOIN_PACKET = 2;
 *     PSO_CCP_3D_TDMA_PACKET = 3;
 *     PSO_CCP_3D_DATA_PACKET = 4;
 *     PSO_CCP_3D_FIND_NEIGHBOURS_PACKET = 5;
 * }
 * </pre>
 */
enum PSO_CCP_3D_Packet_Type {
    PSO_CCP_3D_ADV_PACKET = 1,
    PSO_CCP_3D_JOIN_PACKET = 2,
    PSO_CCP_3D_TDMA_PACKET = 3,
    PSO_CCP_3D_DATA_PACKET = 4,
    PSO_CCP_3D_FIND_NEIGHBOURS_PACKET = 5
};

/**
 * Class generated from <tt>src/node/communication/routing/PSO_CCP_3D/PSO_CCP_3D.msg:25</tt> by nedtool.
 * <pre>
 * packet PSO_CCP_3D_Packet extends RoutingPacket
 * {
 *     nodeData info;
 *     int numOfPackets;
 *     int PSO_CCP_3D_PacketKind @enum(PSO_CCP_3D_Packet_Type);
 *     int schedule[];
 * }
 * </pre>
 */
class PSO_CCP_3D_Packet : public ::RoutingPacket
{
  protected:
    nodeData info_var;
    int numOfPackets_var;
    int PSO_CCP_3D_PacketKind_var;
    int *schedule_var; // array ptr
    unsigned int schedule_arraysize;

  private:
    void copy(const PSO_CCP_3D_Packet& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const PSO_CCP_3D_Packet&);

  public:
    PSO_CCP_3D_Packet(const char *name=NULL, int kind=0);
    PSO_CCP_3D_Packet(const PSO_CCP_3D_Packet& other);
    virtual ~PSO_CCP_3D_Packet();
    PSO_CCP_3D_Packet& operator=(const PSO_CCP_3D_Packet& other);
    virtual PSO_CCP_3D_Packet *dup() const {return new PSO_CCP_3D_Packet(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual nodeData& getInfo();
    virtual const nodeData& getInfo() const {return const_cast<PSO_CCP_3D_Packet*>(this)->getInfo();}
    virtual void setInfo(const nodeData& info);
    virtual int getNumOfPackets() const;
    virtual void setNumOfPackets(int numOfPackets);
    virtual int getPSO_CCP_3D_PacketKind() const;
    virtual void setPSO_CCP_3D_PacketKind(int PSO_CCP_3D_PacketKind);
    virtual void setScheduleArraySize(unsigned int size);
    virtual unsigned int getScheduleArraySize() const;
    virtual int getSchedule(unsigned int k) const;
    virtual void setSchedule(unsigned int k, int schedule);
};

inline void doPacking(cCommBuffer *b, PSO_CCP_3D_Packet& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, PSO_CCP_3D_Packet& obj) {obj.parsimUnpack(b);}


#endif // ifndef _PSO_CCP_3D_M_H_
