#include <lwip/sockets.h>
#include <lwip/err.h>
#include <lwip/sys.h>

#define	PORT			50000

RAW_U8 data_buffer[100];

static void tcp_server_thread(void *p_arg)
{
	struct sockaddr_in server_addr;
	struct sockaddr_in conn_addr;
	int sock_fd;				/* server socked */
	int sock_conn;			/* request socked */
	socklen_t addr_len;
	int err;
	int length;
	int count = 0;
	
	
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1) {
		Uart_Printf("failed to create sock_fd!\n");
		RAW_ASSERT(0);
	}
	
	raw_memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr =htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);
	
	err = bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (err < 0) {
		RAW_ASSERT(0);
	}
	
	err = listen(sock_fd, 1);
	if (err < 0) {
		RAW_ASSERT(0);
	}

	addr_len = sizeof(struct sockaddr_in);
	
	Uart_Printf("before accept!\n");
	sock_conn = accept(sock_fd, (struct sockaddr *)&conn_addr, &addr_len);
	Uart_Printf("after accept!\n");
	
	while (1) {
		raw_memset(data_buffer, 0, sizeof(data_buffer));
		
		length = recv(sock_conn, (unsigned int *)data_buffer, 20, 0);
		
		Uart_Printf("length received %d\n", length);
		Uart_Printf("received string: %s\n", data_buffer);
		Uart_Printf("received count: %d\n", count);

		send(sock_conn, "good", 5, 0);
	}
}

void tcp_server_init(void)
{
	sys_thread_new("tcp_server_thread",  tcp_server_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO - 1);
}
