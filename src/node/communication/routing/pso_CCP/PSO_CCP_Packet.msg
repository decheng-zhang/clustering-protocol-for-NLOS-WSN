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

enum PSO_CCP_Packet_Type
{
	PSO_CCP_ADV_PACKET 	= 1;		
	PSO_CCP_JOIN_PACKET = 2;		
	PSO_CCP_TDMA_PACKET = 3;		
	PSO_CCP_DATA_PACKET = 4;
	PSO_CCP_FIND_NEIGHBOURS_PACKET = 5;
};

packet PSO_CCP_Packet extends RoutingPacket
{	
	nodeData info;
	int numOfPackets;
	int PSO_CCP_PacketKind enum (PSO_CCP_Packet_Type);   
	int schedule[];		
};
