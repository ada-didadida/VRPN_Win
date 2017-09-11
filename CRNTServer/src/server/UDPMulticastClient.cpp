#include "UDPMulticastClient.h"

UDPMulticastClient::UDPMulticastClient(const std::string& ip_address, int32_t port) :
	ip_address_(ip_address)
	,port_(port)
	,is_WSA_clean_up_(false)
	,is_socket_close_(false)
{
	if (InitSocket())
	{
		GetSockDGRAM(port);
	}
}

UDPMulticastClient::~UDPMulticastClient()
{
	if(!is_WSA_clean_up_)
	{
		WSACleanup();
	}

	if(!is_socket_close_)
	{
		closesocket(socket_);
	}	
}

int32_t UDPMulticastClient::GetUDPData(char* data_buf) const
{
	//socket存在
	if(!is_WSA_clean_up_ && !is_socket_close_)
	{		
		struct sockaddr_in from;
		auto from_len = sizeof(from);	

		auto res = recvfrom(socket_, data_buf, RECIEVE_SIZE, 0, reinterpret_cast<struct sockaddr*>(&from), reinterpret_cast<int*>(&from_len));
		
		return res;
	}

	return -1;
}

bool UDPMulticastClient::InitSocket()
{
	WSADATA WsaData;
	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0)
	{
		return false;
	}

	if(LOBYTE(WsaData.wVersion) != 2 || HIBYTE(WsaData.wHighVersion) !=2)
	{
		WSACleanup();
		is_WSA_clean_up_ = true;
		return false;
	}

	return true;
}

bool UDPMulticastClient::GetSockDGRAM(int32_t port)
{
	//获取UDP Socket套接字
	socket_ = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_ < 0)
	{
		printf("Opening datagram socket error");
		return false;
	}
	printf("Opening datagram socket....OK.\n");

	//支持多播
	auto reuse = 1;
	if(setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&reuse), sizeof(reuse)) < 0)
	{
		printf("Setting SO_REUSEADDR error");
		closesocket(socket_);
		is_socket_close_ = true;
		return false;
	}
	printf("Setting SO_REUSEADDR...OK.\n");

	//关联相应地址和套接字
	memset(reinterpret_cast<char*>(&local_sock_), 0, sizeof(local_sock_));
	local_sock_.sin_family = AF_INET;
	local_sock_.sin_port = htons(port);
	local_sock_.sin_addr.s_addr = INADDR_ANY;
	if(bind(socket_, reinterpret_cast<struct sockaddr*>(&local_sock_), sizeof(local_sock_)))
	{
		printf("Binding datagram socket error");
		closesocket(socket_);
		is_socket_close_ = true;
		return false;
	}
	printf("Binding datagram socket...OK.\n");

	//加入多播组
	group_.imr_multiaddr.s_addr = inet_addr(ip_address_.c_str());
	group_.imr_interface.s_addr = htonl(INADDR_ANY);
	if(setsockopt(socket_, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<char*>(&group_), sizeof(group_)) < 0)
	{
		printf("Adding multicast group error");
		closesocket(socket_);
		is_socket_close_ = true;
		return false;
	}
	printf("Adding multicast group...OK.\n");

	//设置超时时间，防止阻塞
	struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	if(setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&tv), sizeof(tv)) < 0)
	{
		printf("Add Timeval error");
		closesocket(socket_);
		is_socket_close_ = true;
		return false;
	}

	return true;
}
