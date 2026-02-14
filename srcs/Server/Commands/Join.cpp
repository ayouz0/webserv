#include "../Server.hpp"
# include "../../Channel/Channel.hpp"

void Server::handleJoinChannel(int socketId, std::vector<std::string> channelData)
{
    Client *client = this->Clients[socketId];
    channelData.erase(channelData.begin());

    try
    {
        if (channelData.empty()) throw IrcException("Not enough parameters", 461); // ERR_NEEDMOREPARAMS

            std::string name = channelData.at(0);
            
            if (name.at(0) != '#') throw IrcException("Invalid Channel Mask", 476);
            
            std::string pass = (channelData.size() >= 2) ? channelData.at(1) : "";
            
            Channel *channel = getChannelByName(name);
            

        if (channel)
        {
            if (!channel->joinChannel(*client, pass)) return; // already a member, do nothing
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
            channel = &channels.back();
        }

        std::string response = ":" + client->getNickname() + "!" + client->getUsername() + 
                      "@" + client->getIpAddress() + " JOIN " + channel->getName() + "\r\n";
        
        channel->broadcast(response);

    }
    catch (const IrcException &e)
    {
        sendMessageToClient(socketId, this->generateErrorResponce(e.getCode(), client->getNickname(), "JOIN", e.what()));
    }
}
