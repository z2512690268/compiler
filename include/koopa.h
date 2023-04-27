
#include <unordered_map>
#include <vector>
#include <string>

struct SymbolItem  {
    std::string origin_name;
    std::string name;
    std::string type;
};

struct SymbolTable {
    std::unordered_map<std::string, SymbolItem> map;
    SymbolTable* parent;
    SymbolTable(SymbolTable* parent = nullptr) : parent(parent) {}
};

struct Statement {
    std::string var_ret;
    std::string op_type;
    std::string var_input1;
    std::string var_input2;
};

struct Scope {
    SymbolTable* table;
    int TempVarNum = 0;
    std::vector<Statement> statements;

    std::string func_name;
    std::vector<std::string> func_params;
    std::string func_ret_type;
    
    Scope* parent;
    Scope(Scope* parent = nullptr) : parent(parent) {
        table = new SymbolTable(parent ? parent->table : nullptr);
    }
    int GetNewTempVarNum() {
        return TempVarNum++;
    }
    bool FindSymbol(std::string name, SymbolItem& item) {
        SymbolTable* table = this->table;
        while(table) {
            auto it = table->map.find(name);
            if(it != table->map.end()) {
                item = it->second;
                return true;
            }
            table = table->parent;
        }
        return false;
    }
};

struct KoopaGenerator {
    Scope global_scope;
};