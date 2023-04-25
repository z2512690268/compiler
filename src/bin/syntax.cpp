#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include "defs.h"
#include "stream.h"

void test_func() {

}

std::unordered_map<std::string, std::function<void()>> func_map = {
    {"test", test_func}
};

void ConstructSyntaxTree(TokenStream<GrammerToken>& stream) {
    while(!stream.Eof()) {
        GrammerToken temp;
        std::stringstream ss;
        stream.GetToken(temp);
        ss << temp;
        if(func_map.find(ss.str()) != func_map.end()) {
            func_map[ss.str()]();
        }
        std::cout << temp << std::endl;
    }
}

int main() {
    std::string project_dir = PROJECT_ROOT_DIR;
    std::string file_name = (project_dir + "test/pipeline/maze.gram");
    
    TokenStream<GrammerToken> stream;
    stream.LoadFile(file_name);

    ConstructSyntaxTree(stream);

    return 0;
}
