#ifndef SEMIC_NODES_HPP_INCLUDED
#define SEMIC_NODES_HPP_INCLUDED

#include <iostream>
#include <vector>

#include <memory>

#include "ir.hpp"

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;

typedef std::vector<std::shared_ptr<NStatement>> StatementList;
typedef std::vector<NExpression*> ExpressionList;
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
    std::string name;
    NIdentifier(const std::string& name) : name(name) { }
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

class NFuncDeclaration : public NDeclaration {
public:
    std::shared_ptr<VariableList> parameter_list;
    std::shared_ptr<StatementList> statements;
    NFuncDeclaration(int type, NIdentifier& id,
        std::shared_ptr<VariableList> parameter_list,
        std::shared_ptr<StatementList> statements) : 
        NDeclaration(type, id), parameter_list(parameter_list), statements(statements) { }
    virtual IR* codeGen(CodeGenContext& context);
};

class NBlock : public NExpression {
public:
    NDeclaration *decl;
    NBlock(NDeclaration *decl) : 
        decl(decl) { }
    virtual IR* codeGen(CodeGenContext& context);
};

class NExpressionStatement : public NStatement {
public:
    NExpression& expression;
    NExpressionStatement(NExpression& expression) : 
        expression(expression) { }
    virtual IR* codeGen(CodeGenContext& context);
};

class NVariableDeclaration : public NStatement {
public:
    int type;
    NIdentifier id;
    NExpression *assignmentExpr;
    NVariableDeclaration(int type, NIdentifier& id) :
        type(type), id(id) { }
    NVariableDeclaration(int type, NIdentifier& id, NExpression *assignmentExpr) :
        type(type), id(id), assignmentExpr(assignmentExpr) { }
    virtual IR* codeGen(CodeGenContext& context);
};

class NFunctionDeclaration : public NStatement {
public:
    const int type;
    const NIdentifier& id;
    VariableList arguments;
    NBlock& block;
    NFunctionDeclaration(int type, const NIdentifier& id, 
            const VariableList& arguments, NBlock& block) :
        type(type), id(id), arguments(arguments), block(block) { }
    virtual IR* codeGen(CodeGenContext& context);
};

#endif