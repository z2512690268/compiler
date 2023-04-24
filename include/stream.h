#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "transfer.h"

template <typename T>
class TokenStream {
public:
    // Constructor
    TokenStream() : current_token_index(0) {}

    // Copy Constructor
    TokenStream(const TokenStream& other) {
        tokens = other.tokens;
        current_token_index = other.current_token_index;
    }

    // Assignment Operator
    TokenStream& operator=(const TokenStream& other) {
        tokens = other.tokens;
        current_token_index = other.current_token_index;
        return *this;
    }

    // Set the stream position to a specific token
    void SetPosition(int index) {
        if (index < 0 || index >= tokens.size()) {
            throw std::out_of_range("TokenStream::set_position: Invalid index");
        }
        current_token_index = index;
    }

    // Export the entire stream to a file
    void ExportFile(const std::string& file_path) {
        std::ofstream out_file(file_path);
        if (!out_file.is_open()) {
            throw std::runtime_error("TokenStream::export_to_file: Failed to open file");
        }
        for (auto token : tokens) {
            out_file << token << std::endl;
        }
        out_file.close();
    }

    // Load the entire stream from a file
    void LoadFile(const std::string& file_path) {
        std::ifstream in_file(file_path);
        if (!in_file.is_open()) {
            throw std::runtime_error("TokenStream::load_from_file: Failed to open file");
        }
        while(true) {
            T token;
            in_file >> token;
            if(in_file.fail()) {
                break;
            }
            tokens.push_back(token);
        }
        in_file.close();
        current_token_index = 0;
    }

    // Check if the tokens enter end
    bool Eof() {
        return current_token_index >= tokens.size();
    }

    // Get a token
    void GetToken(T &token) {
        if (current_token_index >= tokens.size()) {
            throw std::out_of_range("TokenStream::operator>>: Stream out of tokens");
        }
        token = tokens[current_token_index];
        current_token_index++;
    }

    // Find the index of the next occurrence of a token
    int FindToken(const T& token) {
        auto it = std::find(tokens.begin() + current_token_index + 1, tokens.end(), token);
        if (it == tokens.end()) {
            return -1;
        }
        return std::distance(tokens.begin(), it);
    }

    // Add a token to the end of the stream
    void AddToken(const T& token) {
        tokens.push_back(token);
    }

    // Get the number of tokens in the stream
    int Size() const {
        return tokens.size();
    }

private:
    std::vector<T> tokens;
    int current_token_index;
};

struct GrammerToken {
	std::string token;
    std::vector<std::string> rule;
    int terminal;

	GrammerToken() : token(""), terminal(0) {}
    GrammerToken(const std::string& token, const std::vector<std::string>& rule, int terminal) : token(token), rule(rule), terminal(terminal) {}
	
    friend std::ostream& operator<<(std::ostream& os, const GrammerToken& token) {
        os << VisableString(token.token);
        if(!token.terminal) {
            os << " : ";
        } else {
            os << " = ";
        }
        for(auto word : token.rule) {
            os << VisableString(word) << " ";
        }
		return os;
	}

	friend std::istream& operator>>(std::istream& is, GrammerToken& token) {
        std::string line;
        std::getline(is, line);
        std::stringstream ss(line);
        std::cout << line.c_str() << std::endl;
        ss >> token.token;
        token.token = AsciiString(token.token);
        std::string equal;
        ss >> equal;
        if(equal == ":") {
            token.terminal = 0;
        } else if(equal == "=") {
            token.terminal = 1;
        } else {
            if(is.eof()) {
                return is;
            }
            throw std::runtime_error("GrammerToken::operator>>: Invalid token");
        }
        std::string word;
        while(ss >> word) {
            token.rule.push_back(AsciiString(word));
        }

        return is;
	}
    
	bool operator==(const GrammerToken& other) const {
        return token == other.token && rule == other.rule && terminal == other.terminal;
    }
};