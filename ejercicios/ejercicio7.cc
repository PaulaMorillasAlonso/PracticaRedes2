#include <iostream>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <thread>
#include <vector>


class MessageHandlerTCP {
    private:
        int socket_fd;

    public:
        MessageHandlerTCP(int socket_Fd) : socket_fd(socket_Fd) {}

    void operator()(){
        int buff_size = 1024;
        char buffer[buff_size];

        while (true){
            int bytes = recv(socket_fd, &buffer, buff_size, 0);
            if (bytes == 0){
                std::cout << "Último Mensaje\n";
                return;
            }
            buffer[bytes] = '\0';

            int sendBytes = send(socket_fd, buffer, bytes, 0);
            if (sendBytes < 0){
                std::cout << "No se han enviado los bytes enviados\n";
                return;
            }
        }
    }
};


int main(int argc, char **argv){
    struct addrinfo hints;
    struct addrinfo* result;

    if (argc != 3){
        std::cout << "Invalid arguments\n";
        return -1;
    }

    memset(&hints, 0, sizeof(addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (rc != 0){
        std::cout << gai_strerror(rc) << "\n";
        return -1;
    }

    int socket_tcp = socket(result->ai_family, result->ai_socktype, 0);
    if  (socket_tcp < 0){
        std::cout << "No se pudo crear el socket\n";
        return -1;
    }

    int bindE = bind(socket_tcp, (struct sockaddr*) result->ai_addr, result->ai_addrlen);
    if (bindE < 0){
        std::cout << "No se pudo establecer la conexion\n";
        return -1;
    }

    int listenE = listen(socket_tcp, 5);
    if (listenE < 0){
        std::cout << "No se pudo activar el estado Listen\n";
        return -1;
    }

    struct sockaddr client;
    socklen_t client_l = sizeof(client);

    int sock_client = accept(socket_tcp, (struct sockaddr*) &client, &client_l);
    if (sock_client < 0){
        std::cout << "No se pudo aceptar la conexión entrante\n";
        return -1;
    }

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    int nameInfo = getnameinfo((struct sockaddr*) &client, client_l, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);
    if (nameInfo != 0){
        std::cout << "Error al obtener el nombre del cliente y su puerto\n";
        return -1;
    }
    else {
        std::cout << "Conexion desde: " << host << " " << serv << "\n";
    }

    std::vector<std::thread> threads;
    // EJ: 1 Threads
    for (int i = 0; i < 1; ++i){
        threads.push_back(std::thread(MessageHandlerTCP(sock_client)));
    }

    std::cout << "Servidor en marcha\n";

    for(auto& thread : threads){
        thread.join();
    }
    
    freeaddrinfo(result);
    close(socket_tcp);
    close(sock_client);

    std::cout << "Fin de la conexión\n";
    return 0;
}