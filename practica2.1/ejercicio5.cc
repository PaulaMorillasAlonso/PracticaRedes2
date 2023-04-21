#include <iostream>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

int main(int argc, char **argv){
    
    struct addrinfo hints;
    struct addrinfo* result;

    //Comprueba argumentos
    if (argc != 3){
        std::cerr << "Invalid arguments\n";
        return -1;
    }
    //Configura
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);
    if(rc < 0){
        std::cerr << gai_strerror(rc) << "\n";
        return -1;
    }
    //Crea socket
    int sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(sock < 0 ){
        std::cerr << "Error creando el socket" << "\n";
        return -1;
    }
    //Establece conexion
    int connectE=connect(sock,result->ai_addr, result->ai_addrlen);
    if(connectE< 0 ){
        std::cerr << "Error al establecer la conexión" << "\n";
        return -1;
    }
    //Obtiene informacion del buffer y la gestiona
    int buff_size = 1024;
    char buffer[buff_size];
    bool quit=false;
    while(!quit){

        std::cin.getline(buffer, buff_size);
        //El buffer contiene la letra Q
        if(strcmp(buffer, "Q") == 0){
            quit=true;
            break;
        }
        int sendE=send(sock, &buffer, buff_size, 0);
        if(sendE<0){
            std::cout << "No se han podido enviar los bytes\n";
            break;
        }
        int numRec = recv(sock, &buffer, buff_size, 0);
        if(numRec<= 0){
            std::cout << "No se han recibido mas mensajes\n";
            break;
        }
        std::cout<<buffer<<"\n";
    }

    
    freeaddrinfo(result);
    close(sock);
    return 0;

}