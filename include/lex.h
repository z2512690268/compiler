#pragma once

#include <istream>
#include <vector>
#include "automachine.h"

struct LexNFANode : public ATMNode<int, std::string, LexNFANode>{
    LexNFANode(int node_count) : ATMNode<int, std::string, LexNFANode>(node_count){
        ch = 0;
        token = "";
    }

    LexNFANode* GetNodePtr() {
        return this;
    }

    int GetInput() {
        return ch;
    }
    void SetInput(const int& input) {
        ch = input;
    }
    std::string PrintInput() {
        if(ch == 0) {
            return "null";
        }
        else {
            return std::to_string(int(ch)) + "/" + char(ch);
        }
    }
    std::string GetToken() {
        return token;
    }

    std::string PrintToken() {
        return token;
    }
    void SetToken(const std::string& token_str) {
        token = token_str;
    }

    int NullDerive() {
        return ch == 0;
    }
    int CanDerive(int in_ch) {
        return in_ch == ch;
    }

    int ch;              // 转移到该节点的边数据
    std::string token;  // 当前节点对应的终结token
};

struct LexDFANode : public ATMNode<int, std::string, LexDFANode>{
    LexDFANode(int node_count) : ATMNode<int, std::string, LexDFANode>(node_count){
        ch = 0;
    }

    LexDFANode* GetNodePtr() {
        return this;
    }

    int GetInput() {
        return ch;
    }
    void SetInput(const int& input) {
        ch = input;
    }

    std::string GetToken() {
        return token;
    }

    void SetToken(const std::string& token_str) {
        token = token_str;
    }

    std::string PrintInput() {
        if(ch == 0) {
            return "null";
        }
        else {
            return std::to_string(int(ch)) + "/" + char(ch);
        }
    }

    std::string PrintToken() {
        std::string ret = "";
        for(auto& i : token) {
            ret += std::to_string(i) + "#";
        }
        return ret;
    }

    int NullDerive() {
        return ch == 0;
    }
    int CanDerive(int in_ch) {
        return in_ch == ch;
    }

    int ch;                            // 转移到该节点的边数据
    std::string token;  // 当前节点对应的终结token
};

using LexNFA = NFA<LexNFANode, LexDFANode, int, std::string>; 

int lexer(std::istream& fin, std::istream& input, std::vector<std::pair<std::string, std::string>>& output);