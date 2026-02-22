#include "../Server.hpp"

void    Server::closeClientConnection(int clientSocket) throw() {
    close(clientSocket);

    Client *client = findClientBySocketId(clientSocket);
    if (!client) return ;

    for (std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it) {
        it->removeClient(client->getUID());
    }

    for (size_t i = 0; i < this->clientSockets.size(); i++){
        if (this->clientSockets[i].fd == clientSocket){
            this->clientSockets.erase(this->clientSockets.begin() + i);
            break;
        }
    }
    delete this->Clients[clientSocket];
    this->Clients.erase(clientSocket);
}
