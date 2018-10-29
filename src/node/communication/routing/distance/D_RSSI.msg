cplusplus {{
#include "RoutingPacket_m.h"
}}

class RoutingPacket;

enum D_RSSI_Type
{
	D_RSSI_ADV_PACKET =  1;		
};

packet   D_RSSI_Packet extends RoutingPacket
{	
	int D_RSSI_PacketKind enum (D_RSSI_Type);   
	int schedule[];		
};
