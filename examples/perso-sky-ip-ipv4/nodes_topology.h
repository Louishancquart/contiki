#ifndef NODE_TOPOLOGY
#define NODE_TOPOLOGY


/**
	Node topology for a Ipv4 Static Network
**/

/**
	Gateway <---> NODE_1 <---> NODE_2 <---> NODE_3
**/


// NOTA: In IPv4, IP Adresses are always on 4 Bytes. The two MSB are always "172" & "16"
uint8_t ipaddr_gw[]			= 	{1, 0};		
uint8_t ipaddr_node_1[] 	=	{2, 0};	
uint8_t ipaddr_node_2[] 	=	{3, 0};
uint8_t ipaddr_node_3[] 	= 	{4, 0};



#endif /* NODE_TOPOLOGY */

