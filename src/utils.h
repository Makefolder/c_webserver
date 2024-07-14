int create_socket(int flag);
struct sockaddr_in setup_serv_addr(int port, char* address);
struct sockaddr_in setup_local_addr(int port);
