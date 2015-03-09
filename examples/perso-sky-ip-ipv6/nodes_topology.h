#ifndef NODE_TOPOLOGY
#define NODE_TOPOLOGY


/**
	Node topology for a Ipv6 Static Network
**/

/**
	Gateway <---> NODE_1 <---> NODE_2 <---> NODE_3
**/


// NOTA: 	In IPv6, For static routing, we use here the Link-Layer addresses of the node.
// 			The static routing function will deduct from the link-layer address

uint8_t lladdr_gw[]			= 	{0, 18, 116, 1, 0, 1, 1, 1};		
uint8_t lladdr_node_1[] 	=	{0, 18, 116, 2, 0, 2, 2, 2};
uint8_t lladdr_node_2[] 	=	{0, 18, 116, 3, 0, 3, 3, 3};
uint8_t lladdr_node_3[] 	= 	{0, 18, 116, 4, 0, 4, 4, 4};



#endif /* NODE_TOPOLOGY */
