#include<sys/types.h> 
#include<sys/socket.h> 
#include<unistd.h> 
#include<netinet/in.h> 
#include<arpa/inet.h> 
#include<stdio.h> 
#include<stdlib.h> 
#include<errno.h> 
#include<netdb.h> 
#include<stdarg.h> 
#include<string.h>
#include<iostream>
#include<pthread.h>
#include<string>
#include<sys/syscall.h>   

using namespace std;
#define BUFFER_SIZE 1024
string g_ip;
unsigned short g_port;
pid_t gettid()
{      
	return syscall(SYS_gettid); 
} 
inline string IpU32ToString(unsigned ipv4)
{
    char buf[INET_ADDRSTRLEN] = {0};
    struct in_addr in;
    in.s_addr = ipv4;

    if(inet_ntop(AF_INET, &in ,buf, sizeof(buf)))
    {
        return string(buf);
    }
    else
    {
        return string("");
    }

}


void start_udp_server(string &ip, unsigned port){
	cout << "ip: " << ip << " " << port << " " << gettid() << endl;
	struct sockaddr_in server_addr;  
	socklen_t server_addr_length = sizeof(server_addr); 
	//bzero(&server_addr, sizeof(server_addr)); 
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.s_addr = inet_addr(ip.c_str()); 
	server_addr.sin_port = htons(port); 
	int server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0); 
	if(server_socket_fd < 0) 
	{ 
		perror("Create Socket Failed:"); 
		exit(1); 
	}
	int opt_val = 1;
	setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEPORT, &opt_val, sizeof(opt_val));
	
 	if(-1 == (bind(server_socket_fd,(struct sockaddr*)&server_addr,sizeof(server_addr)))) 
 	{ 
  		perror("Server Bind Failed:"); 
  		exit(1); 
 	}
	while(1){  
  		struct sockaddr_in client_addr; 
  		socklen_t client_addr_length = sizeof(client_addr); 
  
  		char buffer[BUFFER_SIZE]; 
  		bzero(buffer, BUFFER_SIZE); 
		cout << "begin to recv data ..." << " " << gettid() << endl;
		int res = recvfrom(server_socket_fd, buffer, BUFFER_SIZE,0,(struct sockaddr*)&client_addr, &client_addr_length);
  		if(res == -1) 
  		{ 
   			perror("Receive Data Failed:"); 
   			exit(1); 
  		} 
  		cout << "recv " << res << " bytes from " << IpU32ToString(client_addr.sin_addr.s_addr) << ":" << server_addr.sin_port << " " << gettid() << endl;  
		res = sendto(server_socket_fd, buffer, res, 0, (struct sockaddr*)&client_addr,sizeof(client_addr));
		if(res < 0) 
		{ 
			perror("Send File Name Failed:"); 
			exit(1); 
		}
		cout << "response " << res << " bytes to client."  << " " << gettid() << endl;
 	}

}

void* thread_func(void* arg){
	start_udp_server(g_ip, g_port);
}

int main(int argc, char **argv){
	
	if (argc < 4) {
		cout << "Usage: " << argv[0] << " <local_ip> <udp_port> <thread_count>"<< endl;
		exit(0);
	}
	g_ip = argv[1];
	g_port = atoi(argv[2]);
	int thread_count = atoi(argv[3]);
	cout << "ip: " << g_ip << " port: " << g_port << " thread_cout: " << thread_count << endl;
	for(int i = 0; i < thread_count; i++){
		pthread_t thread_fd; 
		pthread_create(&thread_fd, NULL, &thread_func, NULL);
	}
	while(1){
		sleep(3600);
	}
	return 0;
}
