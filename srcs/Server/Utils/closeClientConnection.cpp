#include "../Server.hpp"

void    Server::closeClientConnection(int clientSocket) throw() {
    close(clientSocket);
    for (size_t i = 0; i < this->clientSockets.size(); i++){
        if (this->clientSockets[i].fd == clientSocket){
            this->clientSockets.erase(this->clientSockets.begin() + i);
            break;
        }
    }
    delete this->Clients[clientSocket];
    this->Clients.erase(clientSocket);
}
