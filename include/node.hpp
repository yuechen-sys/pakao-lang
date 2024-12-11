#ifndef SEMIC_NODES_HPP_INCLUDED
#define SEMIC_NODES_HPP_INCLUDED

#include <iostream>
#include <vector>
#include <variant>

#include <memory>

#include "ir.hpp"

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;

typedef std::vector<std::shared_ptr<NStatement>> StatementList;
typedef std::vector<std::shared_ptr<NExpression>> ExpressionList;
typedef std::vector<std::shared_ptr<NVariableDeclaration>> VariableList;

class Node {
public:
    virtual ~Node() {}
    virtual void print() {}
};

class NExpression : public Node {
public:
    virtual IRValue codeGen(CodeGenContext& context) = 0;
};

class NStatement : public Node {
public:
    virtual void codeGen(CodeGenContext& context) = 0;
};

class NIdentifier : public NExpression {
public:
    int pointer_level;
    std::shared_ptr<std::string> name;
    int array_size;

    NIdentifier(std::shared_ptr<std::string> n_name) : name(n_name) { }

    virtual void print() {
        for(int i = 0; i < pointer_level; i++) {
            std::cout << "*";
        }
        if (name)
            std::cout << *name;
        if (array_size) {
            std::cout << '[' << array_size << ']';
        }
    }

    std::string get_name() {
        return *name;
    }

    IRValue to_ir(CodeGenContext& context) {
        return IRValue::from_operand(
            context.cur_function->type_tables,
            *name
        );
    }

    virtual ~NIdentifier() { }

    virtual IRValue codeGen(CodeGenContext& context) {
        return to_ir(context);
    }
};


class NType : Node {
public:
    int type;
    std::string to_string() {
        switch (type)
        {
        case 293 :
            return "INT";
            break;
        case 297 :
            return "FLOAT";
            break;
        case 294 :
            return "LONG";
            break;
        case 299 :
            return "VOID";
            break;
        case 298:
            return "DOUBLE";
            break;
        case 291 :
            return "CHAR";
            break;
        default:
            return "UNKOWN";
            break;
        }
    }

    virtual void print() {
        std::cout << to_string();
    }

    virtual std::shared_ptr<Type> result_type(NIdentifier &id) {
        std::shared_ptr<Type> type_ptr;
        switch (type)
        {
        case 293 :
            type_ptr = std::make_shared<IntegerType>(32);
            break;
        case 297 :
            type_ptr = std::make_shared<FloatType>(32);
            break;
        case 294 :
            type_ptr = std::make_shared<IntegerType>(64);
            break;
        case 299 :
            type_ptr = std::make_shared<VoidType>();
            break;
        case 298:
            type_ptr = std::make_shared<FloatType>(64);
            break;
        case 291 :
            type_ptr = std::make_shared<IntegerType>(8);
            break;
        default:
            type_ptr = std::make_shared<VoidType>();
            break;
        }
        for(int i = 0; i < id.pointer_level; i++) {
            type_ptr = std::make_shared<PointerType>(type_ptr);
        }
        if (id.array_size > 0) {
            type_ptr = std::make_shared<ArrayType>(id.array_size, type_ptr);
        }
        return type_ptr;
    }

    NType(int n_type) : type(n_type) {}
};

class NOpType : public Node {
public:
    int type;

    std::string to_string() {
        switch (type) {
        case 285 :
            return "ADD";
            break;
        case 284 :
            return "MINUS";
            break;
        case 287 :
            return "DIV";
            break;
        case 286 :
            return "MUL";
            break;
        case 271 :
            return "AND";
            break;
        case 272 :
            return "OR";
            break;
        default:
            return "UNKOWN";
            break;
        }
    }

