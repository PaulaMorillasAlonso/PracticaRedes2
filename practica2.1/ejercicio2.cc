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
    if (argc != 3){
        std::cout << "Invalid arguments\n";
        return -1;
    }
    //Reservamos espacio para hints
    memset(&hints, 0, sizeof(addrinfo));
    //Especificamos filtro para familia y tipo de socket UDP (sin conexion)

    hints.ai_family = AF_INET;
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
    //Gestion de la conexion
    int bindE=bind(sock_udp, (struct sockaddr *)result->ai_addr, result->ai_addrlen);
    if(bindE < 0){
        std::cout << "No se pudo establecer la conexion" << "\n";
        return -1;
    }
    //Bucle principal
    int buff_size = 1024;

    while(1){

        char buffer[buff_size];
        struct sockaddr client;
        socklen_t client_l = sizeof(client);
        //Recibe bytes del cliente
        int bytes_rcv= recvfrom(sock_udp, buffer, buff_size, 0, (struct sockaddr *) &client,&client_l);
        if (bytes_rcv < 0) {
            std::cout << "Error al recibir los bytes" << "\n";
            return -1;
        }
        buffer[bytes_rcv]='\0';

        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];
        //Obtiene info del cliente
        int nameInf= getnameinfo((struct sockaddr *) &client, client_l, host, NI_MAXHOST,serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);
        if(nameInf!=0){
            std::cout << "Error al obtener el nombre del cliente y su puerto" << "\n";
            return -1;
        }
        else{
            std::cout<<"Conexion de Host: "<< host << " Puerto: "<<serv<<"\n";
            std::cout<<"Numero de bytes "<< bytes_rcv << " Info: "<<buffer<<"\n";
        }
        //Procesa los comandos especificados
        char command = buffer[0];
        time_t t = time(nullptr);
        struct tm*tm = localtime(&t);
        int timeCode = 0;
        char buffTime[buff_size];
        switch (command) {
            case 'q':
                std::cout << "Saliendo..." << std::endl;
                return 0;
            break;
            case 't':
                timeCode=strftime(buffTime,buff_size, "%R", tm);
                std::cout <<"\n";
            break;

            case 'd':
                timeCode=strftime(buffTime,buff_size, "%D", tm);
                std::cout <<"\n";
            break;
            //En caso de que use un comando distinto a q/t/d
            default:
                std::cout << "Comando no soportado: " <<command<< std::endl;
                std::cout <<"\n";

            break;
        }
        //Gestion de errores
        if(timeCode < 0){
            std::cout<< "Error de fecha/hora" << "\n";
            return -1;
        }
        //Envia el mensaje
        int sendCode = sendto(sock_udp, buffTime, timeCode +1, 0,(struct sockaddr *)  &client, client_l);
        if(sendCode < 0){
            std::cout << "Error enviando el mensaje\n";
            return -1;
        }
    }

    freeaddrinfo(result);
    close(sock_udp);
    return 0;
}