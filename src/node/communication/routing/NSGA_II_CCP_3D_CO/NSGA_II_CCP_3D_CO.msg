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


enum NSGA_II_CCP_3D_CO_Packet_Type
{
	NSGA_II_CCP_3D_CO_ADV_PACKET 	= 1;		
	NSGA_II_CCP_3D_CO_JOIN_PACKET = 2;		
	NSGA_II_CCP_3D_CO_TDMA_PACKET = 3;		
	NSGA_II_CCP_3D_CO_DATA_PACKET = 4;
	NSGA_II_CCP_3D_CO_FIND_NEIGHBOURS_PACKET = 5;
};

packet NSGA_II_CCP_3D_CO_Packet extends RoutingPacket
{	
	nodeData info;
	int numOfPackets;
	int NSGA_II_CCP_3D_CO_PacketKind enum (NSGA_II_CCP_3D_CO_Packet_Type);   
	int schedule[];		
};
