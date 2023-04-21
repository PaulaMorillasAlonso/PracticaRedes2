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
    //No hay argumentos suficientes
    if (argc != 4){
        std::cout << "Invalid arguments\n";
        return -1;
    }

    //Reservamos espacio para hints
    memset(&hints, 0, sizeof(addrinfo));
    //Especificamos filtro para familia y tipo de socket UDP (sin conexion)

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    //Comprobacion informacion
    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);
    if(rc != 0){
        //Traduce posibles errores
        std::cout << gai_strerror(rc) << "\n";
        return -1;
    }

    //Creamos el socket
    int sock_udp = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(sock_udp < 0){
        std::cout<< "No se pudo crear el socket" << "\n";
        return -1;
    }

    //Envia el mensaje
    char command =argv[3][0];
    int sendCode = sendto(sock_udp, &command, sizeof(command), 0, result->ai_addr, result->ai_addrlen);
    if(sendCode < 0){
        std::cout << "Error enviando el mensaje\n";
        return -1;
    }
    //Config
    struct sockaddr client;
    socklen_t client_l = sizeof(client);

    int buff_size = 1024;
    char buffer[buff_size];
    
    if(command == 't' || command =='d'){
        int bytes_rcv= recvfrom(sock_udp, buffer, buff_size, 0, &client, &client_l);
        if(bytes_rcv== -1){
            std::cerr << "Error receiving\n";
            return -1;
        }
        buffer[bytes_rcv] = '\0';
        std::cout <<"Date/Time: "<< buffer << "\n";
    }
    else if(command=='q'){
        std::cout << "Saliendo..." << std::endl;
        return 0;
    }
    else {
        std::cout << "Comando no válido" << std::endl;
        return -1;
    }
    freeaddrinfo(result);
    close(sock_udp);
    return 0;
}