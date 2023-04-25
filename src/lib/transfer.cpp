#include "transfer.h"
#include <cctype>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <codecvt>
#include <locale>
#include <iomanip>

std::string TransferHex(uint8_t ch) {
    std::string res = "\\x";
    uint8_t high = ch >> 4;
    uint8_t low = ch & 0x0f;

    if(high < 10) {
        res += (high + '0');
    } else {
        res += (high - 10 + 'A');
    }
    if(low < 10) {
        res += (low + '0');
    } else {
        res += (low - 10 + 'A');
    }
    return res;
}

uint8_t TransBackHex(char ch0, char ch1) {
    ch0 = toupper(ch0);
    ch1 = toupper(ch1);
    if(ch0 >= '0' && ch0 <= '9') {
        ch0 -= '0';
    } else if(ch0 >= 'A' && ch0 <= 'F') {
        ch0 -= 'A' - 10;
    } else {
        throw std::runtime_error("Invalid hex character");
        return 0;
    }
    if(ch1 >= '0' && ch1 <= '9') {
        ch1 -= '0';
    } else if(ch1 >= 'A' && ch1 <= 'F') {
        ch1 -= 'A' - 10;
    } else {
        throw std::runtime_error("Invalid hex character");
        return 0;
    }
    return (ch0 << 4) + ch1;
}

std::string InputTrans(char ch) {
    switch (ch)
    {
        case '*':   return "\\*";
        case '|':   return "\\|";
        case '(':   return "\\(";
        case ')':   return "\\)";
        case '-':   return "\\-";
        case '.':   return "\\.";
        case '\\':  return "\\\\";
        default:    return std::string("") + ch;
    }
}
std::string Char2Str(char ch) {
    std::string str;
    str += ch;
    str = "'" + str + "'";
    return str;
}

// std::string Unprint2Trans(std::string in_str) {
//     std::string str;
//     for(int i = 0; i < in_str.size(); i++) {
//         switch (in_str[i])
//         {
//             case '\n':
//                 str += "\\n";
//                 break;
//             case '\t':
//                 str += "\\t";
//                 break;  
//             case '\r':
//                 str += "\\r";
//                 break;
//             case ' ':
//                 str += "\\_";
//                 break;
//             case TRANSFER_STAR:
//                 str += "\\STAR";
//                 break;
//             case TRANSFER_OR:
//                 str += "\\OR";
//                 break;
//             case TRANSFER_CONCAT:
//                 str += "\\CONCAT";
//                 break;
//             case TRANSFER_DOT:
//                 str += "\\DOT";
//                 break;
//             case TRANSFER_LBRACKET:
//                 str += "\\LBRACKET";
//                 break;
//             case TRANSFER_RBRACKET:
//                 str += "\\RBRACKET";
//                 break;
//             default:
//                 str += in_str[i];
//                 break;  
//         }
//     }
//     return str;
// }

// Encode a token by replacing whitespace and unprintable characters with escape sequences
std::string VisableString(const std::string& token) {
    std::string out;
    for(int i = 0; i < token.size(); i++) {
        switch (token[i])
        {
        case ' ':
            out += "\\s";
            break;
        case '\t':
            out += "\\t";
            break;
        case '\n':
            out += "\\n";
            break;
        case '\r':
            out += "\\r";
            break;
        case '\f':
            out += "\\f";
            break;
        case '\v':
            out += "\\v";
            break;
        default:
            if (std::isprint(token[i])) {
                out += token[i];
            }
            else {
                std::stringstream ss;
                ss << std::hex << +static_cast<unsigned char>(token[i]);
                out += "\\x" + ss.str();
            }
        }
    }
    return out;
}
// Decode a token by replacing escape sequences with whitespace and unprintable characters
std::string AsciiString(const std::string& token) {
    std::string out;
    for(int i = 0; i < token.size(); i++) {
        if (token[i] == '\\') {
            if (i + 1 < token.size()) {
                switch (token[i + 1]) {
                case 's':
                    out += ' ';
                    i++;
                    break;
                case 't':
                    out += '\t';
                    i++;
                    break;
                case 'n':
                    out += '\n';
                    i++;
                    break;
                case 'r':
                    out += '\r';
                    i++;
                    break;
                case 'f':
                    out += '\f';
                    i++;
                    break;
                case 'v':
                    out += '\v';
                    i++;
                    break;
                case 'x': {
                    int code;
                    if (i + 2 < token.size()) {
                        std::istringstream iss(token.substr(i + 2, 2));
                        iss >> std::hex >> code;
                        out += static_cast<unsigned char>(code);
                        i += 3;
                    }
                    else {
                        out += token[i];
                    }
                    break;
                }
                default:
                    out += token[i];
                }
            }
            else {
                out += token[i];
            }
        }
        else {
            out += token[i];
        }
    }
    return out;
}

std::string VisableUtf8String(const std::string& token) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::wstring wstr = conv.from_bytes(token);
    std::string out;
    for (int i = 0; i < wstr.size(); i++) {
        if (wstr[i] < 0x80) {
            out += static_cast<char>(wstr[i]);
        }
        else {
            out += "\\u" + std::to_string(static_cast<int>(wstr[i]));
        }
    }
    return out;
}

std::string Utf8String(const std::string& token) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::wstring wstr;
    for (int i = 0; i < token.size(); i++) {
        if (token[i] == '\\') {
            if (i + 1 < token.size()) {
                switch (token[i + 1]) {
                case 'u': {
                    int code;
                    if (i + 5 < token.size()) {
                        std::istringstream iss(token.substr(i + 2, 4));
                        iss >> std::hex >> code;
                        wstr += static_cast<wchar_t>(code);
                        i += 4;
                    }
                    else {
                        wstr += token[i];
                    }
                    break;
                }
                default:
                    wstr += token[i];
                }
            }
            else {
                wstr += token[i];
            }
        }
        else {
            wstr += token[i];
        }
    }
    return conv.to_bytes(wstr);
}