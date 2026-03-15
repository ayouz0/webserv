#include "../Server.hpp"
enum modeChar
{
    op_i,
    op_t,
    op_k,
    op_o,
    op_l
};

#define Plus true
#define Minus false
struct ModeChange
{
    bool state;
    char mode;
    std::string param;

    ModeChange(bool s, char m, std::string p)
    {
        state = s;
        mode = m;
        param = p;
    }
};

// MODE #channel *( ( "+" / "-" ) *( "i" / "t" / "k" / "o" / "l" ) ) [args...]
void Server::handleMode(int clientSocket, std::vector<std::string> &tokens)
{
    Client *client = findClientBySocketId(clientSocket);
    int howManySucceed = 0;
    if (!client)
        return;
    try
    {
        if (tokens.size() < 2)
            throw IrcException("MODE", MSG_NEEDMOREPARAMS, ERR_NEEDMOREPARAMS);

        std::string target = tokens[1];
        if (target[0] != '&' && target[0] != '#')
            throw IrcException(target, MSG_BADCHANMASK, ERR_BADCHANMASK);
        Channel *channel = getChannelByName(target);
        if (!channel)
            throw IrcException(target, MSG_NOSUCHCHANNEL, ERR_NOSUCHCHANNEL);
        if (!channel->isMember(client->getUID()))
            throw IrcException(target, MSG_NOTONCHANNEL, ERR_NOTONCHANNEL);

        if (tokens.size() == 2)
        {
            std::ostringstream modes;
            //: irc.server.com 324 User #channel +ntk secret_pass
            modes << "irc " << RPL_CHANNELMODEIS << " " << client->getNickname() << " " << channel->getName() << " " << channel->generateModes();

            sendMessageToClient(clientSocket, modes.str());
            return;
        }

        std::vector<ModeChange> modeQueue;
        bool state = true;
        int paramModesCount = 0;

        std::string modeString = tokens[2];
        size_t paramIndex = 3;

        for (size_t i = 0; i < modeString.length(); ++i)
        {
            char c = modeString[i];

            if (c == '+')
            {
                state = true;
                continue;
            }
            if (c == '-')
            {
                state = false;
                continue;
            }

            if (c != 'i' && c != 't' && c != 'k' && c != 'o' && c != 'l')
            {
                this->sendMessageToClient(clientSocket, generateErrorResponce(ERR_UNKNOWNMODE, client->getNickname(), std::string(1, c), "is unknown mode char to me for " + channel->getName()));
                continue;
            }

            // bool requiresParam = false;
            // if (c == 'o' || c == 'k')
            //     requiresParam = true;
            // if (c == 'l' && state == true)
            //     requiresParam = true;

            std::string param = "";
            // if (requiresParam)
            // {
            //     if (paramModesCount >= 3)
            //     {
            //         continue; // standard
            //     }

                if (paramIndex < tokens.size())
                {
                    param = tokens[paramIndex++];
                    paramModesCount++;
                }
            //     // else
            //     // {
            //     //     this->sendMessageToClient(clientSocket, generateErrorResponce(ERR_NEEDMOREPARAMS, client->getNickname(), std::string(1, c), "need more parameters"));
            //     // }
            // }

            modeQueue.push_back(ModeChange(state, c, param));
        }
        for (size_t i = 0; i < modeQueue.size(); ++i)
        {
            try
            {
                channel->applyMode(*this, client, modeQueue[i].state, modeQueue[i].mode, modeQueue[i].param);
                howManySucceed++;
            }
            catch (const IrcException &e)
            {
                this->sendMessageToClient(clientSocket, generateErrorResponce(e.getCode(), client->getNickname(), e.getContext(), e.what()));
            }
        }
        if (howManySucceed <= 0) return ;
        //: Nick!user@host MODE #channel +i-lk secret
        std::string message = ":" + client->getNickname() + "!" + client->getUsername() + "@" +
                            client->getIpAddress() + " MODE " + target + modeString;

        for (size_t i = 3; i < tokens.size(); i++)
            message += " " + tokens.at(i);

        channel->broadcast(message);
    }
    catch (const IrcException &e)
    {
        this->sendMessageToClient(clientSocket, generateErrorResponce(e.getCode(), client->getNickname(), e.getContext(), e.what()));
    }
}