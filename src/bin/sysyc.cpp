#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <fstream>
#include <algorithm>
#include <string>
#include "defs.h"
#include "lex.h"
#include "transfer.h"
#include "path.h"
#include <iostream>
#include "gram.h"
#include "frontend.h"
#include "backend.h"

// #include <unistd.h> 
int main(int argc, char* argv[])
{
    if (argc < 4) {
        std::cout << "Usage: " << argv[0] << " "  << "<koopa/riscv>" << " " << "<input-file>" << " " << "<output-file>" << std::endl;
        return 1;
    }
    // lexer
    Path tmp = PROJECT_ROOT_DIR;
    std::ifstream lexer_in(tmp.append("src").append("res").append("sysy.l"));
    std::ifstream fin(argv[2]);
    // std::ofstream fout(tmp.append("test").append("pipeline").append("tmp.lex"));
    if(!lexer_in.is_open()) {
        std::cout << "lex rule file open failed" << std::endl;
        return 1;
    }
    if(!fin.is_open()) {
        std::cout << "input file open failed" << std::endl;
        return 1;
    }

    std::vector<std::pair<std::string, std::string>> output;
    int lex_out = lexer(lexer_in, fin, output);

    if(!!lex_out){
        std::cerr << "lexer failed!" << std::endl;
        return 1;
    }

    std::ofstream lex_ss("tmp.lex");

    for(auto& outline : output){
        lex_ss << VisableString(outline.first) << " \"" << VisableString(outline.second) << "\"" << std::endl;
    }

    lex_ss.close();
    //----------------------------------------------------------------------------------------------------------------

    // gram
    std::ifstream gram_in(tmp.append("test").append("gram").append("sysy_9.y"));
    if(!gram_in.is_open())  { std::cout << "gram_in open failed!" << std::endl; return 1; }
    std::ifstream lex_in("tmp.lex");
    std::ofstream gram_out("tmp.gram");

    // 解析语法文件
    std::ifstream gram_rule(tmp.append("src").append("res").append("gram.l"));
    if(!gram_rule.is_open())  { std::cout << "gram_rule open failed!" << std::endl; return 1; }

    int ret = grammer(gram_in, gram_rule, lex_in, gram_out);
    if(ret != 0) {
        std::cerr << "grammer failed!" << std::endl;
        return 1;
    }
    gram_out.close();

    // symtax
    KoopaGenerator* generator;
    KoopaIR* ir;
    FrontendBase*   frontend;

    std::string file_name = ("tmp.gram");
    
    frontend = new SysyFrontend(file_name);

    std::ofstream fout(argv[3]);
    if(std::string(argv[1]) == "koopa"){
        generator = new KoopaGenerator();
    } else if(std::string(argv[1]) == "riscv") {
        generator = new RiscvGenerator();
    } else {
        std::cerr << "Unknown target: " << argv[1] << std::endl;
        exit(1);
    }

    ir = frontend->Process();
    std::cout << std::endl << std::endl;

    std::string outstr = generator->GenerateCode(ir);
    std::cout << outstr << std::endl;
    fout << outstr << std::endl;

    return 0;    
}
