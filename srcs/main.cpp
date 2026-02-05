#include "Server/Server.hpp"


int main(int ac, const char **av){

    if (ac != 3){
        std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
        return 1;
    }
    try{
        Server server(av[1], av[2]);
    }
    catch (std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}