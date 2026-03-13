#include "bonus.hpp"


std::string BotUtils::jokes[BotUtils::JOKE_COUNT] = {
    "Why do programmers prefer dark mode? Because light attracts bugs.",
    "There are 10 types of people: those who understand binary and those who don't.",
    "I would tell you a UDP joke, but you might not get it.",
    "A SQL query walks into a bar, walks up to two tables and asks: 'Can I join you?'",
    "Debugging: being the detective in a crime movie where you are also the murderer.",
    "I changed my password to 'incorrect' so when I forget it, the computer reminds me.",
    "Why did the developer go broke? Because he used up all his cache.",
    "What bug a plumber can't solve? A memory leak."
};

std::string BotUtils::facts[BotUtils::FACT_COUNT] = {
    "The first computer bug was an actual moth found in a computer.",
    "The first website is still online at info.cern.ch.",
    "Email existed before the World Wide Web.",
    "Linux powers most of the world's servers.",
    "C++ was originally called C with Classes."
};

std::string BotUtils::advices[BotUtils::ADVICE_COUNT] =
{
    "Always write clean code; future you will thank present you.",
    "Test your code early and often to catch bugs before production.",
    "Read other people's code to learn new patterns and best practices.",
    "Take breaks when debugging; a fresh perspective solves problems faster.",
    "Document your code; it's a gift to your future self and teammates."
};

std::string BotUtils::quotes[BotUtils::QUOTE_COUNT] =
{
    "The only way to do great work is to love what you do. - Steve Jobs",
    "Innovation distinguishes between a leader and a follower. - Steve Jobs",
    "Life is what happens when you're busy making other plans. - John Lennon",
    "The future belongs to those who believe in the beauty of their dreams. - Eleanor Roosevelt",
    "It is during our darkest moments that we must focus to see the light. - Aristotle"
};



const std::string &BotUtils::randomJoke()
{
    return jokes[std::rand() % JOKE_COUNT];
}

const std::string &BotUtils::randomFact()
{
    return facts[std::rand() % FACT_COUNT];
}
const std::string &BotUtils::randomAdvice()
{
    return advices[std::rand() % ADVICE_COUNT];
}

const std::string &BotUtils::randomQuote()
{
    return quotes[std::rand() % QUOTE_COUNT];
}



std::string chatWithBot(const std::string &message)
{
    std::string lowerMessage = message;
    std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(), ::tolower);

    if (lowerMessage == "hello" || lowerMessage == "hi")
    {
        return "Hey there! 👋 Ready to break the internet with me?";
    }
    else if (lowerMessage == "help")
    {
        return "Allowed commads: joke, time, date, fact, quote, advice";
    }

    else if (lowerMessage == "joke")
    {
        return BotUtils::randomJoke();
    }
    else if(lowerMessage == "fact")
    {
        return BotUtils::randomFact();
    }
    else if (lowerMessage == "date" || lowerMessage == "time")
    {
        std::time_t now = std::time(NULL);
        std::tm* timeinfo = std::localtime(&now);
        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        return std::string("📅 Date and time: ") + buffer;
    }
    else if (lowerMessage == "advice")
    {
        return BotUtils::randomAdvice();
    }
    else if (lowerMessage == "quote")
    {
        return BotUtils::randomQuote();
    }
    else
    {
        return "I'm a simple bot, not a genius! 🤖 Try asking me about jokes, facts, or the time instead. For the deep stuff, ChatGPT's got your back: https://chatgpt.com/?q=" + message;
    }
}