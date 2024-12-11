#ifndef SEMIC_SIM_HPP_INCLUDED
#define SEMIC_SIM_HPP_INCLUDED

#include <iostream>
#include <vector>
#include <map>
#include <type_system.hpp>
#include <variant>
#include <stack>

#include "ir.hpp"

typedef std::variant<void*, uint32_t, uint64_t, float, double, std::string> RegisterValue;

class Simulator; 

class Register {
public:
    std::shared_ptr<Type> type;
    RegisterValue value;

    std::shared_ptr<Register> run_binary_op(IROptype op, Register &rhs) {
        auto result = std::make_shared<Register>();
        switch (op) {
        case IROptype::add:
            result->type = type;
            if (type->type_id == Type::TypeID::IntegerTyID) {
                result->value = std::get<uint64_t>(value) + std::get<uint64_t>(rhs.value);
            } else if (type->type_id == Type::TypeID::FloatTyID) {
                result->value = std::get<float>(value) + std::get<float>(rhs.value);
            } else if (type->type_id == Type::TypeID::DoubleTyID) {
                result->value = std::get<double>(value) + std::get<double>(rhs.value);
            }
            break;
        case IROptype::minus:
            result->type = type;
            if (type->type_id == Type::TypeID::IntegerTyID) {
                result->value = std::get<uint64_t>(value) - std::get<uint64_t>(rhs.value);
            } else if (type->type_id == Type::TypeID::FloatTyID) {
                result->value = std::get<float>(value) - std::get<float>(rhs.value);
            } else if (type->type_id == Type::TypeID::DoubleTyID) {
                result->value = std::get<double>(value) - std::get<double>(rhs.value);
            }
            break;
        case IROptype::mul:
            result->type = type;
            if (type->type_id == Type::TypeID::IntegerTyID) {
                result->value = std::get<uint64_t>(value) * std::get<uint64_t>(rhs.value);
            } else if (type->type_id == Type::TypeID::FloatTyID) {
                result->value = std::get<float>(value) * std::get<float>(rhs.value);
            } else if (type->type_id == Type::TypeID::DoubleTyID) {
                result->value = std::get<double>(value) * std::get<double>(rhs.value);
            }
            break;
        case IROptype::divide:
            result->type = type;
            if (type->type_id == Type::TypeID::IntegerTyID) {
                result->value = std::get<uint64_t>(value) / std::get<uint64_t>(rhs.value);
            } else if (type->type_id == Type::TypeID::FloatTyID) {
                result->value = std::get<float>(value) / std::get<float>(rhs.value);
            } else if (type->type_id == Type::TypeID::DoubleTyID) {
                result->value = std::get<double>(value) / std::get<double>(rhs.value);
            }
            break;
        case IROptype::land:
            result->type = std::make_shared<IntegerType>(32);
            if (type->type_id == Type::TypeID::IntegerTyID) {
                result->value = (uint32_t)(std::get<uint64_t>(value) && std::get<uint64_t>(rhs.value));
            } else if (type->type_id == Type::TypeID::FloatTyID) {
                result->value = (uint32_t)(std::get<float>(value) && std::get<float>(rhs.value));
            } else if (type->type_id == Type::TypeID::DoubleTyID) {
                result->value = (uint32_t)(std::get<double>(value) && std::get<double>(rhs.value));
            }
            break;
        case IROptype::lor:
            result->type = std::make_shared<IntegerType>(32);
            if (type->type_id == Type::TypeID::IntegerTyID) {
                result->value = (uint32_t)(std::get<uint64_t>(value) || std::get<uint64_t>(rhs.value));
            } else if (type->type_id == Type::TypeID::FloatTyID) {
                result->value = (uint32_t)(std::get<float>(value) || std::get<float>(rhs.value));
            } else if (type->type_id == Type::TypeID::DoubleTyID) {
                result->value = (uint32_t)(std::get<double>(value) || std::get<double>(rhs.value));
            }
            break;
        case IROptype::less:
            result->type = std::make_shared<IntegerType>(32);
            if (type->type_id == Type::TypeID::IntegerTyID) {
                result->value = (uint32_t)(std::get<uint64_t>(value) < std::get<uint64_t>(rhs.value));
            } else if (type->type_id == Type::TypeID::FloatTyID) {
                result->value = (uint32_t)(std::get<float>(value) < std::get<float>(rhs.value));
            } else if (type->type_id == Type::TypeID::DoubleTyID) {
                result->value = (uint32_t)(std::get<double>(value) < std::get<double>(rhs.value));
            }
            break;
        case IROptype::greater:
            result->type = std::make_shared<IntegerType>(32);
            if (type->type_id == Type::TypeID::IntegerTyID) {
                result->value = (uint32_t)(std::get<uint64_t>(value) > std::get<uint64_t>(rhs.value));
            } else if (type->type_id == Type::TypeID::FloatTyID) {
                result->value = (uint32_t)(std::get<float>(value) > std::get<float>(rhs.value));
            } else if (type->type_id == Type::TypeID::DoubleTyID) {
                result->value = (uint32_t)(std::get<double>(value) > std::get<double>(rhs.value));
            }
            break;
        case IROptype::less_equal:
            result->type = std::make_shared<IntegerType>(32);
            if (type->type_id == Type::TypeID::IntegerTyID) {
                result->value = (uint32_t)(std::get<uint64_t>(value) <= std::get<uint64_t>(rhs.value));
            } else if (type->type_id == Type::TypeID::FloatTyID) {
                result->value = (uint32_t)(std::get<float>(value) <= std::get<float>(rhs.value));
            } else if (type->type_id == Type::TypeID::DoubleTyID) {
                result->value = (uint32_t)(std::get<double>(value) <= std::get<double>(rhs.value));
            }
            break;
        case IROptype::greater_equal:
            result->type = std::make_shared<IntegerType>(32);
            if (type->type_id == Type::TypeID::IntegerTyID) {
                result->value = (uint32_t)(std::get<uint64_t>(value) >= std::get<uint64_t>(rhs.value));
            } else if (type->type_id == Type::TypeID::FloatTyID) {
                result->value = (uint32_t)(std::get<float>(value) >= std::get<float>(rhs.value));
            } else if (type->type_id == Type::TypeID::DoubleTyID) {
                result->value = (uint32_t)(std::get<double>(value) >= std::get<double>(rhs.value));
            }
            break;
        default:
            // Handle other operations if needed
            break;
        }
        return result;
    }

