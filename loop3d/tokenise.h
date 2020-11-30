#ifndef TOKENISE_H
#define TOKENISE_H
#include <string>
#include <vector>

namespace StringTokenise {

void tokenise(const std::string& str, std::vector<std::string> &tokens, std::string delims=" ");

} // StringTokenise namespace

#endif // TOKENISE_H
