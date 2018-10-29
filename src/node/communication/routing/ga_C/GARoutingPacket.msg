cplusplus {{
#include "RoutingPacket_m.h"
}}

class RoutingPacket;

struct nodeData2
{
	int id;
	int serialNo;
	double energy;
}

enum GARoutingPacket_Type
{
	GA_ROUTING_ADV_PACKET 	= 1;		
	GA_ROUTING_JOIN_PACKET = 2;		
	GA_ROUTING_TDMA_PACKET = 3;		
	GA_ROUTING_DATA_PACKET = 4;
	GA_ROUTING_FIND_NEIGHBOURS_PACKET = 5;
};

packet GARoutingPacket extends RoutingPacket
{	
	nodeData2 info;
	int numOfPackets;
	int GARoutingPacketKind enum (GARoutingPacket_Type);   
	int schedule[];		
};
