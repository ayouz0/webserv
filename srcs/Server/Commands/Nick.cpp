#include "../Server.hpp"

bool    isValidNickname(const std::string &nickname){
    if (nickname.empty() || nickname.length() > 9){
        return false;
    }
    for (size_t i = 0; i < nickname.length(); i++){
        char c = nickname[i];   // `|^_-{}[] and \'
        if (!isalnum(c) && c != '-' && c != '_' && c != '[' && c != ']' && c != '\\' && c != '`' && c != '{' && c != '}'){
            return false;
        }
    }
    return true;
}

void    Server::handleNickCommand(const std::vector<std::string> &tokens, int clientSocket){
    

    // if ()
    Client *client = this->Clients[clientSocket];

    if (tokens.size() < 2){
        std::string nickName = client->getNickname().empty() ? "*" : client->getNickname();
        std::string errorMsg = this->generateErrorResponce(431, nickName, "NICK:", "No nickname given");

        this->sendMessageToClient(clientSocket, errorMsg);
        return;
    }

    std::string nickname = tokens[1];
    if (nickname.empty() || !isValidNickname(nickname)){
        std::string nickName = client->getNickname().empty() ? "*" : client->getNickname();
        std::string errorMsg = this->generateErrorResponce(432, nickName, nickname, "Erroneous nickname");

        this->sendMessageToClient(clientSocket, errorMsg);
        return;
    }

    
    for (std::map<int, Client*>::iterator it = this->Clients.begin(); it != Clients.end();   it ++){
        if (it->second->getNickname() == nickname){ // duplicate
            std::string nickName = client->getNickname().empty() ? "*" : client->getNickname();
            std::string errorMsg = this->generateErrorResponce(436, nickName, "NICK", "Nickname already in use");
            this->sendMessageToClient(clientSocket, errorMsg);
            return;
        }
    }
    client->setNickname(nickname);
}
