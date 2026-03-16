#ifndef BONUS_HPP
#define BONUS_HPP

#include <string>
#include <algorithm>
#include <ctime>

#define BOT_NAME "bot"

std::string chatWithBot(const std::string &message);

class BotUtils
{
private:
    static const int JOKE_COUNT = 8;
    static const int FACT_COUNT = 5;
    static const int QUOTE_COUNT = 5;
    static const int ADVICE_COUNT = 5;

    static std::string jokes[JOKE_COUNT];
    static std::string facts[FACT_COUNT];
    static std::string advices[ADVICE_COUNT];
    static std::string quotes[QUOTE_COUNT];

public:
    static const std::string &randomJoke();
    static const std::string &randomFact();
    static const std::string &randomQuote();
    static const std::string &randomAdvice();
};

#endif