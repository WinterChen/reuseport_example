# reuseport_example
SO_REUSEPORT example, with a udp server and a udp client.
# compile
client:  
g++ -o udp_reuseport_client udp_reuseport_client.cc      
server:  
g++ -o udp_reuseport_server udp_reuseport_server.cc -pthread  
