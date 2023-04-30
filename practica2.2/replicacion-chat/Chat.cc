#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char *tmp=_data;
    memcpy(tmp,&type,sizeof(uint8_t));
    tmp+=sizeof(uint8_t);
    memcpy(tmp,&nick ,sizeof(char)*8); 
    tmp+=sizeof(char)*8;
    memcpy(tmp,&message ,sizeof(char)*80); 

}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    memcpy(&type,_data ,sizeof(uint8_t));
    _data+=sizeof(uint8_t);
    memcpy(&nick,_data ,sizeof(char)*8);//x es el destino
    _data+=sizeof(char)*8;
    memcpy(&message,_data ,sizeof(char)*80);
    
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
        Socket *client_socket;
        ChatMessage msg;
        char buffer[ChatMessage::MESSAGE_SIZE];

        int client_fd =socket.recv(msg,client_socket);
        if (client_fd == -1) {
            std::cout<<"Error aceptando la conexion";
        }
        
        if(msg.type==ChatMessage::LOGIN){  // - LOGIN: Añadir al vector clients
            std::cout << msg.nick << " LOGIN\n";
            std::unique_ptr<Socket> new_client (client_socket);
            clients.push_back(std::move(new_client));
        }
        else if(msg.type==ChatMessage::LOGOUT){ // - LOGOUT: Eliminar del vector clients
            std::cout << msg.nick << " LOGOUT\n";
            auto it = clients.begin();
            while(it != clients.end() && !(*(*it).get() == *client_socket )) it++; //avanza hasta encontrar el cliente que quiere borrar
            if(it!=clients.end()) clients.erase(it);
        }
        else{ // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
            
            std::cout << msg.nick << " MESSAGE\n";
            msg.from_bin(buffer);
            //Envia mensaje a los clientes
            msg.to_bin();

            for (const auto& c : clients) {
                if (!(*c == *client_socket)) {
                    c->send(msg,*c);
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
    // Completar
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}

