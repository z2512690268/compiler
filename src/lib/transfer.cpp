#include "transfer.h"

char Transfer(char ch) {
    switch (ch)
    {
        case '*':   return TRANSFER_STAR;
        case '|':   return TRANSFER_OR;
        case '(':   return TRANSFER_LBRACKET;
        case ')':   return TRANSFER_RBRACKET;
        case '-':   return TRANSFER_CONCAT;
        case '.':   return TRANSFER_DOT;
        default:    return ch;
    }
}

int Transback(char ch) {
    switch (ch)
    {
        case TRANSFER_STAR:     return '*';
        case TRANSFER_OR:       return '|';
        case TRANSFER_LBRACKET: return '(';
        case TRANSFER_RBRACKET: return ')';
        case TRANSFER_CONCAT:   return '-';
        case TRANSFER_DOT:   return '.';
        default:                return ch;
    }
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

std::string Unprint2Trans(std::string in_str) {
    std::string str;
    for(int i = 0; i < in_str.size(); i++) {
        switch (in_str[i])
        {
            case '\n':
                str += "\\n";
                break;
            case '\t':
                str += "\\t";
                break;  
            case '\r':
                str += "\\r";
                break;
            case ' ':
                str += "\\_";
                break;
            case TRANSFER_STAR:
                str += "\\STAR";
                break;
            case TRANSFER_OR:
                str += "\\OR";
                break;
            case TRANSFER_CONCAT:
                str += "\\CONCAT";
                break;
            case TRANSFER_DOT:
                str += "\\DOT";
                break;
            case TRANSFER_LBRACKET:
                str += "\\LBRACKET";
                break;
            case TRANSFER_RBRACKET:
                str += "\\RBRACKET";
                break;
            default:
                str += in_str[i];
                break;  
        }
    }
    return str;
}