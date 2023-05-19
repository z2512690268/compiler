#include "gtest/gtest.h"
#include "glog/logging.h"
#include <iostream>
#include <fstream>
#include <string>
#include "defs.h"
#include "lex.h"
#include "transfer.h"
#include "path.h"

TEST(TestLex, GrammerLexTest) {
    Path projdir = PROJECT_ROOT_DIR;
    std::ifstream rule_in(projdir.append("test").append("lex").append("gram.l"));
    std::ifstream input_in(projdir.append("test").append("pipeline").append("gram.input"));
    std::ifstream output_in(projdir.append("test").append("pipeline").append("gram.lex"));

    ASSERT_TRUE(rule_in.is_open());
    ASSERT_TRUE(input_in.is_open());
    ASSERT_TRUE(output_in.is_open());

    std::vector<std::pair<std::string, std::string>> lex_output;
    int lex_ret = lexer(rule_in, input_in, lex_output);
    
    ASSERT_FALSE(lex_ret);

    for(int i = 0; i < lex_output.size(); ++i) {
        std::string line;
        std::getline(output_in, line);
        std::string token_in, match_in;
        // split output_in to token_in and match_in, by space splitter
        size_t space_pos = line.find(' ');
        token_in = line.substr(0, space_pos);
        match_in = line.substr(space_pos + 1); 
        SCOPED_TRACE(std::to_string(i));
        ASSERT_EQ(VisableString(lex_output[i].first), token_in);
        ASSERT_EQ(VisableString("\"" + lex_output[i].second + "\""), match_in);
    }
}

TEST(TestLex, Rule1Test) {
    Path projdir = PROJECT_ROOT_DIR;
    std::ifstream rule_in(projdir.append("test").append("lex").append("rule1.l"));
    std::ifstream input_in(projdir.append("test").append("pipeline").append("rule1.input"));
    std::ifstream output_in(projdir.append("test").append("pipeline").append("rule1.lex"));

    ASSERT_TRUE(rule_in.is_open());
    ASSERT_TRUE(input_in.is_open());
    ASSERT_TRUE(output_in.is_open());

    std::vector<std::pair<std::string, std::string>> lex_output;
    int lex_ret = lexer(rule_in, input_in, lex_output);
    
    ASSERT_FALSE(lex_ret);

    for(int i = 0; i < lex_output.size(); ++i) {
        std::string line;
        std::getline(output_in, line);
        std::string token_in, match_in;
        // split output_in to token_in and match_in, by space splitter
        size_t space_pos = line.find(' ');
        token_in = line.substr(0, space_pos);
        match_in = line.substr(space_pos + 1); 
        SCOPED_TRACE(std::to_string(i));
        ASSERT_EQ(VisableString(lex_output[i].first), token_in);
        ASSERT_EQ(VisableString("\"" + lex_output[i].second + "\""), match_in);
    }
}

TEST(TestLex, SysyTest) {
    Path projdir = PROJECT_ROOT_DIR;
    std::ifstream rule_in(projdir.append("test").append("lex").append("sysy.l"));
    std::ifstream input_in(projdir.append("test").append("pipeline").append("sysy_t1.input"));
    std::ifstream output_in(projdir.append("test").append("pipeline").append("sysy_t1.lex"));


    ASSERT_TRUE(rule_in.is_open());
    ASSERT_TRUE(input_in.is_open());
    ASSERT_TRUE(output_in.is_open());

    std::vector<std::pair<std::string, std::string>> lex_output;
    int lex_ret = lexer(rule_in, input_in, lex_output);
    
    ASSERT_FALSE(lex_ret);

    for(int i = 0; i < lex_output.size(); ++i) {
        std::string line;
        std::getline(output_in, line);
        std::string token_in, match_in;
        // split output_in to token_in and match_in, by space splitter
        size_t space_pos = line.find(' ');
        token_in = line.substr(0, space_pos);
        match_in = line.substr(space_pos + 1); 
        SCOPED_TRACE(std::to_string(i));
        ASSERT_EQ(VisableString(lex_output[i].first), token_in);
        ASSERT_EQ(VisableString("\"" + lex_output[i].second + "\""), match_in);
    }
}

TEST(TestLex, MazeTest) {
    Path projdir = PROJECT_ROOT_DIR;
    std::ifstream rule_in(projdir.append("test").append("lex").append("sysy.l"));
    std::ifstream input_in(projdir.append("test").append("pipeline").append("maze.input"));
    std::ifstream output_in(projdir.append("test").append("pipeline").append("maze.lex"));


    ASSERT_TRUE(rule_in.is_open());
    ASSERT_TRUE(input_in.is_open());
    ASSERT_TRUE(output_in.is_open());

    std::vector<std::pair<std::string, std::string>> lex_output;
    int lex_ret = lexer(rule_in, input_in, lex_output);
    
    ASSERT_FALSE(lex_ret);

    for(int i = 0; i < lex_output.size(); ++i) {
        std::string line;
        std::getline(output_in, line);
        std::string token_in, match_in;
        // split output_in to token_in and match_in, by space splitter
        size_t space_pos = line.find(' ');
        token_in = line.substr(0, space_pos);
        match_in = line.substr(space_pos + 1); 
        SCOPED_TRACE(std::to_string(i));
        ASSERT_EQ(VisableString(lex_output[i].first), token_in);
        ASSERT_EQ(VisableString("\"" + lex_output[i].second + "\""), match_in);
    }
}