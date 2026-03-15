#include "bonus.hpp"

bool validateSecurity(const std::string &prompt) {
    std::string dangerous = "|&;'\"/$()><{}";
    
    if (prompt.find_first_of(dangerous) != std::string::npos) {
        return false;
    }
    return true;
}

std::string chatWithAI(const std::string &prompt)
{
    if (validateSecurity(prompt) == false){
        return "invalid character found\n";
    }

    std::string command = " curl -X POST https://ft-irc-bot.vercel.app/prompt -d 'prompt=" + prompt + "'";
    std::string result;
    char buffer[128];

    FILE *curlFile = popen(command.c_str(), "r");

    if (!curlFile){
        return "something is wrong";
    }

    while (fgets(buffer, 128, curlFile) != NULL) {
        std::cout << "Reading..." << std::endl;
        result += buffer;
    }
    pclose(curlFile);
    return result;

}
