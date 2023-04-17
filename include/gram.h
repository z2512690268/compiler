#pragma once

#include "automachine.h"

struct LR1Item {
    std::string cur_token;              // 当前初始转化字符串
    std::string next_token;             // 下一个输入
    int gram_id;                        // 产生式id
    int stack_pos;                      // 栈顶位置

};

struct GramNFANode : public ATMNode<std::string, LR1Item, GramNFANode> {

    GramNFANode(int node_count) : ATMNode<std::string, LR1Item, GramNFANode>(node_count){
        str = "";
    }
    std::string GetInput() {
        return str;
    }
    void SetInput(const std::string& input_str) {
        str = input_str;
    }
    std::string PrintInput() {
        return str;
    }
    LR1Item GetToken() {
        return token;
    }

    void SetToken(const LR1Item& token_str) {
        token = token_str;
    }

    std::string PrintToken() {
        std::string ret;
        ret += token.cur_token + ":" + token.next_token + ":" + std::to_string(token.gram_id) + ":" + std::to_string(token.stack_pos);
    }

    std::string PrintSelf(std::unordered_map<std::string, std::vector<std::vector<std::string>> >& grams) {
        std::string ret;
        ret += std::to_string(this->id) + " ";
        ret += token.cur_token + ":";
        // std::cout << this->id << " : ";
        std::vector<std::string>& gram = grams[token.cur_token][token.gram_id];
        for(int i = 0; i < gram.size(); i++) {
            if(i == token.stack_pos) {
                ret += " . ";
            }
            ret += gram[i] + " ";
        }
        if(token.stack_pos == gram.size()) {
            ret += ". ";
        }
        ret += ": " + token.next_token;    
        return ret;
    }

    void Print(std::unordered_map<std::string, std::vector<std::vector<std::string>> >& grams, std::unordered_map<int, int>& flag) {
        flag[this->id] = 1;
        std::string ret;
        ret = this->PrintSelf(grams);
        std::cout << ret << std::endl;
        for(int i = 0; i < this->next.size(); i++) {
            std::cout << "\t" << "next: " << this->next[i]->id << std::endl;
        }
        for(int i = 0; i < this->next.size(); i++) {
            if(flag.find(this->next[i]->id) == flag.end())
                this->next[i]->Print(grams, flag);
        }
    }


    int NullDerive() {
        return str == "";
    }
    int CanDerive(std::string in_str) {
        return in_str == str;
    }


    std::string str;              // 转移到该节点的边数据
    LR1Item token;
};

template<typename GramNFAType>
struct GramDFANode : public ATMNode<std::string, std::string, GramDFANode<GramNFANode>> {
    GramDFANode(int node_count) : ATMNode<std::string, std::string, GramDFANode<GramNFANode>>(node_count){
        str = "";
    }
    std::string GetInput() {
        return str;
    }
    void SetInput(const std::string& input_str) {
        str = input_str;
    }
    std::string PrintString() {
        return str;
    }
    std::string GetToken() {
        return token;
    }

    void SetToken(const std::string& token_str) {
        token = token_str;
    }

    std::string PrintToken() {
        return token;
    }

    std::string PrintInput() {
        return str;
    }
    int NullDerive() {
        return str == "";
    }
    int CanDerive(std::string in_str) {
        return in_str == str;
    }

    std::string PrintSelf(std::unordered_map<std::string, std::vector<std::vector<std::string>> >& grams, 
                            NFA<GramNFANode, GramDFANode<GramNFANode>, std::string, LR1Item>& nfa) {
        std::string ret;
        std::vector<GramNFANode*> state_vec;
        nfa.State2Vector(token, state_vec);
        ret += token + ":\n";
        for(auto& state : state_vec) {
            ret += "\t" + state->PrintSelf(grams) + "\n";
        }
        return ret;
    }

    void Print(std::unordered_map<std::string, std::vector<std::vector<std::string>> >& grams, std::unordered_map<int, int>& flag,
                NFA<GramNFANode, GramDFANode<GramNFANode>, std::string, LR1Item>& nfa) {
        flag[this->id] = 1;
        std::cout << "*****************************************************" << std::endl;
        std::cout << "$$$ node:" << this->id  << " input: " << str << std::endl << std::endl;
        std::string ret;
        ret = this->PrintSelf(grams, nfa);
        std::cout << ret << std::endl;
        for(int i = 0; i < this->next.size(); i++) {
            std::cout << "\t" << "next: " << this->next[i]->str << " " << this->next[i]->id << std::endl;
        }
        std::cout << "*****************************************************" << std::endl;
        for(int i = 0; i < this->next.size(); i++) {
            if(flag.find(this->next[i]->id) == flag.end())
                this->next[i]->Print(grams, flag, nfa);
        }
    }


    void State2Vector(std::string str, std::vector<GramNFANode*>& vec) {
        vec.clear();
        int pos = 0;
        while(pos < str.size()) {
            int next_pos = str.find("#", pos);
            if(next_pos == std::string::npos) {
                next_pos = str.size();
            }
            int id = std::stoi(str.substr(pos, next_pos-pos));
            vec.push_back(this->id2node[id]);
            pos = next_pos+1;
        }
    }

    std::string str;              // 转移到该节点的边数据
    std::string token;
};

using GramNFA = NFA<GramNFANode, GramDFANode<GramNFANode>, std::string, LR1Item>;