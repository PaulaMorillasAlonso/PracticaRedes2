#include "Chat.h"
#include <string>
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* tmp = _data;
    memcpy(tmp, &type, sizeof(uint8_t));
    tmp += sizeof(uint8_t);
    memcpy(tmp, nick.c_str(), sizeof(char) * 8);
    tmp += sizeof(char) * 8;
    memcpy(tmp, message.c_str(), sizeof(char) * 80);

}

int ChatMessage::from_bin(char* bobj)
{
    alloc_data(MESSAGE_SIZE);
    memcpy(static_cast<void*>(_data), bobj, MESSAGE_SIZE);
    char* tmp = _data;

    //Reconstruir la clase usando el buffer _data
    memcpy(&type, tmp, sizeof(uint8_t));
    tmp += sizeof(uint8_t);
    char nick_buff[9] = { 0 };
    memcpy(nick_buff, tmp, sizeof(char) * 8);
    nick = std::string(nick_buff);
    tmp += sizeof(char) * 8;
    char m_buff[81] = { 0 };
    memcpy(m_buff, tmp, sizeof(char) * 80);
    message = std::string(m_buff);
    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

         //Recibir Mensajes en y en función del tipo de mensaje
         // Leer mensaje del cliente
         //Socket *client_socket;
         //ChatMessage msg;

        Socket* client_socket;
        ChatMessage msg;

        int client_fd = socket.recv(msg, client_socket);


        if (msg.type == ChatMessage::LOGIN) {  // - LOGIN: Añadir al vector clients

            std::cout << msg.nick << " LOGIN\n";
            if (client_socket != nullptr) {

                std::unique_ptr<Socket> new_client(client_socket);
                clients.push_back(std::move(new_client));
            }
        }
        else if (msg.type == ChatMessage::LOGOUT) { // - LOGOUT: Eliminar del vector clients
            std::cout << msg.nick << " LOGOUT\n";

            for (int i = 0; i < clients.size(); ++i) {
                if ((*clients[i]) == (*client_socket)) {
                    clients.erase(clients.begin() + i);
                }
            }
        }
        else { // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)

            std::cout << msg.nick << " MESSAGE\n";

            for (int i = 0; i < clients.size(); ++i) {
                if (!((*clients[i]) == (*client_socket))) {
                    socket.send(msg, *clients[i]);
                }
            }
        }

    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;
    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);

}

void ChatClient::logout()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);

}

void ChatClient::input_thread()
{
    while (true)
    {
        char buffer[ChatMessage::MESSAGE_SIZE];
        std::cin.getline(buffer, ChatMessage::MESSAGE_SIZE);
        if (strcmp(buffer, "q") == 0) {
            logout();
            break;
        }
        else {
            ChatMessage msg(nick, buffer);
            msg.type = ChatMessage::MESSAGE;
            socket.send(msg, socket);
        }
    }
}

void ChatClient::net_thread()
{
    while (true)
    {
        ChatMessage msg;
        socket.recv(msg);
        std::cout << msg.nick << ": " << msg.message << "\n";

    }
}

