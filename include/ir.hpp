#ifndef SEMIC_IR_HPP_INCLUDED
#define SEMIC_IR_HPP_INCLUDED

#include <vector>
#include <stack>
#include <map>
#include <variant>
#include <string>

#include "type_system.hpp"

enum IROpearndType {
    unknown_operand,
    id,
    temp,
    literal_double,
    literal_int,
    literal_string,
};

struct TempOperand {
    int bid, index;
};

struct Operand {
    std::string name;
};

using IROperand = std::variant<Operand, TempOperand, double, uint64_t, std::string>;

class IRValue {
public:
    IROpearndType type;
    std::shared_ptr<Type> value_type;
    IROperand operand;

    IRValue(IROpearndType n_type, std::shared_ptr<Type> n_value_type, IROperand n_operand) :
        type(n_type), value_type(n_value_type), operand(n_operand) {}

    static IRValue from_double(double n_literal_double) {
        return IRValue(
            IROpearndType::literal_double,
            std::make_shared<FloatType>(64),
            n_literal_double
        );
    }

    static IRValue from_int(uint64_t n_literal_int) {
        return IRValue(
            IROpearndType::literal_int,
            std::make_shared<IntegerType>(64),
            n_literal_int
        );
    }

    static IRValue from_literal_string(std::string &n_literal_string) {
        return IRValue(
            IROpearndType::literal_string,
            std::make_shared<StringType>(),
            n_literal_string
        );
    }

    static IRValue from_operand(
        std::map<std::string, std::shared_ptr<Type>> &variable_types,
        std::string &n_name
    ) {
        return IRValue(
            IROpearndType::id,
            variable_types[n_name],
            Operand {
                n_name,
            }
        );
    }

    static IRValue from_bid(
        std::map<std::string, std::shared_ptr<Type>> &variable_types,
        int bid,
        int index
    ) {
        return IRValue(
            IROpearndType::temp,
            variable_types[std::to_string(bid) + ':' + std::to_string(index)],
            TempOperand {
                bid,
                index
            }
        );
    }

    std::string to_string() {
        std::string result = value_type->get_type_name() + ' ';
        switch (type)
        {
        case IROpearndType::id : {
            Operand op_value = std::get<Operand>(operand);
            return result + op_value.name;
            break;
        }
        case IROpearndType::literal_double : {
            double double_value = std::get<double>(operand);
            return result + std::to_string(double_value);
            break;
        }
        case IROpearndType::literal_int : {
            int int_value = std::get<uint64_t>(operand);
            return result + std::to_string(int_value);
            break;
        }
        case IROpearndType::literal_string : {
            std::string str_value = std::get<std::string>(operand);
            return result + str_value;
            break;
        }
        case IROpearndType::temp : {
            TempOperand temp_value = std::get<TempOperand>(operand);
            return result + std::to_string(temp_value.bid) + ':'
                + std::to_string(temp_value.index);
            break;
        }
        case IROpearndType::unknown_operand : {
            return result + std::string("unkown");
            break;
        }
        default:
            break;
        }
    }

    void print() {
        std::cout << to_string();
    }
};

class IRJump {
public:
    enum IRJumpType {
        unknown,
        direct,
        cond,
        ret,
    };

    IRJumpType jump_type;
    struct DirectJump {
        int jump_bid;
    };

    struct CondJump {
        IRValue cond_value;
        int true_bid, false_bid;
    };

    struct RetJump{
        IRValue return_value;
    };

    std::variant<DirectJump, CondJump, RetJump> jump;

    IRJump() : jump_type(IRJumpType::unknown) { }

    static IRJump from_direct_jump(int n_jump_bid) {
        IRJump new_jump;
        new_jump.jump_type = IRJumpType::direct;
        new_jump.jump = DirectJump {
            n_jump_bid
        };
        return new_jump;
    }

    static IRJump from_cond_jump(IRValue &n_cond_value,
        int n_true_bid, int n_false_bid) {
        IRJump new_jump;
        new_jump.jump_type = IRJumpType::cond;
        new_jump.jump = CondJump {
            n_cond_value,
            n_true_bid,
            n_false_bid
        };
        return new_jump;
    }

    static IRJump from_ret_jump(IRValue &n_ret_value) {
        IRJump new_jump;
        new_jump.jump_type = IRJumpType::ret;
        new_jump.jump = RetJump {
            n_ret_value
        };
        return new_jump;
    }

