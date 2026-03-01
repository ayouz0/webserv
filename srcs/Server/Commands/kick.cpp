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

        if (target_channels.empty() || target_clients.empty())
            throw IrcException("KICK", MSG_NEEDMOREPARAMS, ERR_NEEDMOREPARAMS);

        if (target_channels.size() != 1 && target_channels.size() != target_clients.size())
            throw IrcException("KICK", MSG_NEEDMOREPARAMS, ERR_NEEDMOREPARAMS);

        std::string comment = tokens.size() > 3 ? tokens.at(3) : executor->getNickname();
        if (target_channels.size() == 1)
        {
            if (target_channels.at(0).empty())
                throw IrcException("KICK", MSG_NEEDMOREPARAMS, ERR_NEEDMOREPARAMS);
            if (target_channels.at(0).at(0) != '#' && target_channels.at(0).at(0) != '&')
                throw IrcException(target_channels.at(0), MSG_BADCHANMASK, ERR_BADCHANMASK);

            Channel *channel = getChannelByName(target_channels.at(0));
            if (!channel)
                throw IrcException(target_channels.at(0), MSG_NOSUCHCHANNEL, ERR_NOSUCHCHANNEL);

            channel->kickMultipleMembers(executor->getUID(), target_clients, *this, comment);
        }
        else
        {
            for (size_t i = 0; i < target_channels.size(); i++)
            {
                try
                {
                    if (target_channels.at(i).empty() || target_clients.at(i).empty())
                        throw IrcException("KICK", MSG_NEEDMOREPARAMS, ERR_NEEDMOREPARAMS);
                    if (target_channels.at(i).at(0) != '#' && target_channels.at(i).at(0) != '&')
                        throw IrcException(target_channels.at(i), MSG_BADCHANMASK, ERR_BADCHANMASK);
                    Channel *channel = getChannelByName(target_channels.at(i));
                    if (!channel)
                        throw IrcException(target_channels.at(i), MSG_NOSUCHCHANNEL, ERR_NOSUCHCHANNEL);
                    if (!channel->isModerator(*executor))
                        throw IrcException(channel->getName(), MSG_CHANOPRIVSNEEDED, ERR_CHANOPRIVSNEEDED);
                    Client *target = channel->getMemberByNickname(target_clients.at(i));
                    if (!target)
                        throw IrcException(target_clients.at(i), MSG_USERNOTINCHANNEL, ERR_USERNOTINCHANNEL);
                    std::string response = ":" + executor->getNickname() + "!" + executor->getUsername() +
                                           "@" + executor->getIpAddress() + " KICK " + channel->getName() + " " + target->getNickname() + " :" + comment + "\r\n";
                    channel->broadcast(response);
                    channel->removeClient(target->getUID());
                }
                catch (const IrcException &e)
                {
                    sendMessageToClient(clientSocket, this->generateErrorResponce(e.getCode(), executor->getNickname(), e.getContext(), e.what()));
                }
            }
        }
        this->clearChannelsWithNoMembers();
    }
    catch (const IrcException &e)
    {
        sendMessageToClient(clientSocket, this->generateErrorResponce(e.getCode(), executor->getNickname(), e.getContext(), e.what()));
    }
}