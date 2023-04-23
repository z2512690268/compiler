#include "gtest/gtest.h"
#include "glog/logging.h"
#include <iostream>
#include <fstream>
#include <string>
#include "defs.h"
#include "lex.h"
#include "gram.h"
#include "transfer.h"

TEST(TestGram, MazeTest) {
    std::string projdir = PROJECT_ROOT_DIR;
    std::ifstream gram_in(projdir + "test/gram/sysy.y");
    std::ifstream input_in(projdir + "test/pipeline/maze.lex");
    std::ifstream output_in(projdir + "test/pipeline/maze.gram");
    std::ifstream lex_gram_in(projdir + "test/lex/gram.l");

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

TEST(TestGram, SysyTest) {
    std::string projdir = PROJECT_ROOT_DIR;
    for(int i = 1; i <= 11; ++i) {
        SCOPED_TRACE(std::to_string(i));
        std::ifstream gram_in(projdir + "test/gram/sysy_" + std::to_string(i) + ".y");
        std::ifstream input_in(projdir + "test/pipeline/sysy_t" + std::to_string(i) + ".lex");
        std::ifstream output_in(projdir + "test/pipeline/sysy_t" + std::to_string(i) + ".gram");
        std::ifstream lex_gram_in(projdir + "test/lex/gram.l");

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
}