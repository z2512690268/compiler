#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <queue>
#include <algorithm>
#include "graph.h"
#include "debug.h"

template<typename InputType, typename TokenType, typename SubClassType>
struct ATMNode : BaseNode<SubClassType>{
    ATMNode<InputType, TokenType, SubClassType> (int node_count) : BaseNode<SubClassType>(node_count){
    }

    virtual int NullDerive() = 0;
    virtual int CanDerive(InputType in_ch) = 0;
    virtual InputType GetInput() = 0;
    virtual std::string PrintInput() = 0;
    virtual void SetInput(const InputType& input) = 0;
    virtual TokenType GetToken() = 0;
    virtual std::string PrintToken() = 0;
    virtual void SetToken(const TokenType& token) = 0;


    virtual void Copy(SubClassType* node) {
        SetInput(node->GetInput());
        SetToken(node->GetToken());
        this->next.clear();
        for(auto& i : node->next) {
            this->next.push_back(i);
        }
    }

    virtual void AllDeriveInput(std::vector<InputType>& ch_vec) {
        for(auto& pnode : this->next) {
            if(!pnode->NullDerive()) {
                ch_vec.push_back(pnode->GetInput());
            }
        }      
    };
    virtual void AllDeriveId(std::vector<int>& id_vec) {
        for(auto& pnode : this->next) {
            if(pnode->NullDerive()) {
                id_vec.push_back(pnode->id);
            }
        }
    }
    virtual void DeriveByInput(std::vector<int>& id_vec, InputType input) {
        for(auto& pnode : this->next) {
            if(pnode->CanDerive(input)) {
                id_vec.push_back(pnode->id);
            }
        }
    }
    virtual void Print(std::unordered_map<int, int>& flag) {
        flag[this->id] = 1;
        std::cout << "id: " << this->id << " input:" << PrintInput() << " token: " << PrintToken() << std::endl;
        for(int i = 0; i < this->next.size(); i++) {
            std::cout << "\t" << "next: " << this->next[i]->id << std::endl;
        }
        for(int i = 0; i < this->next.size(); i++) {
            if(flag.find(this->next[i]->id) == flag.end())
                this->next[i]->ATMNode<InputType, TokenType, SubClassType>::Print(flag);
        }
    }
};


template <typename NodeType>
struct AutoMachine {
    NodeType* head;
    int count;
    std::unordered_map<int, NodeType*> id2node;
    AutoMachine() {
        count = 0;
        head = nullptr;
    }
    int GetCount() {
        count++;
        return count;
    }
    NodeType* NewNode() {
        int id = GetCount();
        NodeType* newnode = new NodeType(id);
        id2node[id] = newnode;
        return newnode;
    }
};

template <typename NodeType>
struct DFA : public AutoMachine<NodeType> {
};


template <typename NodeType, typename DFANodeType, typename InputType, typename TokenType>
struct NFA : public AutoMachine<NodeType> {
    DFA<DFANodeType> DFA;

    void ExpandStates(std::vector<NodeType*>& states) {
        std::unordered_map<int, int> id_map;
        for(int i = 0; i < states.size(); i++) {
            id_map[states[i]->id] = 1;
        }
        for(int i = 0; i < states.size(); i++) {
            std::vector<int> next_ids;
            states[i]->AllDeriveId(next_ids);
            for(int j = 0; j < next_ids.size(); j++) {
                if(id_map.find(next_ids[j]) == id_map.end()) {
                    id_map[next_ids[j]] = 1;
                    states.push_back(this->id2node[next_ids[j]]);
                }
            }
        }
    }

    void GetAllNextInputs(std::vector<NodeType*>& states, std::vector<InputType>& ch_vec) {
        std::unordered_map<InputType, int> ch_map;
        for(int i = 0; i < states.size(); i++) {
            std::vector<InputType> next_chs;
            states[i]->AllDeriveInput(next_chs);
            for(int j = 0; j < next_chs.size(); j++) {
                if(ch_map.find(next_chs[j]) == ch_map.end()) {
                    ch_map[next_chs[j]] = 1;
                    ch_vec.push_back(next_chs[j]);
                }
            }
        }
    }

    void DeriveByNextInput(std::vector<NodeType*>& cur_states, std::vector<NodeType*>& next_states, InputType input) {
        std::unordered_map<int, int> id_map;
        for(int i = 0; i < cur_states.size(); i++) {
            std::vector<int> next_ids;
            cur_states[i]->DeriveByInput(next_ids, input);
            for(int j = 0; j < next_ids.size(); j++) {
                if(id_map.find(next_ids[j]) == id_map.end()) {
                    next_states.push_back(this->id2node[next_ids[j]]);
                    id_map[next_ids[j]] = 1;
                }
            }
        }
    }