    IROptype to_optype() {
        switch (type) {
        case 285 :
            return IROptype::add;
            break;
        case 284 :
            return IROptype::minus;
            break;
        case 287 :
            return IROptype::divide;
            break;
        case 286 :
            return IROptype::mul;
            break;
        case 271 :
            return IROptype::land;
            break;
        case 272 :
            return IROptype::lor;
            break;
        case 289 : //LT_OP
            return IROptype::less;
            break;
        case 290 : //GT_OP
            return IROptype::greater;
            break;
        case 263 : //INC_OP
            return IROptype::inc;
            break;
        case 264 : //INC_OP
            return IROptype::dec;
            break;
        default:
            return IROptype::unknown_op;
            break;
        }
    }

    virtual void print() {
        std::cout << to_string();
    }

    NOpType(int n_type) : type(n_type) {}
};

class NInteger : public NExpression {
public:
    long long value;
    NInteger(long long n_value) : value(n_value) { }
    virtual void print() {
        std::cout << " <const: " << value << "> ";
    }
    virtual ~NInteger() {}
    virtual IRValue codeGen(CodeGenContext& context) {
        return IRValue::from_int(value);
    }
};

class NStringLiteral : public NExpression {
public:
    std::shared_ptr<std::string> string_literal;
    NStringLiteral(std::shared_ptr<std::string> n_string_literal) : 
        string_literal(n_string_literal) {}
    virtual void print() {
        std::cout << *string_literal;
    }
    virtual ~NStringLiteral() {}
    virtual IRValue codeGen(CodeGenContext& context) {
        return IRValue::from_literal_string(*string_literal);
    }
};

// class NDouble : public NExpression {
// public:
//     double value;
//     NDouble(double value) : value(value) { }
//     virtual IR* codeGen(CodeGenContext& context);
// };

class NMethodCall : public NExpression {
public:
    std::shared_ptr<NIdentifier> id;
    std::shared_ptr<ExpressionList> arguments;
    NMethodCall(std::shared_ptr<NIdentifier> n_id) : id(n_id) { }
    NMethodCall(std::shared_ptr<NIdentifier> n_id, std::shared_ptr<ExpressionList> n_arguments) :
        id(n_id), arguments(n_arguments) { }
    virtual void print() {
        id->print();
        std::cout << '(';
        for(auto it = arguments->begin(); it != arguments->end(); it++) {
            (*it)->print();
            std::cout << ", ";
        }
        std::cout << ')';
    }
    virtual IRValue codeGen(CodeGenContext& context) {
        std::string name = id->get_name();

        // generation of operand IR
        std::vector<IRValue> operand_arguments;
        for(auto it = arguments->begin(); it != arguments->end(); it++) {
            operand_arguments.push_back((*it)->codeGen(context));
        }

        // generation of function call IR
        std::shared_ptr<IR> new_ir;
        if (name == std::string("printf")) {
            new_ir = std::make_shared<IRPrintf>(
                operand_arguments
            );
        } else {
            auto func = context.find_func_by_name(name);
            new_ir = std::make_shared<IRMethodCall>(
                func->return_type,
                name,
                operand_arguments
            );
        }

        // return current SSA variable
        return context.append_statement(new_ir); 
    };
};

class NBinaryOperator : public NExpression {
public:
    std::shared_ptr<NExpression> lhs;
    NOpType op;
    std::shared_ptr<NExpression> rhs;

    NBinaryOperator(
        std::shared_ptr<NExpression> n_lhs,
        int n_op,
        std::shared_ptr<NExpression> n_rhs
    ) : lhs(n_lhs), op(n_op), rhs(n_rhs) { }

    virtual void print() {
        lhs->print();
        std::cout << " ";
        op.print();
        std::cout << " ";
        rhs->print();
        std::cout << " ";
    }

    virtual ~NBinaryOperator() { }

    virtual IRValue codeGen(CodeGenContext& context) {
        return context.append_statement(
            std::make_shared<IRBinary>(
                op.to_optype(),
                lhs->codeGen(context),
                rhs->codeGen(context)
            )
        );
    };
};

