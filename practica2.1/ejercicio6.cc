#include <iostream>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <thread>
#include <vector>


// Para salir con q, se debe salir el número de veces igual al número de threads
class MessageHandlerUDP {
    private:
        int socket_fd;

    public:
        MessageHandlerUDP(int socket_Fd) : socket_fd(socket_Fd) {}

    void operator()(){
        int buff_size = 1024;
        char buffer[buff_size];
        struct sockaddr client;
        socklen_t client_l = sizeof(client);

        while (true){
            int bytes_rcv = recvfrom(socket_fd, buffer, buff_size, 0, (struct sockaddr*) &client, &client_l);
            if (bytes_rcv < 0){
                std::cout << "Error al recibir los bytes\n";
                return;
            }
            buffer[bytes_rcv] = '\0';

            char host[NI_MAXHOST];
            char serv[NI_MAXSERV];

            int nameInfo = getnameinfo((struct sockaddr*) &client, client_l, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
            if (nameInfo != 0){
                std::cout << "Error al obtener el nombre del cliente y su puerto\n";
                return;
            }
            else {
                std::cout << "Conexion de Host: " << host << " Puerto: " << serv << "\n";
                std::cout << "Numero de Bytes: " << bytes_rcv << " Info: " << buffer << "\n";
            }

            char command = buffer[0];
            time_t t = time(nullptr);
            struct tm* tm = localtime(&t);
            int timeCode = 0;
            char buffTime[buff_size];

            switch (command){
                case 'q':
                    std::cout << "Saliendo...\n";
                    return;
                break;
                case 't':
                    timeCode = strftime(buffTime, buff_size, "%R", tm);
                    std::cout << " (Thread " << std::this_thread::get_id() << ")\n";
                    sleep(3);
                break;
                case 'd':
                    timeCode = strftime(buffTime, buff_size,  "%D", tm);
                    std::cout << " (Thread " << std::this_thread::get_id() << ")\n";
                    sleep(3);
                break;
                default:
                    std::cout << "Comando no reconocido: " << command << std::endl;
                    std::cout << "\n";
                break;
            }

            if (timeCode < 0){
                std::cout << "Error de fecha/hora\n";
                return;
            }

            int sendCode = sendto(socket_fd, buffTime, timeCode + 1, 0, (struct sockaddr*) &client, client_l);
            if (sendCode < 0){
                std::cout << "Error enviando el mensaje\n";
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

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (rc != 0){
        std::cout << gai_strerror(rc) << "\n";
        return -1;
    }

    int socket_udp = socket(result->ai_family, result->ai_socktype, 0);
    if  (socket_udp < 0){
        std::cout << "No se pudo crear el socket\n";
        return -1;
    }

    int bindE = bind(socket_udp, (struct sockaddr*) result->ai_addr, result->ai_addrlen);
    if (bindE < 0){
        std::cout << "No se pudo establecer la conexion\n";
        return -1;
    }

    std::vector<std::thread> threads;
    // EJ: 4 Threads
    for (int i = 0; i < 4; ++i){
        threads.push_back(std::thread(MessageHandlerUDP(socket_udp)));
    }

    std::cout << "Servidor en marcha\n";

    for(auto& thread : threads){
        thread.join();
    }
    
    freeaddrinfo(result);
    close(socket_udp);

    std::cout << "Fin de la conexión\n";
    return 0;
}