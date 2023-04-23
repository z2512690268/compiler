#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

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
            std::string 
            out_file << encode_token_string(token) << std::endl;
        }
        out_file.close();
    }

    // Load the entire stream from a file
    void LoadFile(const std::string& file_path) {
        std::ifstream in_file(file_path);
        if (!in_file.is_open()) {
            throw std::runtime_error("TokenStream::load_from_file: Failed to open file");
        }
        std::string line;
        while (std::getline(in_file, line)) {
            tokens.push_back(decode_token(line));
        }
        in_file.close();
        current_token_index = -1;
    }

    // Check if the tokens enter end
    bool Eof() {
        return current_token_index + 1 >= tokens.size();
    }

    // Get a token
    void GetToken(T &token) {
        if (current_token_index + 1 >= tokens.size()) {
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
        tokens.push_back(token.to_string());
    }

    // Get the number of tokens in the stream
    int Size() const {
        return tokens.size();
    }

private:
    std::vector<T> tokens;
    int current_token_index;


};

// class TokenStream<std::string> {
