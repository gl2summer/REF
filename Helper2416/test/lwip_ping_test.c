
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <raw_api.h>

#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/tcpip.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#include "netif/ppp_oe.h"


RAW_TASK_OBJ 		lwip_main_task_obj;
#define  LWIP_MAIN_TASK_STK_SIZE      4096
PORT_STACK lwip_main_stack[ LWIP_MAIN_TASK_STK_SIZE];

struct netif test_netif;


extern err_t
ethernetif_init(struct netif *netif);

ip_addr_t test_ipaddr, test_netmask, test_gw;

static void
default_netif_add(void)
{
  IP4_ADDR(&test_gw, 192,168,0,1);
  IP4_ADDR(&test_ipaddr, 192,168,0,80);
  IP4_ADDR(&test_netmask, 255,255,255,0);

  netif_set_default(netif_add(&test_netif, &test_ipaddr, &test_netmask,
                              &test_gw, NULL, ethernetif_init, tcpip_input));
  netif_set_up(&test_netif);
}


extern void ping_init(void);
void lwip_main_task(void *arg)
{
	tcpip_init(0, 0);                   
	default_netif_add();	

	ping_init();

	raw_task_delete(&lwip_main_task_obj);

}



void lwip_ping_test(void)
{

	raw_task_create(&lwip_main_task_obj, (RAW_U8  *)"task1", 0,
	                         1, 0,  lwip_main_stack, 
	                         LWIP_MAIN_TASK_STK_SIZE , lwip_main_task, 1); 
}

