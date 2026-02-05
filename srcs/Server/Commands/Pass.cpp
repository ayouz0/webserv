#include "../Server.hpp"

void    Server::handlePassCommand(const std::vector<std::string> &tokens, int clientSocket){

    Client *client = this->Clients[clientSocket];

    if (client->isRegistered() == true){

        std::string nickName = client->getNickname().empty() ? "*" : client->getNickname();
        std::string errorMsg = this->generateErrorResponce(462, nickName, "PASS", "Already registered!");

        this->sendMessageToClient(clientSocket, errorMsg);
        return;
    }

    if (tokens.size() < 2){ 
        std::string nickName = client->getNickname().empty() ? "*" : client->getNickname();
        std::string errorMsg = this->generateErrorResponce(461, nickName, "PASS", "Not Enough Parameters");

        this->sendMessageToClient(clientSocket, errorMsg);
        return;
    }

    std::string password = tokens[1];
    if (password != this->_ServerPassword){
        std::string nickName = client->getNickname().empty() ? "*" : client->getNickname();
        std::string errorMsg = this->generateErrorResponce(462, nickName, "PASS", "Password missmatch");

        this->sendMessageToClient(clientSocket, errorMsg);
        return;
    }

    client->setPassState(true);
}
