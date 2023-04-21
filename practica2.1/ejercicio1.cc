#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <string.h>
#include <iostream>

int main(int argc, char **argv){
    struct addrinfo hints;
    struct addrinfo *result;

    if (argc != 2){
        std::cerr << "Invalid arguments\n";
        return -1;
    }
    memset(&hints,0,sizeof(struct addrinfo));

    hints.ai_flags= AI_PASSIVE;
    hints.ai_family= AF_UNSPEC;
    hints.ai_socktype= SOCK_STREAM;
    int rc= getaddrinfo(argv[1],argv[2], &hints, &result);

    if(rc!=0){
        std::cerr << "[addinfo]: " << gai_strerror(rc)<< "\n";
        return -1;
    }
    
    for(struct addrinfo * i= result; i!= nullptr; i=i->ai_next){

        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);
        
        std::cout<< "HOST: " << host << " FAMILY: " << hints.ai_family << " TYPE: "<< hints.ai_socktype <<"\n";
 
    }
    freeaddrinfo(result);
}