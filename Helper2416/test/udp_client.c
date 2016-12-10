/*
 * send UDP packet to PC
 * remote IP: 192.168.0.100 or INADDR_BROADCAST
 */

#include <lwip/sockets.h>
#include <lwip/err.h>
#include <lwip/sys.h>

#define	PORT			50000

char udp_msg[] = "this is a UDP test package";

static void udp_client_thread(void *p_arg)
{
	struct sockaddr_in client_addr;
	int sock_fd;				/* client socked */
	int err;
	int count = 0;
	
	err = err;
	
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_fd == -1) {
		Uart_Printf("failed to create sock_fd!\n");
		RAW_ASSERT(0);
	}
	
	raw_memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	//client_addr.sin_addr.s_addr = INADDR_BROADCAST;		
	client_addr.sin_addr.s_addr = inet_addr("192.168.0.100");
	client_addr.sin_port = htons(PORT);
	
	while (1) {
		err = sendto(sock_fd, (char *)udp_msg, sizeof(udp_msg), 0, \
						(struct sockaddr *)&client_addr, sizeof(client_addr));
		count++;
		Uart_Printf("-------------------send count %d-------------------\n", count);
		raw_sleep(100);
	}
}

void udp_client_init(void)
{
	sys_thread_new("udp_client_thread",  udp_client_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO - 1);
}