    void print(std::string prefix) {
        std::cout << prefix;
        switch (jump_type)
        {
        case direct: {
            DirectJump direct_jump = std::get<DirectJump>(jump);
            std::cout << "jump b" << direct_jump.jump_bid;
            break;
        }
        
        case cond: {
            CondJump cond_jump = std::get<CondJump>(jump);
            std::cout << "cond " << '<' << cond_jump.cond_value.to_string() << '>' << ' ';
            std::cout << "b" << cond_jump.true_bid << " b" << cond_jump.false_bid;
            break;
        }

        case ret: {
            RetJump ret_jump = std::get<RetJump>(jump);
            std::cout << "ret " << '<' << ret_jump.return_value.to_string() << '>';
            break;
        }

        case unknown: {
            std::cout << "no jump";
            break;
        }
        }
    }
};

enum IROptype {
    add,
    minus,
    divide,
    mul,

    land,
    lor,

    less,
    greater,
    less_equal,
    greater_equal,

    typecast,
    assign,
    inc,
    dec,
    call,
    array_index,
    printf_func,
    unknown_op,
};

static std::string ir_optype_to_str(IROptype type) {
    switch (type)
    {
    case add:
        return std::string("ADD");
        break;
    case minus:
        return std::string("MINUS");
        break;
    case divide:
        return std::string("DIV");
        break;
    case mul:
        return std::string("MUL");
        break;
    case land:
        return std::string("AND");
        break;
    case lor:
        return std::string("OR");
        break;
    case assign:
        return std::string("ASSIGN");
        break;
    case less:
        return std::string("LT");
        break;
    case greater:
        return std::string("GT");
        break;
    case array_index:
        return std::string("ARRAY_INDEX");
        break;
    case call:
    case printf_func:
        return std::string("CALL");
        break;
    case inc:
        return std::string("INC");
        break;
    case dec:
        return std::string("DEC");
        break;
    default:
        return std::string("UNKNOWN");
        break;
    }
}

class IR {
public:
    IROptype type;
    IR(IROptype n_type) : type(n_type) {}

    virtual ~IR() = default;

    virtual void print(std::string prefix) = 0;

    virtual std::shared_ptr<Type> result_type() = 0;
};

class IRBinary : public IR {
public:
    IRValue lhs, rhs;

    IRBinary(IROptype n_type, IRValue n_lhs, IRValue n_rhs) :
        IR(n_type), lhs(n_lhs), rhs(n_rhs) {}

    virtual void print(std::string prefix) override {
        std::cout << prefix;
        std::cout << ir_optype_to_str(type) << ' ';
        std::cout << '<';
        lhs.print();
        std::cout << '>' << ' ';
        std::cout << '<';
        rhs.print();
        std::cout << '>';
    }

    virtual std::shared_ptr<Type> result_type() override {
        return lhs.value_type;
    }
};

class IRUnary : public IR {
public:
    IRValue lhs;

    IRUnary(IROptype n_type, IRValue n_lhs) :
        IR(n_type), lhs(n_lhs) {}

    virtual void print(std::string prefix) {
        std::cout << prefix;
        std::cout << ir_optype_to_str(type) << ' ';
        std::cout << '<';
        lhs.print();
        std::cout << '>';
    }

    virtual std::shared_ptr<Type> result_type() {
        return lhs.value_type;
    }
};

class IRPrintf : public IR {
public:
    std::vector<IRValue> arguments;

    IRPrintf(
        std::vector<IRValue> n_arguments
    ) : IR(IROptype::printf_func),
        arguments(n_arguments) {}
    
    virtual void print(std::string prefix) {
        std::cout << prefix;
        std::cout << ir_optype_to_str(type) << " " << "printf" << '(';
        for(auto arg : arguments) {
            std::cout << '<';
            arg.print();
            std::cout << '>' << ',';
        }
        std::cout << ')';
    }

    virtual std::shared_ptr<Type> result_type() {
        return std::make_shared<VoidType>();
    }
};

class IRMethodCall : public IR {
public:
    std::shared_ptr<Type> return_type;
    std::string name;
    std::vector<IRValue> arguments;

    IRMethodCall(
        std::shared_ptr<Type> n_return_type,
        std::string n_name,
        std::vector<IRValue> n_arguments
    ) : IR(IROptype::call),
        return_type(n_return_type),
        name(n_name),
        arguments(n_arguments) {}

