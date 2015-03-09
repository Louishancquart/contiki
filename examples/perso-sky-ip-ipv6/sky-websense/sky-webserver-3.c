/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
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
 *         Multi-hop webserver for the Tmote Sky
 * \author
 *         Adam Dunkels <adam@sics.se>
 */
#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "dev/button-sensor.h"
#include "../nodes_topology.h"

#include "webserver-nogui.h"

#define DEBUG DEBUG_FULL
#include "net/uip-debug.h"

/*---------------------------------------------------------------------------*/
PROCESS(web_sense_process, "IPv6 web Sense");

#if WEBSERVER==0
  /* No webserver */
  AUTOSTART_PROCESSES(&web_sense_process);
#else
  /* Use an external webserver application */
  #include "webserver-nogui.h"
  AUTOSTART_PROCESSES(&web_sense_process,&webserver_nogui_process);
#endif
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

  // #### PRECEDENT NODES #### //
  /*
   * °1 precedent node
  */  
  // Also add it as the precedent hop for all backward routes 

  // Define the link-layer (MAC) address of the precedent-hop node (lladdr)
  memcpy(&lladdr_hop.addr, &lladdr_node_2, 8*sizeof(uint8_t));

  uip_ds6_set_addr_iid(&local_ipaddr_hop, &lladdr_hop);
  uip_ds6_set_addr_iid(&global_ipaddr, &lladdr_hop);

  // Add the node to the neighbor Cache 
  if(uip_ds6_nbr_add(&local_ipaddr_hop, &lladdr_hop, 1, NBR_REACHABLE) == NULL){
    PRINTF("init_routes: add neighboor fail\n");
  }

  // Add the precedent-hop route as a "Default route"
  if(uip_ds6_defrt_add(&local_ipaddr_hop, 0) == NULL){
    PRINTF("init_routes: Add Default route failed");
  }

  /*
   * °2 precedent node
  */
  memcpy(&lladdr_node.addr, &lladdr_node_1, 8*sizeof(uint8_t));

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
   * °3 precedent node
  */
  memcpy(&lladdr_node.addr, &lladdr_gw, 8*sizeof(uint8_t));

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

  // #### NEXT NODES #### //
  // No next nodes

  // ## STATIC ROUTING END ## //
  PRINTF("init_routes: Static Routing End\n");
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(web_sense_process, ev, data)
{
  /////////////////////////////////
  PROCESS_BEGIN();

  #if UIP_CONF_IPV6_RPL
      PRINTF("RPL ACTIVATED\n");
  #else
      PRINTF("RPL DESACTIVATED\n");
  #endif 

  // Init static routes for this node

  init_routes();

  #if DEBUG || 1
    print_local_addresses();
  #endif

  //-------------------
  PRINTF("SKY Webserver IPv6 n_4 Started\n");

  SENSORS_ACTIVATE(button_sensor);

  while(1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);  // Waiting for a event, don't care which
    PRINTF("Button pushed\n");
  }

  PROCESS_END();

}
/*---------------------------------------------------------------------------*/

