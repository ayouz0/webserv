# include "../Server.hpp"

Client* Server::findClientBySocketId(int socketId) const {
    std::map<int, Client *>::const_iterator it = Clients.find(socketId);
    if (it != Clients.end()) {
        return it->second;
    }
    return NULL;
}