    virtual void print(std::string prefix) {
        std::cout << prefix;
        std::cout << ir_optype_to_str(type) << " " << name << '(';
        for(auto arg : arguments) {
            std::cout << '<';
            arg.print();
            std::cout << '>' << ',';
        }
        std::cout << ')';
    }

    virtual std::shared_ptr<Type> result_type() {
        return return_type;
    }
};

class IRArrayIndex : public IR {
public:
    IRValue array, index;

    IRArrayIndex(IRValue n_array, IRValue n_index) :
        IR(IROptype::array_index), array(n_array), index(n_index) {}

    virtual void print(std::string prefix) {
        std::cout << prefix;
        std::cout << ir_optype_to_str(type) << ' ';
        std::cout << '<';
        array.print();
        std::cout << '>' << ' ';
        std::cout << '<';
        index.print();
        std::cout << '>';
    }

    virtual std::shared_ptr<Type> result_type() {
        return array.value_type->get_element_type();
    }
};

class IRBlock {
public:
    int bid;
    std::vector<std::shared_ptr<IR>> statements;
    IRJump jump;

    IRBlock(int n_bid) : bid(n_bid) { }
    virtual ~IRBlock() {}

    virtual void print(std::string prefix) { } ;
};


class IRFunction {
public:
    std::shared_ptr<Type> return_type;
    std::string func_name;
    std::map<int, std::shared_ptr<IRBlock>> body;
    std::map<std::string, std::shared_ptr<Type>> type_tables;
    std::vector<IRValue> decl_vec;
    std::vector<IRValue> arg_vec;

    IRFunction(
        std::string n_func_name,
        std::shared_ptr<Type> n_return_type
    ) : func_name(n_func_name), return_type(n_return_type) {
        body[0] = std::make_shared<IRBlock>(0);
    }

    void add_arg(IRValue n_value) {
        arg_vec.push_back(n_value);
    }

    void add_decl(IRValue n_value) {
        decl_vec.push_back(n_value);
    }

    void add_type(std::string n_name, std::shared_ptr<Type> decl_type) {
        type_tables.insert(std::make_pair(n_name, decl_type));
    }

    virtual ~IRFunction() {}

    virtual void print(std::string prefix) {
        std::cout << prefix;
        std::cout << func_name << '(';
        std::cout << ')' << '\n';

        for(auto arg : arg_vec) {
            std::cout << "parameter " <<
            " "  << arg.to_string() << '\n';
        }

        for(auto decl : decl_vec) {
            std::cout << "decl " <<
            " "  << decl.to_string() << '\n';
        }
    };
};

class CodeGenContext {
public:
    std::map<std::string, std::shared_ptr<IRFunction>> func_defs;
    std::shared_ptr<IRFunction> cur_function;
    std::shared_ptr<IRBlock> cur_block;

    IRValue generate_temp() {
        return IRValue::from_bid(
            cur_function->type_tables,
            cur_block->bid,
            cur_block->statements.size() - 1
        );
    }

    int append_block() {
        int new_bid = cur_block->bid + 1;
        cur_block = std::make_shared<IRBlock>(new_bid);
        cur_function->body[new_bid] = cur_block;
        return new_bid;
    }

    IRValue append_statement(std::shared_ptr<IR> new_ir) {
        cur_block->statements.push_back(new_ir);
        int index = cur_block->statements.size();
        auto return_type = new_ir->result_type();
        auto return_value = IRValue(
            IROpearndType::temp,
            return_type,
            TempOperand {
                cur_block->bid,
                index - 1
            }
        );
        cur_function->add_type(return_value.to_string(), return_type);
        return return_value;
    }

    std::shared_ptr<IRFunction> find_func_by_name(std::string &name) {
        return func_defs[name];
    }

    void print() {
        for(auto it = func_defs.begin(); it != func_defs.end(); it++) {
            auto func = it->second;
            func->print(std::string(""));
            for(std::size_t bid = 0; bid < func->body.size(); bid++) {
                auto block_p = func->body[bid];
                std::cout << "b" << bid << ':' << '\n';
                for(std::size_t index = 0; index < block_p->statements.size(); index++) { 
                    auto cur_ir = block_p->statements[index];
                    auto prefix = std::string("\t<") +
                        cur_ir->result_type()->get_type_name() + ' ' +
                        std::to_string(bid) + ':' +
                        std::to_string(index) + ">: ";
                    block_p->statements[index]->print(prefix);
                    std::cout << '\n';
                }
                block_p->jump.print(std::string("\t"));
                std::cout <<'\n';
            }
        }
    }
};

#endif