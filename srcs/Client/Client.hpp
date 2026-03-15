#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <string>

class Client{
    private:
        std::string inboundBuffer;
        std::string outboundBuffer;
        std::string _username;
        int         _socket;
        std::string _nickname;

        std::string ipAdress; // aalahyan was here

        const unsigned long UID;

        
        
        bool    _hasPassword;
        bool    _hasNickname;
        bool    _hasUsername;
        bool    _isRegistered;
        
        
        void _init(); // aalahyan was here
        
        
        static unsigned long    nextUID;

    public:
            Client();
            Client (int fd, std::string const &nickname);
            Client(int fd, std::string nickname, std::string ipA); // aalahyan was here: i didn't want to miss with orogonal construuctor but SHIIT
            ~Client();
            std::string getIpAddress(); // aalahyan was here

            unsigned long   getUID() const;

            void        appendToInboundBuffer(std::string const &data);
            std::string &getInboundBuffer();

            void        appendToOutboundBuffer(std::string const &data);
            bool        hasPendingOutput() const;
            std::string &getOutboundBuffer();

            std::string getNickname() const;
            void        setNickname(std::string const &nickname);


            void        setUsername(const std::string &username);
            std::string getUsername() const;

            int         getSocket() const;

            std::string getNextCommandFromInboundBuffer();

            void setPassState(bool state);
            bool getPassState() const;

            void setNickState(bool state);
            bool getNickState() const;

            void setUserState(bool state);
            bool getUserState() const;

            bool canRegister() const;


            void setRegistered(bool state);
            bool isRegistered() const;
};

#endif