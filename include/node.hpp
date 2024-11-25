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
    virtual IR* codeGen(CodeGenContext& context) { }
};

class NExpression : public Node {
};

class NStatement : public Node {
};

class NInteger : public NExpression {
public:
    long long value;
    NInteger(long long value) : value(value) { }
    virtual IR* codeGen(CodeGenContext& context);
};

class NDouble : public NExpression {
public:
    double value;
    NDouble(double value) : value(value) { }
    virtual IR* codeGen(CodeGenContext& context);
};

class NIdentifier : public NExpression {
public:
    int pointer_level;
    std::shared_ptr<std::string> name;
    NIdentifier(std::shared_ptr<std::string> name, int pointer_level) : 
        name(name), pointer_level(pointer_level) { }
    virtual IR* codeGen(CodeGenContext& context);
};

class NMethodCall : public NExpression {
public:
    const NIdentifier& id;
    ExpressionList arguments;
    NMethodCall(const NIdentifier& id, ExpressionList& arguments) :
        id(id), arguments(arguments) { }
    NMethodCall(const NIdentifier& id) : id(id) { }
    virtual IR* codeGen(CodeGenContext& context);
};

class NBinaryOperator : public NExpression {
public:
    int op;
    NExpression& lhs;
    NExpression& rhs;
    NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
        lhs(lhs), rhs(rhs), op(op) { }
    virtual IR* codeGen(CodeGenContext& context);
};

class NAssignment : public NExpression {
public:
    NIdentifier& lhs;
    NExpression& rhs;
    NAssignment(NIdentifier& lhs, NExpression& rhs) : 
        lhs(lhs), rhs(rhs) { }
    virtual IR* codeGen(CodeGenContext& context);
};

class NDeclaration : public NExpression {
public:
    const int type;
    NIdentifier& id;
    NDeclaration(const int &type, NIdentifier& id) : 
        type(type), id(id) { }
    virtual IR* codeGen(CodeGenContext& context);
};

class NBlock : public NExpression {
public:
    std::shared_ptr<StatementList> statements = std::make_shared<StatementList>();
    NBlock() {}
    virtual IR* codeGen(CodeGenContext& context);
};

class NExpressionStatement : public NStatement {
public:
    std::shared_ptr<NExpression> expression;
    NExpressionStatement(std::shared_ptr<NExpression> expression) : 
        expression(expression) { }
    virtual IR* codeGen(CodeGenContext& context);
};

class NVariableDeclaration : public NStatement {
public:
    int type;
    std::shared_ptr<NIdentifier> id;
    std::shared_ptr<NExpression> assign;
    NVariableDeclaration(int type, std::shared_ptr<NIdentifier> id) :
        type(type), id(id) { }
    NVariableDeclaration(int type, std::shared_ptr<NIdentifier> id,
        std::shared_ptr<NExpression> assign) :
        type(type), id(id), assign(assign) { }
    virtual IR* codeGen(CodeGenContext& context);
};

class NFunctionDeclaration : public NStatement {
public:
    int type;
    std::shared_ptr<NIdentifier> id;
    std::shared_ptr<VariableList> arguments;
    std::shared_ptr<StatementList> statements;
    NFunctionDeclaration(int type, std::shared_ptr<NIdentifier> id, 
            std::shared_ptr<VariableList> arguments, std::shared_ptr<StatementList> statements) :
        type(type), id(id), arguments(arguments), statements(statements) { }
    virtual IR* codeGen(CodeGenContext& context);
};

#endif