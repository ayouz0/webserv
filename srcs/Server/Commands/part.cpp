# include "../Server.hpp"

void    Server::handlePart(int clientSocket, std::vector<std::string> tokens){

    Client *client = this->findClientBySocketId(clientSocket);
    if (!client) return ;

    try{
        if (tokens.size() < 2) throw IrcException(MSG_NEEDMOREPARAMS, ERR_NEEDMOREPARAMS);

        std::vector<std::string> channelsToLeave = splitter(tokens.at(1), ',');

        for (size_t i = 0 ; i < channelsToLeave.size(); i++)
        {
            std::string channelName = channelsToLeave.at(i);
            try{
                if (channelName.empty()) continue;

                if (channelName.at(0) != '#' && channelName.at(0) != '&')
                    throw IrcException(MSG_BADCHANMASK,  ERR_BADCHANMASK);

                Channel *channelPtr = getChannelByName(channelName);

                if (!channelPtr) throw IrcException(MSG_NOSUCHCHANNEL, ERR_NOSUCHCHANNEL);

                std::string message = tokens.size() > 2 ? tokens.at(2) : client->getNickname();
                channelPtr->leave(client->getUID(), message); // this checks membership and broadast message

                clearChannelsWithNoMembers();

            }
            catch(const IrcException &e){
                 sendMessageToClient(clientSocket, this->generateErrorResponce(e.getCode(), client->getNickname(), channelName, e.what()));
            }
        }
    }
    catch (const IrcException &e)
    {
        sendMessageToClient(clientSocket, this->generateErrorResponce(e.getCode(), client->getNickname(), "PART", e.what()));
    }

}