#pragma once

#include <winsock.h>
#include <string>
#include <stdint.h>

#pragma comment(lib, "Wsock32.lib")

#define LOCAL_IP		"127.0.0.1"
#define GROUP_IP		"224.5.5.5"
#define PORT			43002
#define RECIEVE_SIZE	1024 * 100

class UDPMulticastClient
{
public:
	UDPMulticastClient(const std::string& ip_address, int32_t port);
	~UDPMulticastClient();

	int32_t GetUDPData(char* data_buf) const;

	int32_t GetSocket() const { return socket_; }
private:
	std::string ip_address_;

	int32_t port_;
	
	int32_t socket_;

	struct sockaddr_in local_sock_;

	struct ip_mreq group_;

	bool is_WSA_clean_up_;

	bool is_socket_close_;

	bool GetSockDGRAM(int32_t port);

	bool InitSocket();
};

