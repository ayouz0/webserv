#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "../Client/Client.hpp"
# include <vector>
#include <exception>
# include <iostream>
#include "../IrcException/IrcException.hpp"
# include <sys/socket.h>
class Channel
{
    struct ChannelMember
    {
        const Client &client;
        bool moderator;

       ChannelMember(const Client &c, bool mod) : client(c), moderator(mod) {}
    };
    
    std::vector<ChannelMember> members;
    std::string topic;
    bool topic_lock;
    int id;
    std::string name;
    bool invite_only;
    std::string password;
    static size_t counter;
    bool locked;

    bool isModerator(const Client &c) const {
        for (size_t i = 0; i < members.size(); i++)
        {
            if (members[i].client.getNickname() == c.getNickname()) 
                return members[i].moderator;
        }
        return false;
    }

public:
    Channel(const Client &creator, std::string name)
        : topic("no topic is set yet"), topic_lock(false), name(name), 
          invite_only(false), password(""), locked(false)
    {
        this->id = ++counter;
        members.push_back(ChannelMember(creator, true));
    }

    bool joinChannel(const Client &c, std::string password) {
        if (invite_only) 
            throw IrcException("Cannot join channel (+i)", 473); // ERR_INVITEONLYCHAN
        
        if (locked && password != this->password) 
            throw IrcException("Bad channel key (+k)", 475);    // ERR_BADCHANNELKEY
            
        members.push_back(ChannelMember(c, false));
        return true;
    }

    bool setTopic(const Client &c, std::string topic) {
        if (topic_lock && !isModerator(c)) 
            throw IrcException("You're not channel operator", 482); // ERR_CHANOPRIVSNEEDED
            
        this->topic = topic;
        return true;
    }

    std::string getName() const {
        return name;
    }

    std::vector<Client *> getMembers() const {
        std::vector<Client *> memberList;
        for (size_t i = 0; i < members.size(); i++)
        {
            memberList.push_back(const_cast<Client *>(&members[i].client));
        }
        return memberList;
    }

    bool setPassword(const Client &c, std::string newPass) {
        if (!isModerator(c))  
            throw IrcException("You're not channel operator", 482); // ERR_CHANOPRIVSNEEDED
            
        this->password = newPass;
        this->locked = !newPass.empty();
        return true;
    }


    void    broadcast(const std::string &message, Client *exclude = NULL)
    {
        for (size_t i = 0; i < members.size(); i++){
            if (exclude && exclude->getSocket() != members[i].client.getSocket())
            {
                send(members[i].client.getSocket(), message.c_str(), message.length(), 0);
            }
        }

    }


};

#endif