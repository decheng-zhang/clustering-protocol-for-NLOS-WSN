cplusplus {{
#include "RoutingPacket_m.h"
}}

class RoutingPacket;

struct nodeData7
{
	int id;
	int serialNo;
	double energy;
}

enum PSO_HC_Packet_Type
{
	PSO_HC_DATA_PACKET = 1;
	PSO_HC_FIND_NEIGHBOURS_PACKET = 2;
};

packet PSO_HC_Packet extends RoutingPacket
{	
	nodeData7 info;
	int numOfPackets = 1;
	int PSO_HC_PacketKind enum (PSO_HC_Packet_Type);   
	int schedule[];		
};
