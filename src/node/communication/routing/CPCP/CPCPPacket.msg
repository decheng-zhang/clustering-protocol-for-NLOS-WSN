cplusplus {{
#include "RoutingPacket_m.h"
}}

class RoutingPacket;


enum CPCPPacket_Type
{
	CPCP_ADV_PACKET 	= 1;		
	CPCP_JOIN_PACKET 	= 2;		
	CPCP_TDMA_PACKET 	= 3;		
	CPCP_DATA_PACKET 	= 4;
	CPCP_ACTIVATION_PACKET  = 5;
	CPCP_FIND_NEIGHBOURS_PACKET = 6;
};

packet CPCPPacket extends RoutingPacket
{	
	int CPCPPacketKind enum (CPCPPacket_Type);   
	int schedule[];		
	int numOfPackets;
};