class NUnaryOperator : public NExpression {
public:
    NOpType op;
    std::shared_ptr<NExpression> lhs;
    NUnaryOperator(std::shared_ptr<NExpression> n_lhs, int n_op) :
        lhs(n_lhs), op(n_op) { }
    virtual void print() {
        op.print();
        std::cout << " ";
        lhs->print();
    }
    virtual ~NUnaryOperator() { }
    virtual IRValue codeGen(CodeGenContext& context) {
        return context.append_statement(
            std::make_shared<IRUnary>(
                op.to_optype(),
                lhs->codeGen(context)
            )
        );
    };
};

class NArrayIndex : public NExpression {
public:
    std::shared_ptr<NExpression> lhs;
    std::shared_ptr<NExpression> rhs;
    NArrayIndex(std::shared_ptr<NExpression> n_lhs,
        std::shared_ptr<NExpression> n_rhs) : 
        lhs(n_lhs), rhs(n_rhs) { }

    virtual void print() {
        lhs->print();
        std::cout << '[';
        rhs->print();
        std::cout << ']';
    }

    virtual ~NArrayIndex() { }

    virtual IRValue codeGen(CodeGenContext& context) {
        return context.append_statement(
            std::make_shared<IRArrayIndex>(
                lhs->codeGen(context),
                rhs->codeGen(context)
            )
        );
    };
};

class NAssignment : public NExpression {
public:
    std::shared_ptr<NExpression> lhs;
    std::shared_ptr<NExpression> rhs;

    NAssignment(std::shared_ptr<NExpression> n_lhs,
        std::shared_ptr<NExpression> n_rhs) : 
        lhs(n_lhs), rhs(n_rhs) { }

    virtual void print() {
        lhs->print();
        std::cout << " = ";
        rhs->print();
        std::cout << ' ';
    }

    virtual ~NAssignment() { }

    virtual IRValue codeGen(CodeGenContext& context) {
        IRValue lhs_value = lhs->codeGen(context),
            rhs_value = rhs->codeGen(context);
        return context.append_statement(
            std::make_shared<IRBinary>(IROptype::assign, lhs_value, rhs_value)
        );
    };
};

// class NDeclaration : public NExpression {
// public:
//     const int type;
//     NIdentifier& id;
//     NDeclaration(const int &type, NIdentifier& id) : 
//         type(type), id(id) { }
//     virtual IR* codeGen(CodeGenContext& context);
// };

class NBlock : public NExpression {
public:
    std::shared_ptr<StatementList> statements = std::make_shared<StatementList>();
    NBlock() {}
    void push_back(std::shared_ptr<NStatement> n_statement) {
        statements->push_back(n_statement);
    }
    virtual void print() {
        for(auto statement : *statements) {
            statement->print();
            std::cout << '\n';
        }
    }
    virtual ~NBlock() { }
    virtual IRValue codeGen(CodeGenContext& context) {
        for(auto it = statements->begin(); it != statements->end(); it++) {
            (*it)->codeGen(context);
        }
        return context.generate_temp();
    }
};

class NExpressionStatement : public NStatement {
public:
    std::shared_ptr<NExpression> expression;
    NExpressionStatement(std::shared_ptr<NExpression> n_expression) : 
        expression(n_expression) { }
    virtual void print() {
        if(expression)
            expression->print();
    }
    virtual ~NExpressionStatement() { }
    virtual void codeGen(CodeGenContext& context) {
        expression->codeGen(context);
    };
};

