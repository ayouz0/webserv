#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "../Client/Client.hpp"
# include <vector>
#include <exception>
# include <iostream>

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
    bool    locked;


    bool    isModerator(const Client &c) const{
        for (size_t i = 0; i < members.size(); i++)
        {
            if (members[i].client.getNickname() == c.getNickname()) return members[i].moderator;
        }
        return false;
    }

public:
    Channel(const Client &creator, std::string name):  topic("no topic is set yet"), topic_lock(false), name(name),invite_only(false), password(""),locked(false)
    {
        this->id = ++counter;
        members.push_back(ChannelMember(creator, true));
    }

    bool joinChannel(const Client &c, std::string password){ // do't forget to try catch
        if (invite_only) throw std::runtime_error("channel is invite only");
        if (locked && password != this->password) throw  std::runtime_error("incorrect password");
        members.push_back(ChannelMember(c, false));
        return true; // !
    }

    bool    setTopic(const Client &c, std::string topic){
        if (topic_lock && !isModerator(c)) throw std::runtime_error("topic is locked you need to be a moderator to change it");
        this->topic = topic;
        return true; //!
    }
};

#endif