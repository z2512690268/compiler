#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// 获取一行，并按照空格分割，忽略其他字符，多个空格视为一个空格，返回分割后的字符串数组
std::vector<std::string> getLine(std::ifstream &file) {
    std::vector<std::string> result;
    std::string line;
    std::getline(file, line);
    std::string word;
    for (auto c : line) {
        if (c == ' ') {
            if (word != "") {
                result.push_back(word);
                word = "";
            }
        } else {
            word += c;
        }
    }
    if (word != "") {
        result.push_back(word);
    }
    return result;
}