#include "../Server.hpp"

void Server::handleTopic(int clientSocket, std::vector<std::string> &tokens)
{
    // tokens.erase(tokens.begin());

    Client *client = findClientBySocketId(clientSocket);
    if (!client)
        return;

    try
    {
        if (tokens.size() < 2)
            throw IrcException(MSG_NEEDMOREPARAMS, ERR_NEEDMOREPARAMS);
        Channel *channel = getChannelByName(tokens.at(1));
        if (!channel)
            throw IrcException(MSG_NOSUCHCHANNEL, ERR_NOSUCHCHANNEL);

        if (!channel->isMember(clientSocket))
            throw IrcException(MSG_NOTONCHANNEL, ERR_NOTONCHANNEL);
        if (tokens.size() == 2)
        {
            //: irc.server.com 332 <client_nick> #ch :<topic_text>\r\n
            std::string topic = channel->getTopic();
            std::ostringstream oss;
            oss << ":" << serverName << " " << (topic.empty() ? RPL_NOTOPIC : RPL_TOPIC) << " " << client->getNickname() << " " << channel->getName() << " :" << (topic.empty() ? "No Topic Set" : topic) << "\r\n";
            sendMessageToClient(clientSocket, oss.str());
        }
        else
        {
            channel->setTopic(*client, tokens.at(2)); // this validates client moderator permession
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