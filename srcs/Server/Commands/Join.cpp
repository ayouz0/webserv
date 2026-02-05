#include "../Server.hpp"
# include "../../Channel/Channel.hpp"

void Server::handleJoinChannel(int socketId, std::vector<std::string> channelData)
{
        Client *client = this->Clients[socketId];
        channelData.erase(channelData.begin());
    try
    {
        if (channelData.empty())
            sendMessageToClient(socketId, this->generateErrorResponce(461, client->getNickname(), "JOIN", "Not enough parameters"));
        Channel *ch = NULL;
        std::string name = channelData.at(0);
        std::string pass = channelData.size() >= 2 ?  channelData.at(1) :  "";
        for (int i = 0; i < channels.size(); i++)
        {
            std::cout << "here" << std::endl;
            if (channels[i].getName() == name)
            {
                ch = &(channels[i]);
                break ;
            }
        }
        if (ch)
        {
            ch->joinChannel(*client, pass);
        }
        else{
            Channel c(*client, name);
            if (!pass.empty()){
                c.setPassord(*client,  pass);
            }
            channels.push_back(c);
            ch = &c;
        }

        std::cout << "joined " << ch->getName() << std::endl;
 
    }
    catch (std::exception &e)
    {
        // std::string error = this->generateErrorResponce();
        sendMessageToClient(socketId, this->generateErrorResponce(411, client->getNickname(), "JOIN", e.what()));
    }
}
