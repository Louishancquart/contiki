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
#include "webserver-nogui.h"

#include "contiki.h"
#include "net/uip-over-mesh.h"
#include "net/rime/route.h"
#include "dev/button-sensor.h"

#include "nodes_topology.h"

#define DEBUG DEBUG_FULL
#include "net/uip-debug.h"

/*---------------------------------------------------------------------------*/
PROCESS(web_sense_process, "IPv4 web Sense");
AUTOSTART_PROCESSES(&webserver_nogui_process, &web_sense_process);
/*---------------------------------------------------------------------------*/
void
init_routes()
{
  static rimeaddr_t ldest;  // Destination node IPv4
  static rimeaddr_t lhop;   // Nexthop node IPv4

  // Neighbors
  // Every node have only one neighbor
  ldest.u8[0] = ipaddr_gw[0];
  ldest.u8[1] = ipaddr_gw[1];

  uip_over_mesh_set_gateway(&ldest);


//----------PRECEDENT--------------------//
  // Precedent-hop address
  lhop.u8[0] = ipaddr_node_2[0];
  lhop.u8[1] = ipaddr_node_2[1];

  // Destination address
  // Add distant node
  ldest.u8[0] = ipaddr_node_2[0];
  ldest.u8[1] = ipaddr_node_2[1];

  //--- Add the next-hop route ---//
  route_add(&ldest, &lhop, 0, 0);

  // Destination address
  // Add distant node
  ldest.u8[0] = ipaddr_node_1[0];
  ldest.u8[1] = ipaddr_node_1[1];

  //--- Add the next-hop route ---//
  route_add(&ldest, &lhop, 0, 0);

  // Destination address
  // Add distant node
  ldest.u8[0] = ipaddr_gw[0];
  ldest.u8[1] = ipaddr_gw[1];

  //--- Add the next-hop route ---//
  route_add(&ldest, &lhop, 0, 0);



//----------NEXT--------------------//


PRINTF("init_routes: Static Routing End\n");


}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(web_sense_process, ev, data)
{
  PROCESS_BEGIN();

  //-------------------------
  // Check if IPV Routing (AODV) is desactivated
  #ifdef AODV_COMPLIANCE
    PRINTF("ADOV ACTIVATED\n");
  #else
    PRINTF("AODV DESACTIVATED\n");
  #endif
  //-------------------------

  init_routes();

  SENSORS_ACTIVATE(button_sensor);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);  // Waiting for a Button event
    PRINTF("Button pushed\n");
    // light1[sensors_pos] = get_light();;
    // temperature[sensors_pos] = get_temp();
    // sensors_pos = (sensors_pos + 1) % HISTORY;
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

