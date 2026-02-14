#include "../Server.hpp"
# include "../../Channel/Channel.hpp"

void Server::handleJoinChannel(int socketId, std::vector<std::string> channelData)
{
    Client *client = this->Clients[socketId];
    channelData.erase(channelData.begin());

    try
    {
        if (channelData.empty())
            throw IrcException("Not enough parameters", 461); // ERR_NEEDMOREPARAMS

        Channel *ch = NULL;
        std::string name = channelData.at(0);
        std::string pass = (channelData.size() >= 2) ? channelData.at(1) : "";

        for (size_t i = 0; i < channels.size(); i++)
        {
            if (channels[i].getName() == name)
            {
                ch = &(channels[i]);
                break;
            }
        }

        if (ch)
        {
            ch->joinChannel(*client, pass);
        }
        else
        {
            Channel newChannel(*client, name);
            if (!pass.empty())
            {
                newChannel.setPassword(*client, pass);
            }
            channels.push_back(newChannel);
            // After push_back, get the pointer to the element in the vector
            ch = &channels.back();
        }

        std::string response = ":" + client->getNickname() + "!" + client->getUsername() + 
                      "@" + client->getIpAddress() + " JOIN " + ch->getName() + "\r\n";
        
        ch->broadcast(response);

    }
    catch (const IrcException &e)
    {
        sendMessageToClient(socketId, this->generateErrorResponce(e.getCode(), client->getNickname(), "JOIN", e.what()));
    }
}
