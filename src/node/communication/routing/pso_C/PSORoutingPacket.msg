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

enum PSORoutingPacket_Type
{
	PSO_ROUTING_ADV_PACKET 	= 1;		
	PSO_ROUTING_JOIN_PACKET = 2;		
	PSO_ROUTING_TDMA_PACKET = 3;		
	PSO_ROUTING_DATA_PACKET = 4;
	PSO_ROUTING_FIND_NEIGHBOURS_PACKET = 5;
};

packet PSORoutingPacket extends RoutingPacket
{	
	nodeData info;
	int numOfPackets;
	int PSORoutingPacketKind enum (PSORoutingPacket_Type);   
	int schedule[];		
};
