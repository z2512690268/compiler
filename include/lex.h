#pragma once

#include <istream>
#include <vector>

int lexer(std::istream& fin, std::istream& input, std::vector<std::pair<std::string, std::string>>& output);