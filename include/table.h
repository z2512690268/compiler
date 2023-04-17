#pragma
#include <vector>
#include <string>
#include <iostream>

struct Table {
    std::vector<std::vector<std::string>> table;

    Table(int row, int col) {
        table.resize(row);
        for(int i = 0; i < row; i++) {
            table[i].resize(col);
        }
    }

    void Set(int row, int col, const std::string& str) {
        table[row][col] = str;
    }

    std::string Get(int row, int col) {
        return table[row][col];
    }

    int GetRowSize() {
        return table.size();
    }

    int GetColSize() {
        return table[0].size();
    }

    int AddRow() {
        table.resize(table.size() + 1);
        table[table.size() - 1].resize(table[0].size());
        return table.size();
    }

    int AddCol() {
        for(int i = 0; i < table.size(); i++) {
            table[i].resize(table[i].size() + 1);
        }
        return table[0].size();
    }

    void Print(){
        // 计算每列的最大宽
        for(int j = 0; j < table[0].size(); ++j){
            int max_width = 0;
            for(int i = 0; i < table.size(); ++i){
                if(table[i][j].size() > max_width){
                    max_width = table[i][j].size();
                }
            }
            for(int i = 0; i < table.size(); ++i){
                table[i][j] += std::string(max_width - table[i][j].size(), ' ');
            }    
        }
        for(int i = 0; i < table.size(); ++i){
            for(int j = 0; j < table[i].size(); ++j){
                std::cout << table[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
};