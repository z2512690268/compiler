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
#include "lex.h"
#include "transfer.h"
// #include <unistd.h> 
int main(int argc, char* argv[])
{
    if (argc < 4) {
        std::cout << "Usage: " << argv[0] << " <rule-file>" << " " << "<input-file>" << " " << "<output-file>" << std::endl;
        return 1;
    }
    std::string tmp = PROJECT_ROOT_DIR;
    // std::cout << tmp + argv[1] << std::endl;
    // return 0;
    std::ifstream fin(tmp + "/test/lex/" + argv[1] + ".l");
    std::ifstream input(tmp + "/test/pipeline/" + argv[2] + ".input");
    std::ofstream fout(tmp + "/test/pipeline/" + argv[3] + ".lex");
    if(!fin.is_open()) {
        std::cout << "rule file open failed" << std::endl;
        return 1;
    }
    if(!input.is_open()) {
        std::cout << "input file open failed" << std::endl;
        return 1;
    }

    std::vector<std::pair<std::string, std::string>> output;
    int lex_out = lexer(fin, input, output);

    if(!!lex_out){
        return 1;
    }
    for(auto& outline : output){
        fout << VisableString(outline.first) << " \"" << VisableString(outline.second) << "\"" << std::endl;
    }
    return 0;
}
