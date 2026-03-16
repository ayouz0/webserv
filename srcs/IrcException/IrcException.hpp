#ifndef IRCEX_CPP
#define IRCEX_CPP

# include <exception>
# include <string>


class IrcException : public   std::exception {
    std::string _mesasge;
    std::string _context;
    int _code;
    public:
        IrcException(std::string context, std::string message, int code ): _mesasge(message), _context(context), _code(code){}

        virtual const char* what() const throw() {
            return _mesasge.c_str();
        }
        int getCode() const{
            return _code;
        }

        std::string getContext() const{
            return _context;
        }

        virtual ~IrcException() throw() {}
};


#endif