#ifndef SEMIC_TYPE_SYSTEM_HPP_INCLUDED
#define SEMIC_TYPE_SYSTEM_HPP_INCLUDED

#include <string>
#include <memory>

class Type {
public:
    enum TypeID {
        VoidTyID,
        FloatTyID,
        DoubleTyID,
        IntegerTyID,
        PointerTyID,
        ArrayTyID, 
        FunctionTyID,
        StringTyID,
    } type_id;

    Type(TypeID n_type_id) : type_id(n_type_id) {}
    virtual ~Type() {}

    virtual std::string get_type_name() = 0;

    virtual std::shared_ptr<Type> get_element_type() = 0;

    virtual size_t get_size() = 0;
};

class VoidType : public Type {
public:
    VoidType() : Type(TypeID::VoidTyID) {}

    virtual std::string get_type_name() override {
        return std::string("void");
    }

    virtual std::shared_ptr<Type> get_element_type() override {
        return std::make_shared<VoidType>();
    }

    virtual size_t get_size() {
        return 0;
    };
};

class IntegerType : public Type {
public:
    int bit_width;
    IntegerType(int n_bit_width) :
       Type(TypeID::IntegerTyID), bit_width(n_bit_width) {}

    virtual std::string get_type_name() override {
        return std::string("i") + std::to_string(bit_width);
    }

    virtual std::shared_ptr<Type> get_element_type() override {
        return std::make_shared<VoidType>();
    }

    virtual size_t get_size() {
        return bit_width / 8;
    };
};

class FloatType : public Type {
public:
    int bit_width;
    FloatType(int n_bit_width) :
       Type(TypeID::FloatTyID), bit_width(n_bit_width) {}

    virtual std::string get_type_name() override {
        return std::string("f") + std::to_string(bit_width);
    }

    virtual std::shared_ptr<Type> get_element_type() override {
        return std::make_shared<VoidType>();
    }

    virtual size_t get_size() {
        return bit_width / 8;
    };
};

// class FunctionType : public Type{
// public:
// };

class PointerType : public Type {
public:
    std::shared_ptr<Type> element_type;

    PointerType(std::shared_ptr<Type> n_element_type) :
        Type(TypeID::PointerTyID), element_type(n_element_type) {}
    
    virtual std::string get_type_name() override {
        return element_type->get_type_name() + '*';
    }

    virtual std::shared_ptr<Type> get_element_type() {
        return element_type;
    }

    virtual size_t get_size() {
        return 8;
    };
};

class ArrayType : public Type {
public:
    size_t size;
    std::shared_ptr<Type> element_type;
    ArrayType(size_t n_size, std::shared_ptr<Type> n_element_type) :
        Type(TypeID::ArrayTyID), size(n_size), element_type(n_element_type) {}
    
    virtual std::string get_type_name() override {
        return element_type->get_type_name() + '[' + 
            std::to_string(size) + ']';
    }

    virtual std::shared_ptr<Type> get_element_type() {
        return element_type;
    }

    virtual size_t get_size() {
        return size * element_type->get_size();
    };
};

class StringType : public Type {
public:
    StringType() : Type(TypeID::StringTyID) {}

    virtual std::string get_type_name() override {
        return std::string("str");
    }

    virtual std::shared_ptr<Type> get_element_type() {
        return std::make_shared<VoidType>();
    }

    virtual size_t get_size() {
        return 8;
    };
};

#endif