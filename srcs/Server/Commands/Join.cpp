#include "../Server.hpp"
#include "../../Channel/Channel.hpp"

std::vector<std::pair<std::string, std::string> > constructChannelAndPasses(std::vector<std::string> data)
{
    std::vector<std::string> channels = splitter(data[0], ',');
    std::vector<std::string> passes = (data.size() >= 2) ? splitter(data[1], ',') : std::vector<std::string>();

    std::vector<std::pair<std::string, std::string> > channelsAndPasses;
    for (size_t i = 0; i < channels.size(); i++)
    {
        std::string pass = (i < passes.size()) ? passes[i] : "";
        channelsAndPasses.push_back(std::make_pair(channels[i], pass));
    }
    return channelsAndPasses;
}

void Server::handleJoinChannel(int socketId, std::vector<std::string> channelData)
{
    Client *client = this->Clients[socketId];
    channelData.erase(channelData.begin());
    try
    {
        if (channelData.empty())
            throw IrcException("Not enough parameters", 461); // ERR_NEEDMOREPARAMS

        std::vector<std::pair<std::string, std::string> > channelsAndPasses = constructChannelAndPasses(channelData);


        for (size_t i = 0; i < channelsAndPasses.size(); i++)
        {
            try{

                    std::string name = channelsAndPasses[i].first;
                    std::string pass = channelsAndPasses[i].second;
    
                    if (name.at(0) != '#')
                        throw IrcException("Invalid Channel Mask", 476);
    
                    Channel *channel = getChannelByName(name);
    
                    if (channel)
                    {
                        if (!channel->joinChannel(*client, pass))
                            continue; // already a member, do nothing
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
            catch(const IrcException &e){
                sendMessageToClient(socketId, this->generateErrorResponce(e.getCode(), client->getNickname(), "JOIN", e.what()));
            }

        }
    }
    catch (const IrcException &e)
    {
        sendMessageToClient(socketId, this->generateErrorResponce(e.getCode(), client->getNickname(), "JOIN", e.what()));
    }
}
