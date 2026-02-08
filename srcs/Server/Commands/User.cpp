#include "../Server.hpp"
bool isValidUsername(const std::string &username)
{
    if (username.empty() || username.length() > 9) 
        return false;

    if (username.find_first_of(" @!\r\n") != std::string::npos)
        return false;

    return true;
}

//       Command: USER
//       Parameters: <username> <hostname> <servername> <realname>
void        Server::handleUserCommand(const std::vector<std::string> &tokens, int clientSocket) {

    Client *client = this->Clients[clientSocket];
    if ( client->getPassState() == false ){
        std::string nickName = client->getNickname().empty() ? "*" : client->getNickname();
        std::string errorMsg = this->generateErrorResponce(431, nickName, "USER", "Client did not set the password");
        this->sendMessageToClient(clientSocket, errorMsg);
    }


    if (client->isRegistered() == true ){
        std::string nickName = client->getNickname().empty() ? "*" : client->getNickname();
        std::string errorMsg = this->generateErrorResponce(462, nickName, "USER", "Client already registered");
        this->sendMessageToClient(clientSocket, errorMsg);
    }

    if ( tokens.size() < 5 ){
        std::string nickName = client->getNickname().empty() ? "*" : client->getNickname();
        std::string errorMsg = this->generateErrorResponce(431, nickName, "USER", "need more parameters");

        this->sendMessageToClient(clientSocket, errorMsg);
        return;
    }

    for (size_t i = 1 ; i < tokens.size(); i++){
        if ( tokens[i].empty() ){
        std::string nickName = client->getNickname().empty() ? "*" : client->getNickname();
            std::string errorMsg = this->generateErrorResponce(431, nickName, "USER", "empty Parameter");
            this->sendMessageToClient(clientSocket, errorMsg);
            return;
        }
    }

    if ( !isValidUsername(tokens[1]) ){
        std::string nickName = client->getNickname();
        std::string errorMsg = this->generateErrorResponce(432, nickName, nickName, "Erroneous username");

        this->sendMessageToClient(clientSocket, errorMsg);
            return;
    }

    client->setUsername(tokens[1]);
    client->setUserState(true);
    if(client->canRegister()){
        client->setRegistered(true);
    }
}