    std::shared_ptr<Register> run_unary_op(IROptype op) {
        auto result = std::make_shared<Register>();
        switch (op) {
        case IROptype::inc:
            result->type = type;
            if (type->type_id == Type::TypeID::IntegerTyID) {
                result->value = std::get<uint64_t>(value) + 1;
            } else if (type->type_id == Type::TypeID::FloatTyID) {
                result->value = std::get<float>(value) + 1.0f;
            } else if (type->type_id == Type::TypeID::DoubleTyID) {
                result->value = std::get<double>(value) + 1.0;
            }
            break;
        case IROptype::dec:
            result->type = type;
            if (type->type_id == Type::TypeID::IntegerTyID) {
                result->value = std::get<uint64_t>(value) - 1;
            } else if (type->type_id == Type::TypeID::FloatTyID) {
                result->value = std::get<float>(value) - 1.0f;
            } else if (type->type_id == Type::TypeID::DoubleTyID) {
                result->value = std::get<double>(value) - 1.0;
            }
            break;
        default:
            // Handle other operations if needed
            break;
        }
        return result;
    }

    std::shared_ptr<Register> run_typecast_op(std::shared_ptr<Type> target_type) {
        auto result = std::make_shared<Register>();
        result->type = target_type;
        // Implement typecast logic if needed
        return result;
    }

    std::string to_string() const {
        return std::visit([](auto&& val) -> std::string {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, void*>) {
                return "void*";
            } else if constexpr (std::is_same_v<T, uint32_t>) {
                return std::to_string(val);
            } else if constexpr (std::is_same_v<T, uint64_t>) {
                return std::to_string(val);
            } else if constexpr (std::is_same_v<T, float>) {
                return std::to_string(val);
            } else if constexpr (std::is_same_v<T, double>) {
                return std::to_string(val);
            } else if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else {
                return "unknown";
            }
        }, value);
    }

    void print() const {
        std::cout << to_string();
    }
};

