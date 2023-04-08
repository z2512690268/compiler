#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <algorithm>

template<typename Edge>
struct Node {
    int id;
    Edge ch;              // 转移到该节点的边数据
    std::string token;  // 当前节点对应的终结token
    std::vector<Node*> next; // 当前节点指向的节点
    Node(int node_count) {
        ch = 0;
        token = "";
        id = node_count;
    }
    Node(int node_count, Edge ch) : ch(ch){
        token = "";
        id = node_count;
    }
    Node(int node_count, Edge ch, std::string token) : ch(ch), token(token) {
        id = node_count;
    }
    void Copy(Node* node) {
        ch = node->ch;
        token = node->token;
        next.clear();
        for(auto& i : node->next) {
            next.push_back(i);
        }
    }
    void Print(std::unordered_map<int, int>& flag) {
        flag[id] = 1;
        std::cout << "id: " << id << " ch:" << char(ch) << " token: " << token << std::endl;
        for(int i = 0; i < next.size(); i++) {
            std::cout << "\t" << "next: " << next[i]->id << std::endl;
        }
        for(int i = 0; i < next.size(); i++) {
            if(flag[next[i]->id] == 0)
                next[i]->Print(flag);
        }
    }
    void GenerateMap(std::unordered_map<int, int>& flag, std::unordered_map<int, Node*>& id2node) {
        id2node[id] = this;
        flag[id] = 1;
        for(int i = 0; i < next.size(); i++) {
            if(flag[next[i]->id] == 0)
                next[i]->GenerateMap(flag, id2node);
        }
    }
};

typedef Node<int> LexNode;

struct AutoMachine {
    LexNode* head;
    int count;
    std::unordered_map<int, LexNode*> id2node;
    AutoMachine() {
        count = 0;
        head = nullptr;
    }
    int GetCount() {
        count++;
        return count;
    }
    void GenerateMap() {
        std::unordered_map<int, int> flag;
        id2node.clear();
        head->GenerateMap(flag, id2node);
    }
};

std::string Vector2State(std::vector<LexNode*>& vec) {
    std::string str = "";
    std::vector<int> id_vec;
    for(int i = 0; i < vec.size(); i++) {
        id_vec.push_back(vec[i]->id);
    }
    std::sort(id_vec.begin(), id_vec.end());
    // 将状态从小到大排序
    for(int i = 0; i < id_vec.size(); i++) {
        str += std::to_string(id_vec[i]);
        if(i != id_vec.size() - 1)
            str += "#";
    }
    return str;
}

void State2Vector(std::string str, std::vector<LexNode*>& vec, AutoMachine& NFA) {
    vec.clear();
    int pos = 0;
    while(pos < str.size()) {
        int next_pos = str.find("#", pos);
        if(next_pos == std::string::npos) {
            next_pos = str.size();
        }
        int id = std::stoi(str.substr(pos, next_pos-pos));
        vec.push_back(NFA.id2node[id]);
        pos = next_pos+1;
    }
}
