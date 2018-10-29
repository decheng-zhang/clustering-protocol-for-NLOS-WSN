cplusplus {{
#include "RoutingPacket_m.h"
}}

class RoutingPacket;

struct nodeData5
{
	int id;
	int serialNo;
	double energy;
}

enum PSOClusteringPacket_Type
{
	PSO_CLUSTERING_DATA_PACKET = 1;
	PSO_CLUSTERING_FIND_NEIGHBOURS_PACKET = 2;
};

packet PSOClusteringPacket extends RoutingPacket
{	
	nodeData5 info;
	double numOfPackets = 1.0;
	int PSOClusteringPacketKind enum (PSOClusteringPacket_Type);   
	int schedule[];		
};
