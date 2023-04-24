#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "defs.h"
#include "stream.h"

void ConstructSyntaxTree(TokenStream<GrammerToken>& stream) {

}

int main() {
    std::string project_dir = PROJECT_ROOT_DIR;
    std::string file_name = (project_dir + "test/pipeline/maze.gram");
    
    TokenStream<GrammerToken> stream;
    stream.LoadFile(file_name);

    // while(!stream.Eof()) {
    //     GrammerToken temp;
    //     stream.GetToken(temp);
    //     std::cout << temp << std::endl;
    // }

    ConstructSyntaxTree(stream);

    return 0;
}
