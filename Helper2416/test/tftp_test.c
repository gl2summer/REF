#if 0
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




RAW_TASK_OBJ 		tftp_client_task_obj;
#define  TFTP_CLIENT_TASK_STK_SIZE      4096
PORT_STACK tftp_client_stack[ TFTP_CLIENT_TASK_STK_SIZE];
struct netif tftp_client_netif;


extern err_t ethernetif_init(struct netif *netif);
extern void tftp_client_thread(void);


ip_addr_t tftp_client_ipaddr, tftp_client_netmask, tftp_client_gw;




extern void tftp_client_init(void);

static void
default_netif_add(void)
{
  IP4_ADDR(&tftp_client_gw, 192,168,0,1);
  IP4_ADDR(&tftp_client_ipaddr, 192,168,0,80);
  IP4_ADDR(&tftp_client_netmask, 255,255,255,0);

  netif_set_default(netif_add(&tftp_client_netif, &tftp_client_ipaddr, &tftp_client_netmask,
                              &tftp_client_gw, NULL, ethernetif_init, tcpip_input));
  netif_set_up(&tftp_client_netif);
}


void tftp_client_task(void *arg)
{
	tcpip_init(0, 0);                   
	default_netif_add();	

	tftp_client_init();

	raw_task_delete(&tftp_client_task_obj);

}



void tftp_client()
{
#if 0
	raw_task_create(&tftp_client_task_obj, (RAW_U8  *)"task1", 0,
	                         1, 0,  tftp_client_stack, 
	                         TFTP_CLIENT_TASK_STK_SIZE , tftp_client_task, 1); 
#endif
	tftp_client_task((void *)0);
}


#endif

#if 1
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

#include <rsh.h>

RAW_S32 tftp_routine(RAW_S8 * pcWriteBuffer, size_t xWriteBufferLen, const RAW_S8 * pcCommandString);

/* used to test tftp */
xCommandLineInput tftp = {
	"tftp",
	"tftp: test tftp\n",
	tftp_routine,
	0
};
xCommandLineInputListItem tftp_item;


RAW_TASK_OBJ 		tftp_client_task_obj;
#define  TFTP_CLIENT_TASK_STK_SIZE      4096
PORT_STACK tftp_client_stack[ TFTP_CLIENT_TASK_STK_SIZE];
struct netif tftp_client_netif;


extern err_t ethernetif_init(struct netif *netif);
extern void tftp_client_thread(void);
extern void tftp_client_init(void);
extern void tcp_client_init(void);
extern void tcp_server_init(void);
extern void udp_client_init(void);
extern void udp_server_init(void);

ip_addr_t tftp_client_ipaddr, tftp_client_netmask, tftp_client_gw;

static void default_netif_add(void)
{
  IP4_ADDR(&tftp_client_gw, 192,168,0,1);
  IP4_ADDR(&tftp_client_ipaddr, 192,168,0,80);
  IP4_ADDR(&tftp_client_netmask, 255,255,255,0);

  netif_set_default(netif_add(&tftp_client_netif, &tftp_client_ipaddr, &tftp_client_netmask,
                              &tftp_client_gw, NULL, ethernetif_init, tcpip_input));
  netif_set_up(&tftp_client_netif);
}


void tftp_client_task(void *arg)
{
	tcpip_init(0, 0);                   
	default_netif_add();	
tftp_client_init();
	//tcp_client_init();
//	tcp_server_init();
//udp_client_init();
//	udp_server_init();
	
	raw_task_delete(&tftp_client_task_obj);
}



int tftp_client()
{
#if 0
	raw_task_create(&tftp_client_task_obj, (RAW_U8  *)"task1", 0,
	                         1, 0,  tftp_client_stack, 
	                         TFTP_CLIENT_TASK_STK_SIZE , tftp_client_task, 1); 
#endif
	tftp_client_task((void *)0);
	
	return 0;
}


RAW_S32 tftp_routine(RAW_S8 * pcWriteBuffer, size_t xWriteBufferLen, const RAW_S8 * pcCommandString)
{
	Uart_Printf("\nstart test tftp ..............\n");
	tftp_client();
	
	return 1;
}
#endif
