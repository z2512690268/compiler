#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include "defs.h"
#include "koopa.h"
#include "frontend.h"
#include "backend.h"

// 主函数
int main(int argc, char* argv[]) {
    std::string project_dir = PROJECT_ROOT_DIR;
    if(argc < 4) {
        std::cout << "Usage: " << argv[0] << " " << "<frontend-lang> <backend-lang> <filename-base>" << std::endl;
        exit(1);
    }

    KoopaGenerator* generator;
    FrontendBase*   frontend;
    std::string file_name = (project_dir + "test/pipeline/" + argv[3] + ".gram");

    if(std::string(argv[2]) == "koopa"){
        generator = new KoopaGenerator();
    } else if(std::string(argv[2]) == "riscv") {
        generator = new RiscvGenerator();
    } else {
        std::cerr << "Unknown target: " << argv[2] << std::endl;
        exit(1);
    }

    if(std::string(argv[1]) == "sysy") {
        frontend = new SysyFrontend(file_name, generator);
    } else if(std::string(argv[1]) == "koopa") {
        frontend = new KoopaFrontend(file_name, generator);
    } else {
        std::cerr << "Unknown source: " << argv[2] << std::endl;
        exit(1);
    }

    frontend->Process();

    std::ofstream fout(project_dir + "test/pipeline/" + argv[3] + "." + std::string(argv[2]));

    std::cout << std::endl << std::endl;

    std::string output = generator->GenerateCode();
    std::cout << output << std::endl;
    fout << output << std::endl;

    return 0;
}