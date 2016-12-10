/*
  *	receive UDP packet from PC
  *   local server IP:192.168.0.80 or INADDR_ANY
  */

#include <lwip/sockets.h>
#include <lwip/err.h>
#include <lwip/sys.h>

#define	PORT			50000

static RAW_U32 udp_msg[100];

static void udp_server_thread(void *p_arg)
{
	struct sockaddr_in server_addr;
	int sock_fd;				/* server socked */
	int err;
	int count = 0;
	
	
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_fd == -1) {
		Uart_Printf("failed to create sock_fd!\n");
		RAW_ASSERT(0);
	}
	
	raw_memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;		
	//server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_addr.s_addr = inet_addr("192.168.0.80");
	server_addr.sin_port = htons(PORT);
	
	err = bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (err == -1) {
		RAW_ASSERT(0);
	}
	
	while (1) {
		raw_memset(udp_msg, 0, sizeof(udp_msg));
		err = recv(sock_fd, (RAW_U8 *)udp_msg, sizeof(udp_msg), 0);
		
		Uart_Printf("receive msg: %s", udp_msg);
		
		count++;
	}
}

void udp_server_init(void)
{
	sys_thread_new("udp_server_thread",  udp_server_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO - 1);
}

