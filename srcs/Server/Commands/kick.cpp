#include "../Server.hpp"



void Server::handleKick(int clientSocket, std::vector<std::string> tokens)
{
    Client *executor = findClientBySocketId(clientSocket);
    if (!executor)
        return;

    try
    {
        if (tokens.size() < 3)
            throw IrcException("KICK", MSG_NEEDMOREPARAMS, ERR_NEEDMOREPARAMS);
        std::vector<std::string> target_channels = splitter(tokens.at(1), ',');
        std::vector<std::string> target_clients = splitter(tokens.at(2), ',');

        if (target_channels.size() != 1 && target_channels.size() != target_clients.size())
            throw IrcException("KICK", MSG_NEEDMOREPARAMS, ERR_NEEDMOREPARAMS);
        
        if (target_channels.size() == 1)
        {
            Channel *channel = getChannelByName(target_channels.at(0));
            if (!channel) throw IrcException(target_channels.at(0), MSG_NOSUCHCHANNEL, ERR_NOSUCHCHANNEL);

            channel->kickMultipleMembers(executor->getUID(), target_clients, *this, tokens.size() > 3 ? tokens.at(3) : executor->getNickname());
            
        }
        // TODO don't forget the other case
    }
    catch (const IrcException &e)
    {
        sendMessageToClient(clientSocket, this->generateErrorResponce(e.getCode(), executor->getNickname(), e.getContext(), e.what()));
    }
}