#include "../Server.hpp"


void    Server::handleQuit(int clientSocket, std::vector<std::string> &tokens){
    Client *client = findClientBySocketId(clientSocket);
    if (!client) return ;

    std::string message = tokens.size() > 1 ? tokens.at(1) : "Bye";

    for (size_t i = 0; i < channels.size(); i++)
    {
        Channel &curr = channels.at(i);
        if (curr.isMember(client->getUID())){
            curr.broadcast(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIpAddress() + " QUIT :" + message);
        }
    }
    closeClientConnection(clientSocket);
}
