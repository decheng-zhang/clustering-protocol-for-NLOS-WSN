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

enum MOPSORoutingPacket_Type
{
	MOPSO_ROUTING_ADV_PACKET 	= 1;		
	MOPSO_ROUTING_JOIN_PACKET = 2;		
	MOPSO_ROUTING_TDMA_PACKET = 3;		
	MOPSO_ROUTING_DATA_PACKET = 4;
	MOPSO_ROUTING_FIND_NEIGHBOURS_PACKET = 5;
};

packet MOPSORoutingPacket extends RoutingPacket
{	
	nodeData info;
	int numOfPackets;
	int MOPSORoutingPacketKind enum (MOPSORoutingPacket_Type);   
	int schedule[];		
};
