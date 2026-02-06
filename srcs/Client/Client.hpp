#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <string>

class Client{
    private:
        std::string messageBuffer;
        std::string _username;
        int         _socket;
        std::string _nickname;

        std::string ipAdress; // aalahyan was here


        bool    _hasPassword;
        bool    _hasNickname;
        bool    _hasUsername;
        bool    _isRegistered;


        void _init(){ // aalahyan was here
            _hasPassword = false;
            _hasNickname = false;
            _hasUsername = false;
            _isRegistered = false;
        }



    public:
            Client();
            Client (int fd, std::string const &nickname);
            Client(int fd, std::string nickname, std::string ipA){
                _init();
                this->_socket = fd;
                this->_nickname = nickname;
                this->ipAdress = ipA;
            } // aalahyan was here: i didn't want to miss with orogonal construuctor but SHIIT
            ~Client();
            
            std::string getIpAddress(){ // aalahyan was here
                return ipAdress;
            }
            void        appendToBuffer(std::string const &data){this->messageBuffer += data;};
            std::string &getBuffer() {return messageBuffer;}; 

            std::string getNickname() const {return _nickname;};
            void        setNickname(std::string const &nickname) {_nickname = nickname;};


            void        setUsername(const std::string &username) { _username = username; };
            std::string getUsername() const { return _username; };

            int         getSocket() const { return _socket; };
            
            std::string getNextCommandFromBuffer(){
                size_t pos = messageBuffer.find("\r\n");
                if (pos != std::string::npos) {
                    std::string command = messageBuffer.substr(0, pos);
                    messageBuffer.erase(0, pos + 2); // +2 to remove the \r\n
                    return command;
                }
                std::string empty = "";
                return empty;
            };

            void setPassState(bool state) { _hasPassword = state; }
            bool getPassState() const { return _hasPassword; }

            void setNickState(bool state) { _hasNickname = state; }
            bool getNickState() const { return _hasNickname; }

            void setUserState(bool state) { _hasUsername = state; }
            bool getUserState() const { return _hasUsername; }

            bool canRegister() const {
                return (_hasPassword && _hasNickname && _hasUsername && !_isRegistered);
            }


            void setRegistered(bool state) { _isRegistered = state; }
            bool isRegistered() const { return _isRegistered; }
};  

#endif