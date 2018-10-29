cplusplus {{
#include "RoutingPacket_m.h"
}}

class RoutingPacket;

struct nodeData1
{
	int id;
	int serialNo;
	double energy;
}

enum LeachCentralizedRoutingPacket_Type
{	
	LEACHCENTRALIZED_ROUTING_DATA_PACKET = 1;
	LEACHCENTRALIZED_FIND_NEIGHBOURS_PACKET = 2;
};

packet LeachCentralizedRoutingPacket extends RoutingPacket
{	
	nodeData1 info;
	int numOfPackets;

	int LeachCentralizedRoutingPacketKind enum (LeachCentralizedRoutingPacket_Type);   
	int schedule[];		
};
