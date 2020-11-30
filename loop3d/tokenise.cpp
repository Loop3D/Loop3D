#include "tokenise.h"

void StringTokenise::tokenise(const std::string& str, std::vector<std::string> &tokens, std::string delims)
{
    char quoteUsed;
    std::string quotes = "\"'";
    std::size_t startPos, delimiterPos, quotePos, endPos = 0;
    while (endPos != std::string::npos) {
        startPos = str.find_first_not_of(delims,endPos);
        quotePos = str.find_first_of(quotes,startPos);
        if (quotePos <= startPos) {
            startPos = quotePos;
            quoteUsed = (quotePos == std::string::npos ? 0 : str[quotePos]);
        } else {
            quoteUsed = 0;
        }
        delimiterPos = str.find_first_of(delims,startPos);
        endPos = (quoteUsed != 0 ? str.find_first_of(quoteUsed, quotePos+1)+1 : delimiterPos);

        if (startPos != std::string::npos) {
            if (quoteUsed) tokens.push_back(str.substr(startPos+1, endPos - startPos - 2));
            else tokens.push_back(str.substr(startPos, endPos - startPos));
        }
    }
}
