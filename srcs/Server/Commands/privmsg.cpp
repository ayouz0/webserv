#include "../Server.hpp"

void Server::handlePrivMsg(int clientSocket, std::vector<std::string> &tokens)
{
    Client *client = findClientBySocketId(clientSocket);
    if (!client)
        return;

    if (tokens.size() < 2 || tokens.at(1).empty())
        return sendMessageToClient(clientSocket, generateErrorResponce(ERR_NORECIPIENT, client->getNickname(), "PRIVMSG", "No recipient given (PRIVMSG)"));

    if (tokens.size() < 3 || tokens.at(2).empty())
        return sendMessageToClient(clientSocket, generateErrorResponce(ERR_NOTEXTTOSEND, client->getNickname(), "PRIVMSG", "No text to send"));

    std::string message = tokens.at(2);
    std::vector<std::string> dests = splitter(tokens.at(1), ',');
    
    if (dests.empty())
        return sendMessageToClient(clientSocket, generateErrorResponce(ERR_NORECIPIENT, client->getNickname(), "PRIVMSG", "No recipient given (PRIVMSG)"));
    for (size_t i = 0; i < dests.size(); i++)
    {
        std::string dest = dests.at(i);
        
        if (dest.empty())
            continue;
        try
        {
            if (dest.at(0) == '#')
            {
                Channel *ch = getChannelByName(dest);

                if (!ch)
                    throw IrcException(MSG_NOSUCHNICK, ERR_NOSUCHNICK);

                if (!ch->isMember(clientSocket))
                    throw IrcException(MSG_CANNOTSENDTOCHAN, ERR_CANNOTSENDTOCHAN);

                //: user1!user1_username@127.0.0.1 JOIN #general\r\n
                std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@" +
                                  client->getIpAddress() + " PRIVMSG " + dest + " :" + message + "\r\n";

                if (ch)
                    ch->broadcast(msg, client);
            }
            else
            {
                Client *from = client;
                Client *to = getClientByNickname(dest);

                if (to)
                {
                    std::string msg = ":" + from->getNickname() + "!" + from->getUsername() + "@" +
                                      from->getIpAddress() + " PRIVMSG " + to->getNickname() + " :" + message + "\r\n";
                    sendMessageToClient(to->getSocket(), msg);
                }
                else
                {
                    throw IrcException(MSG_NOSUCHNICK, ERR_NOSUCHNICK);
                }
            }
        }
        catch (const IrcException &e)
        {
            sendMessageToClient(clientSocket, generateErrorResponce(e.getCode(), client->getNickname(), dest, e.what()));
        }
    }
}
