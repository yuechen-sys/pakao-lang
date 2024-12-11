#ifndef SEMIC_SIM_HPP_INCLUDED
#define SEMIC_SIM_HPP_INCLUDED

#include <vector>
#include <map>
#include <type_system.hpp>
#include <variant>
#include <stack>

#include "ir.hpp"

typedef std::variant<void*, uint32_t, uint64_t, float, double, std::string> RegisterValue;

struct Register {
    std::shared_ptr<Type> type;
    RegisterValue value;
};

struct FunctionContext {
    int bid, index;
    std::map<std::string, Register> symbol_table;
};

class Simulator {
public:
    std::stack<FunctionContext> st;

    void run(CodeGenContext &context) {

    }
};

#endif