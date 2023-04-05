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

#define MAX_NODE 10000
int flag[MAX_NODE] = {0};

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
    void Print(int flag[]) {
        flag[id] = 1;
        std::cout << "id: " << id << " ch: " << ch << " token: " << token << std::endl;
        for(int i = 0; i < next.size(); i++) {
            std::cout << "\t" << "next: " << next[i]->id << std::endl;
        }
        for(int i = 0; i < next.size(); i++) {
            if(flag[next[i]->id] == 0)
                next[i]->Print(flag);
        }
    }
    void GenerateMap(int flag[], std::unordered_map<int, Node*>& id2node) {
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
        memset(flag, 0, sizeof(flag));
        id2node.clear();
        head->GenerateMap(flag, id2node);
    }

};

std::string Char2Str(char ch) {
    std::string str;
    str += ch;
    str = "'" + str + "'";
    return str;
}

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
#include <unistd.h> 
int main(int argc, char* argv[])
{
    if (argc < 4) {
        std::cout << "Usage: " << argv[0] << " <rule-file>" << " " << "<input-file>" << " " << "<output-file>" << std::endl;
        return 1;
    }
    std::string tmp = PROJECT_ROOT_DIR;
    std::cout << tmp + argv[1] << std::endl;
    // return 0;
    std::ifstream fin(tmp + argv[1]);
    std::ifstream input(tmp + argv[2]);
    std::ofstream output(tmp + argv[3]);
    // std::ifstream fin("rule.txt");
    // std::ifstream input(argv[2]);
    // std::ofstream output(argv[3]);

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
        regex = "(" + regex + ")";
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
            } else if(regex[i] == '\\') {
                // TODO: 转义字符处理
                // switch (regex[i + 1])
                // {
                // case 'n':
                //     regex[i + 1] = '\n';
                //     break;
                // case 't':
                //     regex[i + 1] = '\t';
                //     break;
                // case '\\':
                //     regex[i + 1] = '\\';
                //     break;
                // default:
                //     break;
                // }
                // continue;
            } else {
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

    std::cout << "step final:" << std::endl;
    memset(flag, 0, sizeof(flag));
    NFA.head->Print(flag);
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

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
            std::cout << "hsjkdahkdjhaj " << qtop.first << " " << qtop.second << std::endl;
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
    std::cout << "step "<< "final" << std::endl;
    memset(flag, 0, sizeof(flag));
    DFA.head->Print(flag);
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    // parse by DFA
    char ch;
    std::string match;
    std::string token;
    int line = 1;
    Node* cur = DFA.head;
    while(!input.eof()) {
        int i;
        input.get(ch);
        int cur_next_size = cur->next.size();
        // std::cout << "#5# " << ch << std::endl;
        // 还能匹配就继续匹配
        for(i = 0; i < cur->next.size(); i++) {
            if(cur->next[i]->ch == ch) {
                cur = cur->next[i];
                match += ch;
                break;
            }
        }
        // std::cout << "#7# " << cur->id << " " <<  i << std::endl;
        if(i == cur_next_size || input.eof()) {
            // 无法匹配
            // std::cout << "#6 " << ch << " " << match << " " << cur->token << std::endl;
            std::vector<Node*> state_vec;
            State2Vector(cur->token, state_vec, NFA);
            std::unordered_set<std::string> token_vec;
            for(int j = 0; j < state_vec.size(); j++) {
                // std::cout << "#8# " << state_vec[j]->id << " " << state_vec[j]->token << std::endl;
                if(state_vec[j]->token != "") {
                    token_vec.insert(state_vec[j]->token);
                }
            }
            int j;
            for(j = 0; j < token_list.size(); j++) {
                if(token_vec.find(token_list[j]) != token_vec.end()) {
                    token = token_list[j];
                    if(i == cur_next_size)
                        input.unget();
                    // std::cout << "#11# " << token << " " << match << std::endl;
                    break;
                }
            }
            if(j == token_list.size()) {
                // std::cout << "#12# " << match << std::endl;
                match += ch;
                int cnt = match.size();
                while(cnt > 1) {
                    input.unget();
                    cnt--;
                }
                token = "'";
                token.push_back(match[0]);
                token += "'";
                match = match.substr(0, 1);
            }
            output << token << " " << match << std::endl;
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
}