class NVariableDeclaration : public NStatement {
public:
    NType type;
    std::shared_ptr<NIdentifier> id;
    std::shared_ptr<NExpression> assign;
    NVariableDeclaration(int n_type, std::shared_ptr<NIdentifier> n_id) :
        type(n_type), id(n_id) { }
    NVariableDeclaration(int n_type, std::shared_ptr<NIdentifier> n_id,
        std::shared_ptr<NExpression> n_assign) :
        type(n_type), id(n_id), assign(n_assign) { }
    virtual void print() {
        type.print();
        std::cout << " ";
        if(id) {
            id->print();
        }
        if(assign) {
            std::cout << " = ";
            assign->print();
            std::cout << " ";
        }
    }
    virtual ~NVariableDeclaration() { }
    virtual void codeGen(CodeGenContext& context) {
        if(!id) return;
        auto id_value = *id;
        context.cur_function->add_decl(
            IRValue (
                IROpearndType::id,
                type.result_type(id_value),
                Operand { id_value.get_name() }
            )
        );
        context.cur_function->add_type(id_value.get_name(), type.result_type(id_value));
        if (assign) {
            IRValue assign_value = assign->codeGen(context);
            IRValue lhs = id->to_ir(context);
            context.append_statement(
                std::make_shared<IRBinary>(IROptype::assign, lhs, assign->codeGen(context))
            );
        }
    };
};

class NVariableDeclarationList : public NStatement {
public:
    std::shared_ptr<VariableList> var_list;
    NVariableDeclarationList(std::shared_ptr<VariableList> n_var_list) :
        var_list(n_var_list) { }

    void push_back(std::shared_ptr<NVariableDeclaration> n_var) {
        var_list->push_back(n_var);
    }

    void set_type(int type) {
        for(auto it = var_list->begin(); it != var_list->end(); it++) {
            (*it)->type = type;
        }
    }

    virtual void print() override {
        for(auto var: *var_list) {
            var->print();
            std::cout << '\n';
        }
    }

    virtual void codeGen(CodeGenContext& context) override {
       for(auto it = var_list->begin(); it != var_list->end(); it++) {
            (*it)->codeGen(context);
       }
    }
};

class NForStatement: public NStatement{
public:
    std::shared_ptr<NBlock>  block;
    std::shared_ptr<NExpression> initial, condition, increment;

    NForStatement(){}

    NForStatement(std::shared_ptr<NBlock> b, std::shared_ptr<NExpression> init = nullptr, std::shared_ptr<NExpression> cond = nullptr, std::shared_ptr<NExpression> incre = nullptr)
            : block(b), initial(init), condition(cond), increment(incre){
        if( condition == nullptr ){
            condition = std::make_shared<NInteger>(1);
        }
    }

    virtual void print() override {
        if( initial ) {
            initial->print();
            std::cout << '\n';
        }
        if( condition ) {
            condition->print();
            std::cout << '\n';
        }
        if( increment ) {
            increment->print();
            std::cout << '\n';
        }
        block->print();
    }

    virtual void codeGen(CodeGenContext& context) override {
        std::shared_ptr<IRBlock> orig_blk, init_blk, cond_blk, inc_blk, body_blk;
        orig_blk = context.cur_block;

        // init blk
        int init_bid = context.append_block();
        initial->codeGen(context);

        init_blk = context.cur_block;
        orig_blk->jump = IRJump::from_direct_jump(init_bid);

        // cond blk
        int cond_bid = context.append_block();
        IRValue cond_value = condition->codeGen(context);

        cond_blk = context.cur_block;
        init_blk->jump = IRJump::from_direct_jump(cond_bid);

        // body blk
        int body_bid = context.append_block();
        block->codeGen(context);

        body_blk = context.cur_block;

        // inc blk
        if (increment) {
            int inc_bid = context.append_block();
            increment->codeGen(context);
            inc_blk = context.cur_block;

            body_blk->jump = IRJump::from_direct_jump(inc_bid);
            inc_blk->jump = IRJump::from_direct_jump(cond_bid);
        } else {
            body_blk->jump = IRJump::from_direct_jump(cond_bid);
        }

        int new_bid = context.append_block();
        cond_blk->jump = IRJump::from_cond_jump(cond_value, body_bid, new_bid);
    }
};

class NIfStatement: public NStatement{
public:
    std::shared_ptr<NExpression>  condition;
    std::shared_ptr<NBlock> true_block;          // should not be null
    std::shared_ptr<NBlock> false_block;         // can be null


    NIfStatement(){}

