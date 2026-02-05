#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>

/// @brief  breaks client message into tokens
/// @param  string constructed from buffer
/// @return vector of tokens
std::vector<std::string>    Tokenize(const std::string &message);

#endif