    // void GetAllStateTokens(std::vector<NodeType*>& cur_states, std::vector<TokenType>& tokens) {
    //     std::unordered_map<std::string, int> token_map;
    //     for(int i = 0; i < cur_states.size(); ++i) {
    //         if(cur_states[i]->token != "") {
    //             if(token_map.find(cur_states[i]->token) == token_map.end()) {
    //                 tokens.push_back(cur_states[i]->token);
    //                 token_map[cur_states[i]->token] = 1;
    //             }
    //         }
    //     }
    // }

    int GenerateDFA() {
        std::unordered_map<std::string, DFANodeType*> states2node;

        std::queue<std::pair<std::string, InputType> > q;
        std::vector<NodeType*> cur_state_vec;
        std::string cur_state_string;
        // 拿到初始状态的最终状态
        cur_state_vec.push_back(this->head);
        ExpandStates(cur_state_vec);
        std::vector<std::string> cur_tokens;
        // GetAllStateTokens(cur_state_vec, cur_tokens);
        // if(cur_tokens.size() > 0) {
        //     std::cout << "ERROR! null string may match some regex, which would lead to nondeterministic result!" << std::endl;
        //     std::cout << "The error rules: ";
        //     for(int i = 0; i < cur_tokens.size(); ++i) {
        //         std::cout << cur_tokens[i] << " ";
        //     }
        //     std::cout << std::endl;
        //     return 1;
        // }
        // 初始状态的转移边
        std::vector<InputType> ch_vec;
        GetAllNextInputs(cur_state_vec, ch_vec);
        // 压入栈中
        cur_state_string = Vector2State(cur_state_vec);
        DFANodeType* newNode = DFA.NewNode();
        for(int i = 0; i < ch_vec.size(); i++) {
            q.push(std::make_pair(cur_state_string, ch_vec[i]));
        }
        // 为DFA创建头节点
        newNode->token = cur_state_string;
        states2node[cur_state_string] = newNode;
        DFA.head = newNode;
        // cur_state_string = Vector2State(cur_state_vec);
        // q.push(std::make_pair(cur_state_string, InputType()));
        int cnt = 0;
        while(!q.empty()) {
            DEBUG_ATM_NFA2DFA std::cout << "Enter While, q has " << q.size() << " elements" << std::endl;
            std::pair<std::string, InputType> qtop = q.front();
            q.pop();
            // DEBUG_ATM_NFA2DFA std::cout << "queue top: cur_state_string:" << qtop.first << " " << "char:" << qtop.second.PrintInput() << std::endl;
            cur_state_vec.clear();
            State2Vector(qtop.first, cur_state_vec);
            std::vector<NodeType*> next_state_vec;
            // 转移单步
            DeriveByNextInput(cur_state_vec, next_state_vec, qtop.second);

            // 转移空
            ExpandStates(next_state_vec);
            std::vector<InputType> ch_vec;
            GetAllNextInputs(next_state_vec, ch_vec);
            std::string next_state_string = Vector2State(next_state_vec);

            // DEBUG_ATM_NFA2DFA std::cout << "Transfered state:" << next_state_string << std::endl;
            // DEBUG_ATM_NFA2DFA {
            //     std::cout << "may input char list:";
            //     for(auto& ch : ch_vec){
            //         std::cout << char(ch);
            //     }
            //     std::cout << std::endl;
            // }
            // std::cout << "#3# " << next_state_string << std::endl;
            if(states2node.find(next_state_string) == states2node.end()) {
                // 新边未处理过, 则先建好新节点，再重新转移
                DEBUG_ATM_NFA2DFA std::cout << "Set as a new State" << std::endl;
                DFANodeType* newNode = DFA.NewNode();
                newNode->SetInput(qtop.second);
                newNode->token = (next_state_string);
                states2node[next_state_string] = newNode;
                states2node[qtop.first]->AddNext(newNode);
                for(int i = 0; i < ch_vec.size(); i++) {
                    q.push(std::make_pair(next_state_string, ch_vec[i]));
                }
            } else{
                // 新边已处理过，直接转移
                states2node[qtop.first]->AddNext(states2node[next_state_string]);
            }
            // }
            DEBUG_ATM_NFA2DFA std::cout << std::endl << std::endl;
        }
        return 0;
    }


    std::string Vector2State(std::vector<NodeType*>& vec) {
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

    void State2Vector(std::string str, std::vector<NodeType*>& vec) {
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

};
