cplusplus {{
#include "RoutingPacket_m.h"
}}

class RoutingPacket;

struct nodeData9
{
	int id;
	int serialNo;
	double energy;
}

enum PSOPacket_Type
{
	PSO_ADV_PACKET 	= 1;		
	PSO_JOIN_PACKET = 2;		
	PSO_TDMA_PACKET = 3;		
	PSO_DATA_PACKET = 4;
	PSO_FIND_NEIGHBOURS_PACKET = 5;
};

packet PSOPacket extends RoutingPacket
{	
	nodeData9 info;
	int numOfPackets;
	int PSOPacketKind enum (PSOPacket_Type);   
	int schedule[];		
};
