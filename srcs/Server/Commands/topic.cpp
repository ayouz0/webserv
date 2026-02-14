#include "../Server.hpp"

void Server::handleTopic(int clientSocket, std::vector<std::string> &tokens)
{
    tokens.erase(tokens.begin());

    Client *client = Clients[clientSocket];
    try
    {
        if (tokens.size() < 1)
            throw IrcException("Not enough parameters", 461);
        Channel *channel = getChannelByName(tokens.at(0));
        if (!channel)
            throw IrcException("No such channel", 403);

        if (!channel->isMember(clientSocket))
            throw IrcException("You're not on that channel", 442);

        if (tokens.size() == 1)
        {
            //: irc.server.com 332 <client_nick> #ch :<topic_text>\r\n
            std::ostringstream oss;
            oss << ":" << serverName << " " << 332 << " " << client->getNickname() << " " << channel->getName() << " :" << channel->getTopic() << "\r\n";
            sendMessageToClient(clientSocket, oss.str());
        }
        else
        {
            channel->setTopic(*client, tokens[1]); // this validates client moderator permession
            //:<nick>!<user>@<host> TOPIC #channel_name :<new_topic>\r\n
            std::string m = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIpAddress() + " TOPIC " + channel->getName() + " :" + channel->getTopic();
            channel->broadcast(m);
        }
    }
    catch (IrcException &e)
    {
        sendMessageToClient(clientSocket, this->generateErrorResponce(e.getCode(), client->getNickname(), "TOPIC", e.what()));
    }
}