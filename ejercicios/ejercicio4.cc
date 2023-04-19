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

    if (argc != 3){
        std::cerr << "Invalid arguments\n";
        return -1;
    }

    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);
    if(rc < 0){
        std::cerr << gai_strerror(rc) << "\n";
        return -1;
    }

    int sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(sock < 0 ){
        std::cerr << "Error creando el socket" << "\n";
        return -1;
    }

    //Gestion de la conexion
    int bindE=bind(sock, (struct sockaddr *)result->ai_addr, result->ai_addrlen);
    if(bindE < 0){
        std::cout << "No se pudo establecer la conexion" << "\n";
        return -1;
    }

    //Espera una sola conexion
    int listenE=listen(sock,1);

    if(listenE < 0){
        std::cout << "No se pudo poner el estado LISTEN" << "\n";
        return -1;
    }

    //Si se puede poner a escuchar
    struct sockaddr client;
    socklen_t client_l = sizeof(client);
    //Acepta la conexion
    int  client_socket= accept(sock, &client, &client_l);
    if(client_socket < 0){
        std::cout << "No se pudo aceptar la conexión entrante" << "\n";
        return -1;
    }
    //Si se establece bien la conexion mostrar la dirección y número de puerto del cliente
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    //Obtiene info del cliente
    int nameInf= getnameinfo((struct sockaddr *) &client, client_l, host, NI_MAXHOST,serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);
    if(nameInf!=0){
        std::cout << "Error al obtener el nombre del cliente y su puerto" << "\n";
        return -1;
    }
    else{
        std::cout<<"Conexion desde: "<< host << " "<<serv<<"\n";
    }
    //Lee n bytes a traves del socket del cliente y lo guarda en buffer
    //Despues envia esos n bytes de buffer al cliente 
    int buff_size = 1024;
    char buffer[buff_size];
    while(1){

        int numBytes = recv(client_socket, &buffer, buff_size, 0);
        if(numBytes <= 0){
            std::cout << "No hay más mensajes enviados\n";
            break;
        }
        buffer[numBytes] = '\0';

        int sendBytes=send(client_socket, buffer, numBytes, 0);
        if(sendBytes<0){
            std::cout << "No se han enviado los bytes necesarios\n";
            break;
        }
    }

    freeaddrinfo(result);
    close(sock);
    close(client_socket);
    
    std::cout << "Conexión terminada" << "\n";
    return 0;

}