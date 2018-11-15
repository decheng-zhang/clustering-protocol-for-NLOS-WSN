//
// Generated file, do not edit! Created by opp_msgc 4.5 from src/node/communication/routing/NSGA_II_CCP_3D_CO/NSGA_II_CCP_3D_CO.msg.
//

#ifndef _NSGA_II_CCP_3D_CO_M_H_
#define _NSGA_II_CCP_3D_CO_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0405
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif

// cplusplus {{
#include "RoutingPacket_m.h"
// }}



/**
 * Enum generated from <tt>src/node/communication/routing/NSGA_II_CCP_3D_CO/NSGA_II_CCP_3D_CO.msg</tt> by opp_msgc.
 * <pre>
 * enum NSGA_II_CCP_3D_CO_Packet_Type
 * {
 * 	NSGA_II_CCP_3D_CO_ADV_PACKET 	= 1;		
 * 	NSGA_II_CCP_3D_CO_JOIN_PACKET = 2;		
 * 	NSGA_II_CCP_3D_CO_TDMA_PACKET = 3;		
 * 	NSGA_II_CCP_3D_CO_DATA_PACKET = 4;
 * 	NSGA_II_CCP_3D_CO_FIND_NEIGHBOURS_PACKET = 5;
 * };
 * </pre>
 */
enum NSGA_II_CCP_3D_CO_Packet_Type {
    NSGA_II_CCP_3D_CO_ADV_PACKET = 1,
    NSGA_II_CCP_3D_CO_JOIN_PACKET = 2,
    NSGA_II_CCP_3D_CO_TDMA_PACKET = 3,
    NSGA_II_CCP_3D_CO_DATA_PACKET = 4,
    NSGA_II_CCP_3D_CO_FIND_NEIGHBOURS_PACKET = 5
};

/**
 * Struct generated from src/node/communication/routing/NSGA_II_CCP_3D_CO/NSGA_II_CCP_3D_CO.msg by opp_msgc.
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
 * Class generated from <tt>src/node/communication/routing/NSGA_II_CCP_3D_CO/NSGA_II_CCP_3D_CO.msg</tt> by opp_msgc.
 * <pre>
 * packet NSGA_II_CCP_3D_CO_Packet extends RoutingPacket
 * {	
 * 	nodeData info;
 * 	int numOfPackets;
 * 	int NSGA_II_CCP_3D_CO_PacketKind enum (NSGA_II_CCP_3D_CO_Packet_Type);   
 * 	int schedule[];		
 * };
 * </pre>
 */
class NSGA_II_CCP_3D_CO_Packet : public ::RoutingPacket
{
  protected:
    nodeData info_var;
    int numOfPackets_var;
    int NSGA_II_CCP_3D_CO_PacketKind_var;
    int *schedule_var; // array ptr
    unsigned int schedule_arraysize;

  private:
    void copy(const NSGA_II_CCP_3D_CO_Packet& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const NSGA_II_CCP_3D_CO_Packet&);

  public:
    NSGA_II_CCP_3D_CO_Packet(const char *name=NULL, int kind=0);
    NSGA_II_CCP_3D_CO_Packet(const NSGA_II_CCP_3D_CO_Packet& other);
    virtual ~NSGA_II_CCP_3D_CO_Packet();
    NSGA_II_CCP_3D_CO_Packet& operator=(const NSGA_II_CCP_3D_CO_Packet& other);
    virtual NSGA_II_CCP_3D_CO_Packet *dup() const {return new NSGA_II_CCP_3D_CO_Packet(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual nodeData& getInfo();
    virtual const nodeData& getInfo() const {return const_cast<NSGA_II_CCP_3D_CO_Packet*>(this)->getInfo();}
    virtual void setInfo(const nodeData& info);
    virtual int getNumOfPackets() const;
    virtual void setNumOfPackets(int numOfPackets);
    virtual int getNSGA_II_CCP_3D_CO_PacketKind() const;
    virtual void setNSGA_II_CCP_3D_CO_PacketKind(int NSGA_II_CCP_3D_CO_PacketKind);
    virtual void setScheduleArraySize(unsigned int size);
    virtual unsigned int getScheduleArraySize() const;
    virtual int getSchedule(unsigned int k) const;
    virtual void setSchedule(unsigned int k, int schedule);
};

inline void doPacking(cCommBuffer *b, NSGA_II_CCP_3D_CO_Packet& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, NSGA_II_CCP_3D_CO_Packet& obj) {obj.parsimUnpack(b);}


#endif // _NSGA_II_CCP_3D_CO_M_H_
