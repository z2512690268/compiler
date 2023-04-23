#pragma once

#include <string>

#define TRANSFER_STAR -1
#define TRANSFER_OR -2
#define TRANSFER_LBRACKET -3
#define TRANSFER_RBRACKET -4
#define TRANSFER_CONCAT -5
#define TRANSFER_DOT -6

char Transfer(char ch);

int Transback(char ch);

std::string InputTrans(char ch);
std::string Char2Str(char ch);

std::string Unprint2Trans(std::string in_str);

std::string VisableString(const std::string& token);
std::string AsciiString(const std::string& token);
