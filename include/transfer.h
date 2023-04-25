#pragma once

#include <cstdint>
#include <string>


std::string TransferHex(uint8_t ch);
uint8_t TransBackHex(char ch0, char ch1);

std::string InputTrans(char ch);
std::string Char2Str(char ch);

std::string Unprint2Trans(std::string in_str);

std::string VisableString(const std::string& token);
std::string AsciiString(const std::string& token);
