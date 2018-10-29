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


enum EA_CH_3D_Packet_Type
{
	EA_CH_3D_ADV_PACKET 	= 1;		
	EA_CH_3D_JOIN_PACKET = 2;		
	EA_CH_3D_TDMA_PACKET = 3;		
	EA_CH_3D_DATA_PACKET = 4;
	EA_CH_3D_FIND_NEIGHBOURS_PACKET = 5;
};

packet EA_CH_3D_Packet extends RoutingPacket
{	
	nodeData info;
	int numOfPackets;
	int EA_CH_3D_PacketKind enum (EA_CH_3D_Packet_Type);   
	int schedule[];		
};
