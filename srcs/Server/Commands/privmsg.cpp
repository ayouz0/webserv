#include "../Server.hpp"

void    Server::handlePrivMsg(int clientSocket, std::vector<std::string> &tokens){
    if (tokens.size() < 3) throw IrcException("Invalid number of argumenets", 122); // TODO check things

    std::string dest = tokens.at(1);

    if (dest.at(0) == '#')
    {
        Channel* ch = getChannelByName(dest.substr(1));
        //:user1!user1_username@127.0.0.1 JOIN #general\r\n
        std::string message = ":" + Clients[clientSocket]->getNickname() + "!" + Clients[clientSocket]->getUsername() + "@" + \
        Clients[clientSocket]->getIpAddress() + " PRIVMSG " + dest + " :" + tokens.at(2) + "\r\n";
        
        if (ch) ch->broadcast(message, Clients[clientSocket]);
    }
    else
    {
        Client  *from = Clients[clientSocket];
        Client  *to = getClientByNickname(dest);

        if (to){
            std::string message = ":" + from->getNickname() + "!" + from->getUsername() + "@" + \
            from->getIpAddress()    + " PRIVMSG " + to->getNickname() + " :" + tokens.at(2) + "\r\n";
            sendMessageToClient(to->getSocket(), message);
        }
    }

}

