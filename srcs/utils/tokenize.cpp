#include "utils.hpp"

static std::string *getNexToken(const std::string &m, int &i)
{
    bool break_space = true;
    int len = m.length();

    while (i < len && std::isspace(m.at(i)))
        i++;
    int start = i;
    while (i < len)
    {
        if (m.at(i) == ':' && break_space)
        {
            break_space = false;
            start = ++i;
            continue;
        }

        if (std::isspace(m.at(i)) && break_space)
            break;
        i++;
    }

    if (start != i)
    {
        return  new std::string(m.substr(start, i - start));
    }
    return NULL;
}


std::vector<std::string> Tokenize(const std::string &message)
{

    std::vector<std::string> tokens;
    int i = 0;

    while (std::string *s = getNexToken(message.c_str(), i))
    {
        tokens.push_back(std::string(*s));
        delete s;
    }
    return tokens;
}



// int main(){
//     std::string s("hhdh :hdhdh: dhdh:");

//     std::vector<std::string> toks = Tokenize(s);

//     for (std::vector<std::string>::iterator it = toks.begin(); it != toks.end(); it++)
//     {
//         std::cout << "tok: " << *it << "\n";
//     }

// }