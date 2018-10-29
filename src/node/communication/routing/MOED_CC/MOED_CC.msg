cplusplus {{
#include "RoutingPacket_m.h"
}}

class RoutingPacket;

struct nodeData
{
	int id;
	int serialNo;
	double energy;
}


enum MOED_CC_Packet_Type
{
	MOED_CC_ADV_PACKET 	= 1;		
	MOED_CC_JOIN_PACKET = 2;		
	MOED_CC_TDMA_PACKET = 3;		
	MOED_CC_DATA_PACKET = 4;
	MOED_CC_FIND_NEIGHBOURS_PACKET = 5;
};

packet MOED_CC_Packet extends RoutingPacket
{	
	nodeData info;
	int numOfPackets;
	int MOED_CC_PacketKind enum (MOED_CC_Packet_Type);   
	int schedule[];		
};
