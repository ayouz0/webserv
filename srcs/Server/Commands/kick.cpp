# include "../Server.hpp"

void    Server::handleKick(int clientSocket, std::vector<std::string> tokens){
    Client *client = findClientBySocketId(clientSocket);
    if (!client)
        return ;
}