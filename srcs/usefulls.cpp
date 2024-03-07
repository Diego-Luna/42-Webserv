
#include "../Includes/Utils.hpp"

void fatal(const char *msg)
{
	    perror(msg);
		exit(EXIT_FAILURE);
}

std::vector<std::string> split(const std::string &input, char c)
{
 	size_t startPosition = 0;
    size_t spacePosition;

    std::vector<std::string> words;
    while ((spacePosition = input.find(c, startPosition)) != std::string::npos) {
        std::string word = input.substr(startPosition, spacePosition - startPosition);
        words.push_back(word);
        startPosition = spacePosition + 1;
    }
    if (startPosition < input.size()) {
        words.push_back(input.substr(startPosition));
    }
    return (words);
}

std::string size_tToString(size_t number) {
    std::stringstream convertedNumber;
    convertedNumber << number;
    return convertedNumber.str();
}