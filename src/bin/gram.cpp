#include <iostream>
#include <string>
#include <fstream>
#include "defs.h"

int main(int argc, char *argv[]) {
    if(argc < 4) {
        std::cout << "Usage: " << argv[0] << " <grammer-rule> <lex-output> <grammer-output>" << std::endl;
        return 1;
    }
    std::string tmp = PROJECT_ROOT_DIR;
    std::ifstream gin(tmp + "/test/grammer/" + argv[1]);
    std::ifstream tokenin(tmp + "/test/pipeline/" + argv[2]);
    std::ofstream gramout(tmp + "/test/pipeline/" + argv[3]);
}