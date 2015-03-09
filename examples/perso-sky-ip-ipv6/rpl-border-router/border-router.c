/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */
/**
 * \file
 *         border-router
 * \author
 *         Niclas Finne <nfi@sics.se>
 *         Joakim Eriksson <joakime@sics.se>
 *         Nicolas Tsiftes <nvt@sics.se>
 */

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/uip.h"
#include "net/uip-ds6.h"

#include "net/netstack.h"
#include "dev/slip.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "dev/button-sensor.h"
#include "../nodes_topology.h"

// #include "wget.h"

#define DEBUG 0
#include "net/uip-debug.h"

PROCESS(border_router_process, "Border router process");

#if WEBSERVER==0
  /* No webserver */
  AUTOSTART_PROCESSES(&border_router_process);
#else
  /* Use an external webserver application */
  #include "webserver-nogui.h"
  AUTOSTART_PROCESSES(&border_router_process,&webserver_nogui_process);
#endif

static uip_ipaddr_t prefix;
static uint8_t prefix_set;


/*---------------------------------------------------------------------------*/
static void
print_local_addresses(void)
{
  int i;
  uint8_t state;

  PRINTA("Server IPv6 addresses:\n");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      PRINTA(" ");
      uip_debug_ipaddr_print(&uip_ds6_if.addr_list[i].ipaddr);
      PRINTA("\n");
    }
  }
}
/*---------------------------------------------------------------------------*/
void
request_prefix(void)
{
  /* mess up uip_buf with a dirty request... */
  uip_buf[0] = '?';
  uip_buf[1] = 'P';
  uip_len = 2;
  slip_send();
  uip_len = 0;
}
/*---------------------------------------------------------------------------*/
void
set_prefix_64(uip_ipaddr_t *prefix_64)
{
  // Don't need to set a prefix, Already set by the non defining of RPL !

  // uip_ipaddr_t ipaddr;
  // memcpy(&prefix, prefix_64, 16);
  // memcpy(&ipaddr, prefix_64, 16);
  prefix_set = 1;
  // uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  // uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
}
/*---------------------------------------------------------------------------*/
/* 
  # Init the Static routing for the node 
    - Add the neighbors to the neighbor cache 
    - Define the MAC address && Local add ip of the hops
    - Add the routes to the neighbor passing throw one of the hops (next or precedent)
*/
void
init_routes()
{
  uip_lladdr_t lladdr_hop;         // next/precedent Hop MAC
  uip_ipaddr_t local_ipaddr_hop;   // next/precedent Local-IP 
  
  uip_lladdr_t lladdr_node;        // node (neighbor or distant) MAC
  uip_ipaddr_t local_ipaddr;       // node (neighbor or distant) Local-IP 
  uip_ipaddr_t global_ipaddr;      // node (neighbor or distant) Global-IP
  int i=0;
  // Copy the actual MAC Adresse
  // and Some initialisations
  memcpy(&lladdr_hop, &uip_lladdr, sizeof(uip_lladdr_t));
  memcpy(&lladdr_node, &uip_lladdr, sizeof(uip_lladdr_t));
  uip_ip6addr(&local_ipaddr_hop, 0xfe80, 0, 0, 0, 0, 0, 0, 0);
  uip_ip6addr(&local_ipaddr, 0xfe80, 0, 0, 0, 0, 0, 0, 0);
  uip_ip6addr(&global_ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);

  // #### NEXT NODES #### //

  /*
   * °1 next node
  */  
  // Also add it as the next hop for all routes

  // Define the link-layer (MAC) address of the next-hop node (lladdr)
  memcpy(&lladdr_hop.addr, &lladdr_node_1, 8*sizeof(uint8_t));
  
  uip_ds6_set_addr_iid(&local_ipaddr_hop, &lladdr_hop);
  uip_ds6_set_addr_iid(&global_ipaddr, &lladdr_hop);

  // Add the node to the neighbor Cache 
  if(uip_ds6_nbr_add(&local_ipaddr_hop, &lladdr_hop, 1, NBR_REACHABLE) == NULL){
    PRINTF("init_routes: add neighboor fail\n");
  }

  // Add the route
  if(uip_ds6_route_add(&global_ipaddr, 128, &local_ipaddr_hop) == NULL){
    PRINTF("init_routes: Add route failed");
  }

  /*
   * °2 next node
  */
  memcpy(&lladdr_node.addr, &lladdr_node_2, 8*sizeof(uint8_t));

  uip_ds6_set_addr_iid(&local_ipaddr, &lladdr_node);
  uip_ds6_set_addr_iid(&global_ipaddr, &lladdr_node);

  // Add the next-hop node to the neighbor Cache 
  if(uip_ds6_nbr_add(&local_ipaddr, &lladdr_node, 1, NBR_REACHABLE) == NULL){
    PRINTF("init_routes: add neighboor fail\n");
  }


  // Add the route
  if(uip_ds6_route_add(&global_ipaddr, 128, &local_ipaddr_hop) == NULL){
    PRINTF("init_routes: Add route failed");
  }

  /*
   * °3 next node  
  */ 
  memcpy(&lladdr_node.addr, &lladdr_node_3, 8*sizeof(uint8_t));

  uip_ds6_set_addr_iid(&local_ipaddr, &lladdr_node);
  uip_ds6_set_addr_iid(&global_ipaddr, &lladdr_node);

  // Add the next-hop node to the neighbor Cache 
  if(uip_ds6_nbr_add(&local_ipaddr, &lladdr_node, 1, NBR_REACHABLE) == NULL){
    PRINTF("init_routes: add neighboor fail\n");
  }

  // Add the route
  if(uip_ds6_route_add(&global_ipaddr, 128, &local_ipaddr_hop) == NULL){
    PRINTF("init_routes: Add route failed");
  }


  // ## STATIC ROUTING END ## //
  PRINTF("init_routes: Static Routing End\n");
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(border_router_process, ev, data)
{
  static struct etimer et;

  PROCESS_BEGIN();

/* While waiting for the prefix to be sent through the SLIP connection, the future
 * border router can join an existing DAG as a parent or child, or acquire a default 
 * router that will later take precedence over the SLIP fallback interface.
 * Prevent that by turning the radio off until we are initialized as a DAG root.
 */
  prefix_set = 0;
  NETSTACK_MAC.off(0);

  PROCESS_PAUSE();

  #if UIP_CONF_IPV6_RPL
    PRINTF("RPL ACTIVATED\n");
  #else
    PRINTF("RPL DESACTIVATED\n");
  #endif 
 
  /* Request prefix until it has been received */
  while(!prefix_set) {
    etimer_set(&et, CLOCK_SECOND);
    request_prefix();
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  }

  /* Now turn the radio on, but disable radio duty cycling.
   * Since we are the Border router, reception delays would constrain mesh throughbut.
   */
  NETSTACK_MAC.off(1);

  // Init static routes for this node
  init_routes();

  #if DEBUG || 1
    print_local_addresses();
  #endif

  //-------------------
  PRINTF("Border router IPv6 started\n");
  
  SENSORS_ACTIVATE(button_sensor);


  while(1) {
    // Waiting for a event, don't care which
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
    printf("BUTTON PUSHED\n");
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