    NIfStatement(std::shared_ptr<NExpression> cond, std::shared_ptr<NBlock> n_blk, std::shared_ptr<NBlock> n_blk2 = nullptr)
        : condition(cond), true_block(n_blk), false_block(n_blk2){
    }

    virtual void print() override {
        std::cout << "if(";
        condition->print();
        std::cout << ")\n";

        true_block->print();

        if (false_block){
            false_block->print();
        }
    }

    virtual void codeGen(CodeGenContext& context) override {
        // condition value
        IRValue cond_value = condition->codeGen(context);
        std::shared_ptr<IRBlock> cond_block = context.cur_block,
            true_block_p, false_block_p;
        int true_bid, false_bid;

        // new block
        true_bid = context.append_block();
        true_block->codeGen(context);

        // use the final block in true block
        true_block_p = context.cur_block;

        //TODO: else block
        if (false_block) {
            false_bid = context.append_block();
            false_block->codeGen(context);
            false_block_p = context.cur_block;
        }

        int new_bid = context.append_block();

        // append jump to original blk and true blk
        cond_block->jump = IRJump::from_cond_jump(cond_value, true_bid, false_block ? false_bid : new_bid);
        true_block_p->jump = IRJump::from_direct_jump(new_bid);
        if (false_block) {
            false_block_p->jump = IRJump::from_direct_jump(new_bid);
        }
    }
};

class NReturnStatement: public NStatement{
public:
    std::shared_ptr<NExpression> expression;

    NReturnStatement(){}

    NReturnStatement(std::shared_ptr<NExpression>  n_expression = nullptr)
            : expression(n_expression) {
    }

    virtual void print() override {
        std::cout << "RETURN ";
        if(expression)
            expression->print();
        std::cout << '\n';
    }

    virtual void codeGen(CodeGenContext& context) override {
        IRValue ret_value = expression->codeGen(context);
        context.cur_block->jump = IRJump::from_ret_jump(ret_value);
        context.append_block();
    }

};

class NFunctionDeclaration : public NStatement {
public:
    std::shared_ptr<NIdentifier> name;
    std::shared_ptr<NVariableDeclarationList> arguments;

    NFunctionDeclaration(std::shared_ptr<NIdentifier> n_name,
        std::shared_ptr<NVariableDeclarationList> n_arguments) :
        name(n_name), arguments(n_arguments) { }

    virtual void print() override {
        name->print();
        std::cout << " ( ";
        if (arguments) {
            arguments->print();
            std::cout << ",";
        }
        std::cout << " ) " << '\n';
    }

    virtual void codeGen(CodeGenContext& context) override {
        for(auto it = arguments->var_list->begin(); it != arguments->var_list->end(); it++) {
            auto var = *it;
            auto &id = var->id;
            auto &type = var->type;
            if(!id) continue;
            auto id_value = *id;
            context.cur_function->add_arg(
                IRValue (
                    IROpearndType::id,
                    type.result_type(id_value),
                    Operand { id_value.get_name() }
                )
            );
            context.cur_function->add_type(id_value.get_name(), type.result_type(id_value));
        }
    }
};

class NFunction : public NStatement {
public:
    NType type;
    std::shared_ptr<NFunctionDeclaration> func_decl;
    std::shared_ptr<NBlock> block;
    NFunction(int n_type,
        std::shared_ptr<NFunctionDeclaration> n_func_decl, 
        std::shared_ptr<NBlock> n_block) :
        type(n_type), func_decl(n_func_decl), block(n_block) { }

    virtual void print() override {
        type.print(); std::cout << " ";
        func_decl->print();
        block->print();
    }

    virtual void codeGen(CodeGenContext& context) override {
        auto func_id = *func_decl->name;
        auto func_name = func_id.get_name();
        std::shared_ptr<IRFunction> func_p =
            std::make_shared<IRFunction>(func_name, type.result_type(func_id));

        context.cur_function = func_p;
        context.cur_block = context.cur_function->body[0];
        context.func_defs[func_name] = func_p;
        func_decl->codeGen(context);
        block->codeGen(context);
    }
};

#endif