#include "./utils.hpp"

std::vector<std::string>    splitter(const std::string &str, char delimiter){
    std::vector<std::string> result;
    std::string token;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == delimiter) {
            if (!token.empty()) {
                result.push_back(token);
                token.clear();
            }
        } else {
            token += str[i];
        }
    }
    if (!token.empty()) {
        result.push_back(token);
    }
    return result;
}