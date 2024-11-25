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
    virtual IR* codeGen(CodeGenContext& context) { return nullptr; }
};

class NExpression : public Node {
};

class NStatement : public Node {
};

// class NInteger : public NExpression {
// public:
//     long long value;
//     NInteger(long long value) : value(value) { }
//     virtual IR* codeGen(CodeGenContext& context);
// };

// class NDouble : public NExpression {
// public:
//     double value;
//     NDouble(double value) : value(value) { }
//     virtual IR* codeGen(CodeGenContext& context);
// };

class NIdentifier : public NExpression {
public:
    int pointer_level;
    std::shared_ptr<std::string> name;
    NIdentifier(std::shared_ptr<std::string> n_name, int n_pointer_level) : 
        name(n_name), pointer_level(n_pointer_level) { }
    virtual ~NIdentifier() { }
    virtual IR* codeGen(CodeGenContext& context) { return nullptr; };
};

// class NMethodCall : public NExpression {
// public:
//     const NIdentifier& id;
//     ExpressionList arguments;
//     NMethodCall(const NIdentifier& id, ExpressionList& arguments) :
//         id(id), arguments(arguments) { }
//     NMethodCall(const NIdentifier& id) : id(id) { }
//     virtual IR* codeGen(CodeGenContext& context);
// };

class NBinaryOperator : public NExpression {
public:
    int op;
    std::shared_ptr<NExpression> lhs, rhs;
    NBinaryOperator(std::shared_ptr<NExpression> n_lhs, int n_op,
        std::shared_ptr<NExpression> n_rhs) :
        lhs(n_lhs), rhs(n_rhs), op(n_op) { }
    virtual ~NBinaryOperator() { }
    virtual IR* codeGen(CodeGenContext& context) { return nullptr; };
};

class NAssignment : public NExpression {
public:
    std::shared_ptr<NIdentifier> lhs;
    std::shared_ptr<NExpression> rhs;
    NAssignment(std::shared_ptr<NIdentifier> n_lhs,
        std::shared_ptr<NExpression> n_rhs) : 
        lhs(n_lhs), rhs(n_rhs) { }
    virtual ~NAssignment() { }
    virtual IR* codeGen(CodeGenContext& context) { return nullptr; };
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
    virtual ~NBlock() { }
    virtual IR* codeGen(CodeGenContext& context) {  return nullptr;  }
};

class NExpressionStatement : public NStatement {
public:
    std::shared_ptr<NExpression> expression;
    NExpressionStatement(std::shared_ptr<NExpression> expression) : 
        expression(expression) { }
    virtual ~NExpressionStatement() { }
    virtual IR* codeGen(CodeGenContext& context) {  return nullptr;  };
};

class NVariableDeclaration : public NStatement {
public:
    int type;
    std::shared_ptr<NIdentifier> id;
    std::shared_ptr<NExpression> assign;
    NVariableDeclaration(int n_type, std::shared_ptr<NIdentifier> n_id) :
        type(n_type), id(n_id) { }
    NVariableDeclaration(int n_type, std::shared_ptr<NIdentifier> n_id,
        std::shared_ptr<NExpression> n_assign) :
        type(n_type), id(n_id), assign(n_assign) { }
    virtual ~NVariableDeclaration() { }
    virtual IR* codeGen(CodeGenContext& context) { return nullptr; };
};

class NFunctionDeclaration : public NStatement {
public:
    int type;
    std::shared_ptr<NIdentifier> id;
    std::shared_ptr<VariableList> arguments;
    std::shared_ptr<NBlock> block;
    NFunctionDeclaration(int n_type, std::shared_ptr<NIdentifier> n_id, 
            std::shared_ptr<VariableList> n_arguments, std::shared_ptr<NBlock> n_block) :
        type(n_type), id(n_id), arguments(n_arguments), block(n_block) { }
    virtual ~NFunctionDeclaration() { }
    virtual IR* codeGen(CodeGenContext& context) { return nullptr; };
};

#endif