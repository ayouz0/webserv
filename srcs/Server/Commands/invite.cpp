#include "../Server.hpp"
// #include "../../Channel/Channel.hpp"

void Server::handleInvite(int clientSocket, std::vector<std::string> tokens)
{
    Client *client = findClientBySocketId(clientSocket);
    if (!client)
        return;
    try
    {
        

        if (tokens.size() < 3)
            throw IrcException("Need More Params", ERR_NEEDMOREPARAMS);
        Client *invitedClient = getClientByNickname(tokens.at(1));
        if (!invitedClient)
            throw IrcException("No Such Nick", ERR_NOSUCHNICK);
        Channel *channel = getChannelByName(tokens.at(2));
        if (!channel || !channel->isMember(client->getUID()))
            throw IrcException("No Such Channel", ERR_NOSUCHCHANNEL);
        if (channel->isMember(invitedClient->getUID()))
            throw IrcException("User Already In Channel", ERR_USERONCHANNEL);
        channel->invite(*client, invitedClient->getUID()); //ATTENTION this will throw ERR_CHANOPRIVSNEEDED  exception

        std::ostringstream oss;
        oss << ":" << serverName << " " << RPL_INVITING << " " << client->getNickname() << " " << invitedClient->getNickname() << " " << channel->getName();
        sendMessageToClient(client->getSocket(), oss.str());

        sendMessageToClient(invitedClient->getSocket(), ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIpAddress() + " INVITE " + invitedClient->getNickname() + " :" + channel->getName());

    }
    catch (const IrcException &e)
    {
        sendMessageToClient(clientSocket, generateErrorResponce(e.getCode(), client->getNickname(), "INVITE", e.what()));
    }
}