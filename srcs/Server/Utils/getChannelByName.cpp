#include "../Server.hpp"


Channel *Server::getChannelByName(std::string name){
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels.at(i).getName() == name) return &channels.at(i);
    }
    return NULL;
}


