#pragma once

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <fstream>
#include <algorithm>
#include "defs.h"
#include "lex_debug.h"

#define TRANSFER_STAR -1
#define TRANSFER_OR -2
#define TRANSFER_LBRACKET -3
#define TRANSFER_RBRACKET -4
#define TRANSFER_CONCAT -5
#define TRANSFER_DOT -6

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

struct Node {
    int ch;   // 当前节点包含的数据
    int id;
    std::string token;  // 当前节点对应的token
    std::vector<Node*> next; // 当前节点指向的节点
    Node(int node_count) {
        ch = 0;
        token = "";
        id = node_count;
    }
    Node(int node_count, int ch) : ch(ch){
        token = "";
        id = node_count;
    }
    Node(int node_count, int ch, std::string token) : ch(ch), token(token) {
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

struct AutoMachine {
    Node* head;
    int count;
    std::unordered_map<int, Node*> id2node;
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

std::string Vector2State(std::vector<Node*>& vec) {
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

void State2Vector(std::string str, std::vector<Node*>& vec, AutoMachine& NFA) {
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

int lexer(std::ifstream& fin, std::ifstream& input, std::vector<std::pair<std::string, std::string>>& output)
{
    // paser rule file to NFA
    // rule regex defination: 
    //  support |, *, (), default concat, \n, \t, \\, \:/**/
    // example:
    //first_rule:a|b
    //second_rule:(a|b)*
    //third_rule:ab(sd)*a|b
    AutoMachine NFA;
    NFA.head = new Node(NFA.GetCount());

    std::string buffer;

    std::vector<Node*> start_stack;
    std::vector<Node*> end_stack;
    std::vector<std::string> token_list;
    
    int cnt = 1;
    while(std::getline(fin, buffer)) {
        if(buffer.empty() || buffer[0] == '#')
            continue;
        // std::cout << buffer << std::endl;
        int pos = buffer.find(":");
        std::string token = buffer.substr(0, pos);
        token_list.push_back(token);
        std::string regex = buffer.substr(pos+1);

        // regex预处理
        // ()\*|
        // 前后加括号
        regex = "(" + regex + ")";
        DEBUG_NEWREGEX std::cout << "Add Bracket:" << Unprint2Trans(regex) << std::endl;
        // 处理-运算符, 将a-z转换为(a|b|c|...|z), 只支持两侧均为小写字母或大写字母或数字，且左侧小于等于右侧的情况，否则报错
        
        for(int i = 0; i < regex.size(); i++) {
            if(regex[i] == '-') {  
                if(i == 0 || i == regex.size() - 1) {
                    std::cout << "Error: - must be between two characters" << std::endl;
                    return 1;
                }
                int judger = (regex[i-1] <= regex[i+1]);
                if(judger) {
                    std::string pre = regex.substr(0, i - 1);
                    std::string post = regex.substr(i + 2); 
                    std::string new_regex;
                    new_regex += '(';
                    for(char ch = regex[i-1]; ch <= regex[i+1]; ch++) {
                        new_regex += InputTrans(ch);
                        if(ch != regex[i+1])
                            new_regex += '|';
                    }
                    new_regex += ')';
                    regex = pre + new_regex + post;
                    i = (pre + new_regex).size() - 1;
                } else {
                    std::cout << "Error: - must be between two characters and left <= right" << std::endl;
                    return 1;
                }
            }
        }
        DEBUG_NEWREGEX std::cout << "Expand:" << Unprint2Trans(regex) << std::endl;

        // 处理转义
        std::string new_regex;
        for(int i = 0; i < regex.size(); i++) {
            if(regex[i] == '\\') {
                if(i == regex.size() - 1) {
                    std::cout << "Error: \\ must be followed by a character" << std::endl;
                    return 1;
                }
                i++;
                switch(regex[i]) {
                    case 'n':
                        new_regex += '\n';
                        break;
                    case 't':
                        new_regex += '\t';
                        break;
                    case 'r':
                        new_regex += '\r';
                        break;
                    case '\\':
                        new_regex += '\\';
                        break;
                    case 'd':
                        new_regex += "(";
                        for(char ch = '0'; ch <= '9'; ch++) {
                            new_regex += ch;
                            if(ch != '9')
                                new_regex += '|';
                        }
                        new_regex += ")";
                        break;
                    case 'w':
                        new_regex += "(";
                        for(char ch = 'a'; ch <= 'z'; ch++) {
                            new_regex += ch;
                            new_regex += '|';
                        }
                        for(char ch = 'A'; ch <= 'Z'; ch++) {
                            new_regex += ch;
                            new_regex += '|';
                        }
                        for(char ch = '0'; ch <= '9'; ch++) {
                            new_regex += ch;
                            new_regex += '|';
                        }
                        new_regex += "_)";
                        break;
                    default:
                        new_regex += Transfer(regex[i]);
                        break;
                }
            } else {
                new_regex += regex[i];
            }
        }
        regex = new_regex;

        DEBUG_NEWREGEX std::cout << "Transfer:" << Unprint2Trans(regex) << std::endl;

        // 处理通配符.
        new_regex.clear();
        for(int i = 0; i < regex.size(); ++i){
            if(regex[i] == '.') {
                new_regex += "(";
                for(char ch = 0x20; ch <= 0x7e; ch++) {
                    new_regex += Transfer(ch);
                    if(ch != 0x7e)
                        new_regex += '|';
                }
                new_regex += ")";
            } else {
                new_regex += regex[i];
            }
        }
        regex = new_regex;

        DEBUG_NEWREGEX std::cout << "general match sign:" << Unprint2Trans(regex) << std::endl;
        DEBUG_NEWREGEX std::cout << "End the regex" << std::endl << std::endl;


        // std::cout << token << " " << regex << std::endl;
        // regex paser to NFA
        Node* start, *end;
        Node* newNode = new Node(NFA.GetCount());
        NFA.head->next.push_back(newNode);
        start = newNode;
        end = newNode;
        for(int i = 0; i < regex.size(); i++) {
            if(regex[i] == '(') {
                // 左括号压一个start栈
                Node* newNode = new Node(NFA.GetCount());
                end->next.push_back(newNode);
                start = newNode;
                end = newNode;
                start_stack.push_back(end);
            } else if(regex[i] == ')') {
                // 右括号弹一个start栈，压入一个end栈，弹len(start->next)个end栈
                Node* newNode = new Node(NFA.GetCount());
                end_stack.push_back(end);
                if(start_stack.empty()) {
                    std::cout << "error: " << buffer << std::endl;
                    return 1;
                }
                start = start_stack.back();
                start_stack.pop_back();
                for(int j = 0; j < start->next.size(); j++) {
                    if(end_stack.empty()) {
                        std::cout << "error: " << buffer << std::endl;
                        return 1;
                    }
                    Node* endtop = end_stack.back();
                    end_stack.pop_back();
                    endtop->next.push_back(newNode);
                }
                end = newNode;
            } else if(regex[i] == '|') {
                // |压入一个end栈
                end_stack.push_back(end);
                end = start_stack.back();
                start = end;
            } else if(regex[i] == '*') {
                // start, end循环
                Node* newNode1 = new Node(NFA.GetCount());
                Node* newNode2 = new Node(NFA.GetCount());
                newNode1->Copy(start);
                start->Copy(newNode2);
                if(start == end) {
                    // 单字符+*
                    start->next.push_back(newNode1);
                    start->next.push_back(newNode2);
                    newNode1->next.push_back(newNode1);
                    newNode1->next.push_back(newNode2);
                } else {
                    // 括号+*
                    start->next.push_back(newNode1);
                    start->next.push_back(newNode2);
                    end->next.push_back(newNode1);
                    end->next.push_back(newNode2);
                }
                end = newNode2;
                start = end;
            } else {
                if(regex[i] < 0) {
                    regex[i] = Transback(regex[i]);
                    if(regex[i] < 0) {
                        std::cout << "error: " << buffer << std::endl;
                        return 1;
                    }
                }
                Node* newNode = new Node(NFA.GetCount(), regex[i]);
                end->next.push_back(newNode);
                end = newNode;
                start = end;
            }
        }
        // newNode = new Node();
        // end->next.push_back(newNode);
        // end = newNode;
        end->token = token;
        // std::cout << "step "<< cnt << std::endl;
        // memset(flag, 0, sizeof(flag));
        // NFA.head->Print(flag);
        // std::cout << std::endl;
        // std::cout << std::endl;
        // std::cout << std::endl;
        // std::cout << std::endl;
        // cnt++;
    }

    DEBUG_NFA
    {
        std::cout << "step final:" << std::endl;
        std::unordered_map<int, int> flag;
        NFA.head->Print(flag);
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
    }
        // exit(0);
    // NFA to DFA
    NFA.GenerateMap();
    AutoMachine DFA; 
    std::unordered_map<std::string, Node*> states2node;

    std::queue<std::pair<std::string, int> > q;
    std::vector<Node*> cur_state_vec;
    std::string cur_state_string;
    cur_state_vec.push_back(NFA.head);
    cur_state_string = Vector2State(cur_state_vec);
    q.push(std::make_pair(cur_state_string, 0));
    cnt=1;
    while(!q.empty()) {
        std::pair<std::string, int> qtop = q.front();
        q.pop();
        if(qtop.second == 0) {
            // 若第一次处理，则创建节点，并将完整的当前状态和所有可能的输出字符压入队列
            // 找到所有能空转移的状态（排除重复）
            // std::cout << "hsjkdahkdjhaj " << qtop.first << " " << qtop.second << std::endl;
            std::unordered_map<int, int> id_map;
            cur_state_vec.clear();
            State2Vector(qtop.first, cur_state_vec, NFA);
            // for(int i = 0; i < cur_state_vec.size(); i++) {
            //     std::cout << "ttttssada " << cur_state_vec[i]->id << std::endl;
            //     for(int j = 0; j < cur_state_vec[i]->next.size(); j++) {
            //         std::cout << "ttttssada " << cur_state_vec[i]->next[j]->id << std::endl;
            //     }
            // }
            // 寻找所有可能的转移字符 (排除重复)
            std::unordered_map<int, int> ch_map;
            std::vector<int> ch_vec;
            for(int i = 0; i < cur_state_vec.size(); i++) {
                id_map[cur_state_vec[i]->id] = 1;
            }
            for(int i = 0; i < cur_state_vec.size(); i++) {
                for(int j = 0; j < cur_state_vec[i]->next.size(); j++) {
                    Node* next = cur_state_vec[i]->next[j];
                    if(next->ch == 0) {
                        if(id_map.find(next->id) == id_map.end()) {
                            id_map[next->id] = 1;
                            cur_state_vec.push_back(next);
                        }
                    } else {
                        if(ch_map.find(next->ch) == ch_map.end()) {
                            ch_map[next->ch] = 1;
                            ch_vec.push_back(next->ch);
                        }
                    }
                    // std::cout << "#1# " << i << " " << j << " " << std::endl;
                    // for(int k = 0; k < cur_state_vec.size(); k++) {
                    //     std::cout << "ttttssada " << cur_state_vec[k]->id << std::endl;
                    // }
                    // for(int k = 0; k < ch_vec.size(); k++) {
                    //     std::cout << "dsadaddas " << ch_vec[k] << std::endl;
                    // }
                }
            }
            cur_state_string = Vector2State(cur_state_vec);
            // std::cout << "#2# " << cur_state_string << std::endl;
            if(states2node.find(cur_state_string) == states2node.end()) {
                // 未处理过
                Node* newNode = new Node(DFA.GetCount());
                newNode->token = cur_state_string;
                states2node[cur_state_string] = newNode;
                if(DFA.head == nullptr) {
                    DFA.head = newNode;
                }
                // 将所有可能的转移字符压入队列
                for(int i = 0; i < ch_vec.size(); i++) {
                    q.push(std::make_pair(cur_state_string, ch_vec[i]));
                }
            }
        } else {
            // 若不是第一次处理，则在这里完成转移
            cur_state_vec.clear();
            State2Vector(qtop.first, cur_state_vec, NFA);
            std::vector<Node*> next_state_vec;
            // 转移单步
            std::unordered_map<int, int> id_map;
            for(int i = 0; i < cur_state_vec.size(); i++) {
                for(int j = 0; j < cur_state_vec[i]->next.size(); j++) {
                    Node* next = cur_state_vec[i]->next[j];
                    if(next->ch == qtop.second) {
                        if(id_map.find(next->id) == id_map.end()) {
                            next_state_vec.push_back(next);
                            id_map[next->id] = 1;
                        }
                    }
                }
            }
            // 转移空
            std::unordered_map<int, int> ch_map;
            std::vector<int> ch_vec;
            for(int i = 0; i < next_state_vec.size(); i++) {
                id_map[next_state_vec[i]->id] = 1;
            }
            for(int i = 0; i < next_state_vec.size(); i++) {
                for(int j = 0; j < next_state_vec[i]->next.size(); j++) {
                    Node* next = next_state_vec[i]->next[j];
                    if(next->ch == 0) {
                        if(id_map.find(next->id) == id_map.end()) {
                            id_map[next->id] = 1;
                            next_state_vec.push_back(next);
                        }
                    } else {
                        if(ch_map.find(next->ch) == ch_map.end()) {
                            ch_map[next->ch] = 1;
                            ch_vec.push_back(next->ch);
                        }
                    }
                }
            }
            std::string next_state_string = Vector2State(next_state_vec);
            // std::cout << "#3# " << next_state_string << std::endl;
            if(states2node.find(next_state_string) == states2node.end()) {
                // 新边未处理过, 则先建好新节点，再重新转移
                Node* newNode = new Node(DFA.GetCount(), qtop.second);
                newNode->token = next_state_string;
                states2node[next_state_string] = newNode;
                states2node[qtop.first]->next.push_back(newNode);
                for(int i = 0; i < ch_vec.size(); i++) {
                    q.push(std::make_pair(next_state_string, ch_vec[i]));
                }
            } else{
                // 新边已处理过，直接转移
                states2node[qtop.first]->next.push_back(states2node[next_state_string]);
            }
        } 
        // std::cout << "step "<< cnt << std::endl;
        // memset(flag, 0, sizeof(flag));
        // DFA.head->Print(flag);
        // std::cout << std::endl;
        // std::cout << std::endl;
        // std::cout << std::endl;
        // std::cout << std::endl;
        // cnt++;
        // if(cnt >= 10)
        //     exit(0);
    }
    DEBUG_DFA
    { 
        std::cout << "step "<< "final" << std::endl;
        std::unordered_map<int, int> flag;
        DFA.head->Print(flag);
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
    }
    // parse by DFA
    char ch;
    std::string match;
    std::string token;
    int line = 1;
    Node* cur = DFA.head;
    while(!input.eof()) {
        int i;
        input.get(ch);
        if(input.fail())    {
            DEBUG_MATCH std::cout << "End of input file!" << std::endl;
            break;
        }
        int cur_next_size = cur->next.size();
        // 还能匹配就继续匹配
        DEBUG_MATCH std::cout << std::endl << std::endl;
        DEBUG_MATCH std::cout << "Round Start:" << "Id:" << cur->id << " matched:" << match << std::endl;
        DEBUG_MATCH std::cout << "New Char:" << ch << std::endl;
        for(i = 0; i < cur->next.size(); i++) {
            if(cur->next[i]->ch == ch) {
                cur = cur->next[i];
                match += ch;
                break;
            }
        }
        DEBUG_MATCH if(i != cur_next_size) std::cout << "next state:" << cur->id << " EdgeId:" << i << std::endl;
        if(i == cur_next_size || input.eof()) {
            // 无法匹配
            DEBUG_MATCH std::cout << "Unmatched! StateId:" << cur->id << " NFAStates:" << cur->token << std::endl;
            std::vector<Node*> state_vec;
            State2Vector(cur->token, state_vec, NFA);
            std::unordered_set<std::string> token_vec;
            DEBUG_MATCH std::cout << "Token List: " << std::endl;
            for(int j = 0; j < state_vec.size(); j++) {
                if(state_vec[j]->token != "") {
                    DEBUG_MATCH std::cout << "\tNFAState:" << state_vec[j]->id << " token:" << state_vec[j]->token << std::endl;
                    token_vec.insert(state_vec[j]->token);
                }
            }
            int j;
            for(j = 0; j < token_list.size(); j++) {
                if(token_vec.find(token_list[j]) != token_vec.end()) {
                    token = token_list[j];
                    if(i == cur_next_size){
                        DEBUG_MATCH std::cout << "Unget char:" << ch << std::endl; 
                        input.unget();
                    }
                    break;
                }
            }
            if(j == token_list.size()) {
                DEBUG_MATCH std::cout << "Match Single Char" << std::endl;
                match += ch;
                int cnt = match.size();
                while(cnt > 1) {
                    DEBUG_MATCH std::cout << "Unget char:" << ch << std::endl;
                    input.unget();
                    cnt--;
                }
                token = "'";
                token.push_back(match[0]);
                token += "'";
                match = match.substr(0, 1);
            }
            // output << Unprint2Trans(token) << " " << "\"" << Unprint2Trans(match) << "\"" << std::endl;
            output.push_back(std::make_pair(token, match));
            DEBUG_MATCH std::cout << "Matched!!!, token:" << Unprint2Trans(token) << " matched:" << Unprint2Trans(match) << std::endl;
            cur = DFA.head;
            match = "";
            token = "";
            // if(!input.eof()) {
            //     char testch = input.get();
            //     input.unget();
            //     std::cout << "#13# "<< testch << std::endl;
            // }
        }
    }
    return 0;
}