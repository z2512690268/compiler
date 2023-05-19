#include "gtest/gtest.h"
#include "glog/logging.h"
#include <iostream>
#include <fstream>
#include <string>
#include "defs.h"
#include "lex.h"
#include "gram.h"
#include "transfer.h"
#include "path.h"

TEST(TestGram, MazeTest) {
    Path projdir = PROJECT_ROOT_DIR;
    std::ifstream gram_in(projdir.append("test").append("gram").append("sysy").add(".y"));
    std::ifstream input_in(projdir.append("test").append("pipeline").append("maze").add(".lex"));
    std::ifstream output_in(projdir.append("test").append("pipeline").append("maze").add(".gram"));
    std::ifstream lex_gram_in(projdir.append("src").append("res").append("gram.l"));

    ASSERT_TRUE(gram_in.is_open());
    ASSERT_TRUE(input_in.is_open());
    ASSERT_TRUE(output_in.is_open());
    ASSERT_TRUE(lex_gram_in.is_open());

    std::stringstream gram_out;
    int ret = grammer(gram_in, lex_gram_in, input_in, gram_out);
    ASSERT_FALSE(ret);

    std::string line;
    while(std::getline(gram_out, line)) {
        std::string line_in;
        std::getline(output_in, line_in);
        ASSERT_EQ(line, line_in);
    }
}