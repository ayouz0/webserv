#include "../Server.hpp"

void Server::sendMessageToClient(int clientSocket, std::string msg) {
    if ((msg.length() < 2) || (msg.substr(msg.length() - 2) != "\r\n")){
        msg += "\r\n";
    }

    std::map<int, Client *>::iterator it = this->Clients.find(clientSocket);
    if (it == this->Clients.end())
        return;

    it->second->appendToOutboundBuffer(msg);
}