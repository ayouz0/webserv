#ifndef IRCEX_CPP
#define IRCEX_CPP

# include <exception>
# include <string>


class IrcException : public   std::exception {
    std::string _mesasge;
    int _code;
    public:
        IrcException(std::string message, int code ): _mesasge(message), _code(code){}

        virtual const char* what() const throw() {
            return _mesasge.c_str();
        }
        int getCode() const{
            return _code;
        }

        virtual ~IrcException() throw() {}

};


#endif