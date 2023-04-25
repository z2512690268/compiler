#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>

template <typename SubClassType>
struct BaseNode {
    BaseNode(int node_count) {
        id = node_count;
    }
    size_t GetNextSize() {
        return next.size();
    }
    virtual void AddNext(SubClassType* newNext) {
        next.push_back(newNext);
    }
    SubClassType* GetNext(int index) {
        return next[index];
    }
    virtual void Print(std::unordered_map<int, int>& flag){
        flag[id] = 1;
        std::cout << "id: " << id << std::endl;
        for(int i = 0; i < next.size(); i++) {
            std::cout << "\t" << "next: " << next[i]->id << std::endl;
        }
        for(int i = 0; i < next.size(); i++) {
            if(flag[next[i]->id] == 0)
                next[i]->BaseNode::Print(flag);
        }
    }

    int id;
    std::vector<SubClassType*> next; // 当前节点指向的节点
};


// 树节点
template<typename NodeType>
struct Tree
{
    NodeType* head;
    int count;
    Tree() {
        count = 0;
        head = nullptr;
    }
    virtual int GetCount() {
        count++;
        return count;
    }
    virtual NodeType* NewNode() {
        int id = GetCount();
        NodeType* newnode = new  NodeType(id);
        return newnode;
    }
};
