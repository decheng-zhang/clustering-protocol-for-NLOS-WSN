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


enum LOS_CCP_3D_Packet_Type
{
	LOS_CCP_3D_ADV_PACKET 	= 1;		
	LOS_CCP_3D_JOIN_PACKET = 2;		
	LOS_CCP_3D_TDMA_PACKET = 3;		
	LOS_CCP_3D_DATA_PACKET = 4;
	LOS_CCP_3D_FIND_NEIGHBOURS_PACKET = 5;
};

packet LOS_CCP_3D_Packet extends RoutingPacket
{	
	nodeData info;
	int numOfPackets;
	int LOS_CCP_3D_PacketKind enum (LOS_CCP_3D_Packet_Type);   
	int schedule[];		
};