class FunctionContext {
public:
    int bid, index;
    std::shared_ptr<IRFunction> cur_function;
    std::map<std::string, std::shared_ptr<Register>> symbol_table;
    Simulator* sim;

    FunctionContext(std::shared_ptr<IRFunction> n_func, Simulator* n_sim) :
        cur_function(n_func), bid(0), index(0), sim(n_sim) {
        initialize();
    }

    void initialize() {
        for (const auto& decl : cur_function->decl_vec) {
            auto reg = std::make_shared<Register>();
            reg->type = decl.value_type;
            // 根据类型初始化变量
            switch (decl.value_type->type_id) {
            case Type::TypeID::IntegerTyID:
                reg->value = uint64_t(0);
                break;
            case Type::TypeID::FloatTyID:
                reg->value = float(0.0);
                break;
            case Type::TypeID::DoubleTyID:
                reg->value = double(0.0);
                break;
            case Type::TypeID::StringTyID:
                reg->value = std::string("");
                break;
            default:
                reg->value = nullptr;
                break;
            }
            symbol_table[decl.get_name()] = reg;
        }
    }

    void step() {
        while (true) {
            auto cur_block = cur_function->body[bid];
            if (index >= cur_block->statements.size()) {
                cur_block->jump.run(this);
                continue;
            }
            auto cur_ir = cur_block->statements[index];
            if (cur_ir->type == IROptype::call) {
                cur_ir->run(this);
                return; // 切换到新函数上下文
            }
            cur_ir->run(this);
            index++;
        }
    }

    std::shared_ptr<Register> get_value(IRValue value) {
        switch (value.type)
        {
        case IROpearndType::id : {
            Operand op_value = std::get<Operand>(value.operand);
            return symbol_table[op_value.name];
            break;
        }
        case IROpearndType::literal_double : {
            double double_value = std::get<double>(value.operand);
            auto reg = std::make_shared<Register>();
            reg->value = double_value;
            return reg;
            break;
        }
        case IROpearndType::literal_int : {
            uint64_t int_value = std::get<uint64_t>(value.operand);
            auto reg = std::make_shared<Register>();
            reg->value = int_value;
            return reg;
            break;
        }
        case IROpearndType::literal_string : {
            std::string str_value = std::get<std::string>(value.operand);
            auto reg = std::make_shared<Register>();
            reg->value = str_value;
            return reg;
            break;
        }
        case IROpearndType::temp : {
            TempOperand temp_value = std::get<TempOperand>(value.operand);
            return symbol_table[std::to_string(temp_value.bid) + ':' + std::to_string(temp_value.index)];
            break;
        }
        case IROpearndType::unknown_operand : {
            return nullptr;
            break;
        }
        default:
            return nullptr;
        }
    }

    void set_temp_value(TempOperand temp, std::shared_ptr<Register> value) {
        symbol_table[std::to_string(temp.bid) + ':' + std::to_string(temp.index)] = value;
    }
};

class Simulator {
public:
    std::stack<std::shared_ptr<FunctionContext>> st;
    CodeGenContext* context;

    Simulator(CodeGenContext* ctx) : context(ctx) {}

    void init_run() {
        auto main_func = context->func_defs["main"];
        auto start_context = std::make_shared<FunctionContext>(main_func, this);
        st.push(start_context);
    }

    void run() {
        init_run();
        while(!st.empty()) {
            auto cur_context = st.top();
            cur_context->step();
            if (cur_context->cur_function->return_type->type_id != Type::TypeID::VoidTyID) {
                auto return_value = cur_context->symbol_table["return"];
                st.pop();
                if (!st.empty()) {
                    auto caller_context = st.top();
                    TempOperand temp{caller_context->bid, caller_context->index};
                    caller_context->set_temp_value(temp, return_value);
                }
            } else {
                st.pop();
            }
        }
    }
};

#endif