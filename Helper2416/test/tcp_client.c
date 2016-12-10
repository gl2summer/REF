#include <lwip/sockets.h>
#include <lwip/err.h>
#include <lwip/sys.h>

#define	PORT		50000

char msg[] = "hello, you are connected!\n";

static void tcp_client_thread(void * arg)
{
	struct sockaddr_in server_addr;
	int sock_fd;
	
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1) {
		Uart_Printf("failed to create sock_fd!\n");
		RAW_ASSERT(0);
	}
	
	raw_memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("192.168.0.100");
	server_addr.sin_port = htons(PORT);
	
	connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));

	while (1) {
		send(sock_fd, (char *)msg, sizeof(msg), 0);
		raw_sleep(100);
	}
	
//	close(sock_fd);
}


void tcp_client_init(void)
{
	sys_thread_new("tcp_client", tcp_client_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}


