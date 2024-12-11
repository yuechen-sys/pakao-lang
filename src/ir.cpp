#include "ir.hpp"
#include "sim.hpp"

void IRBinary::run(FunctionContext* context) {
    auto lhs_value = context->get_value(lhs);
    auto rhs_value = context->get_value(rhs);

    // std::cout << '<' << lhs.to_string() << '>' << ' ' <<
    //     ir_optype_to_str(type)<< ' ' << '<' <<
    //     rhs.to_string() << '>' << '\n';

    if (type == IROptype::assign) {
        lhs_value->value = rhs_value->value;
        context->symbol_table[lhs.get_name()] = lhs_value;
    } else {
        auto result = lhs_value->run_binary_op(type, *rhs_value);
        TempOperand temp{context->bid, context->index};
        context->set_temp_value(temp, result);
    }
}

void IRUnary::run(FunctionContext* context) {
    auto lhs_value = context->get_value(lhs);
    auto result = lhs_value->run_unary_op(type);
    TempOperand temp{context->bid, context->index};
    context->set_temp_value(temp, result);
    context->symbol_table[lhs.get_name()] = result;
}

void IRPrintf::run(FunctionContext* context) {
    if (arguments.empty()) return;

    std::cout << "printf(";

    // 打印参数内容
    for (size_t i = 0; i < arguments.size(); ++i) {
        auto arg_value = context->get_value(arguments[i]);

        // 如果是数组类型，传递指针
        if (arg_value->type->type_id == Type::TypeID::ArrayTyID) {
            auto element_type = arg_value->type->get_element_type();
            auto element_size = element_type->get_size();
            auto array_ptr = std::get<void*>(arg_value->value);
            std::cout << '[';
            for (size_t j = 0; j < arg_value->type->get_size() / element_size; ++j) {
                if (element_type->type_id == Type::TypeID::IntegerTyID) {
                    std::cout << static_cast<uint32_t*>(array_ptr)[j];
                } else if (element_type->type_id == Type::TypeID::FloatTyID) {
                    std::cout << static_cast<float*>(array_ptr)[j];
                } else if (element_type->type_id == Type::TypeID::DoubleTyID) {
                    std::cout << static_cast<double*>(array_ptr)[j];
                } else if (element_type->type_id == Type::TypeID::StringTyID) {
                    std::cout << static_cast<std::string*>(array_ptr)[j];
                }
                if (j < arg_value->type->get_size() / element_size - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << ']';
        } else {
            std::visit([](auto&& val) {
                std::cout << val;
            }, arg_value->value);
        }

        if (i < arguments.size() - 1) {
            std::cout << ", ";
        }
    }

    std::cout << ')' << '\n';
}

void IRMethodCall::run(FunctionContext* context) {
    auto func = context->sim->context->find_func_by_name(name);
    auto new_context = std::make_shared<FunctionContext>(func, context->sim);

    std::cout << name << '(';

    // 将参数传递给新函数上下文
    for (size_t i = 0; i < arguments.size(); ++i) {
        auto arg_value = context->get_value(arguments[i]);

        // 如果是数组类型，传递指针
        if (arg_value->type->type_id == Type::TypeID::ArrayTyID) {
            auto element_type = arg_value->type->get_element_type();
            auto element_size = element_type->get_size();
            auto array_ptr = std::get<void*>(arg_value->value);
            auto pointer_value = std::make_shared<Register>();
            pointer_value->type = std::make_shared<PointerType>(element_type);
            pointer_value->value = array_ptr;
            new_context->symbol_table[func->arg_vec[i].get_name()] = pointer_value;

            // 打印数组内容
            std::cout << '[';
            for (size_t j = 0; j < arg_value->type->get_size() / element_size; ++j) {
                if (element_type->type_id == Type::TypeID::IntegerTyID) {
                    std::cout << static_cast<uint32_t*>(array_ptr)[j];
                } else if (element_type->type_id == Type::TypeID::FloatTyID) {
                    std::cout << static_cast<float*>(array_ptr)[j];
                } else if (element_type->type_id == Type::TypeID::DoubleTyID) {
                    std::cout << static_cast<double*>(array_ptr)[j];
                } else if (element_type->type_id == Type::TypeID::StringTyID) {
                    std::cout << static_cast<std::string*>(array_ptr)[j];
                }
                if (j < arg_value->type->get_size() / element_size - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << ']';
        } else {
            new_context->symbol_table[func->arg_vec[i].get_name()] = arg_value;
            std::visit([](auto&& val) {
                std::cout << val;
            }, arg_value->value);
        }

        if (i < arguments.size() - 1) {
            std::cout << ", ";
        }
    }

    std::cout << ')' << '\n';

    context->sim->st.push(new_context);
}

void IRJump::run(FunctionContext* context) {
    switch (jump_type) {
    case IRJumpType::direct: {
        auto direct_jump = std::get<DirectJump>(jump);
        context->bid = direct_jump.jump_bid;
        context->index = 0;
        break;
    }
    case IRJumpType::cond: {
        auto cond_jump = std::get<CondJump>(jump);
        auto cond_value = context->get_value(cond_jump.cond_value);
        if (std::get<uint32_t>(cond_value->value)) {
            context->bid = cond_jump.true_bid;
        } else {
            context->bid = cond_jump.false_bid;
        }
        context->index = 0;
        break;
    }
    case IRJumpType::ret: {
        auto ret_jump = std::get<RetJump>(jump);
        auto return_value = context->get_value(ret_jump.return_value);
        context->symbol_table["return"] = return_value;

        // 打印返回值
        std::cout << "return(";
        std::visit([](auto&& val) {
            std::cout << val;
        }, return_value->value);
        std::cout << ")" << std::endl;

        break;
    }
    default:
        break;
    }
}

void IRArrayIndex::run(FunctionContext* context) {
    auto array_value = context->get_value(array);
    auto index_value = context->get_value(index);
    auto element_type = array_value->type->get_element_type();
    auto element_size = element_type->get_size();

    auto array_ptr = std::get<void*>(array_value->value);
    auto index = std::get<uint64_t>(index_value->value);
    auto element_ptr = static_cast<char*>(array_ptr) + index * element_size;

    auto result = std::make_shared<Register>();
    result->type = std::make_shared<PointerType>(element_type);
    result->value = static_cast<void*>(element_ptr);

    TempOperand temp{context->bid, context->index};
    context->set_temp_value(temp, result);
}

void IRLoad::run(FunctionContext* context) {
    auto pointer_value = context->get_value(pointer);
    auto element_type = pointer_value->type->get_element_type();
    auto element_ptr = std::get<void*>(pointer_value->value);

    auto result = std::make_shared<Register>();
    result->type = element_type;

    if (element_type->type_id == Type::TypeID::IntegerTyID) {
        result->value = (uint64_t)(*static_cast<uint32_t*>(element_ptr));
    } else if (element_type->type_id == Type::TypeID::FloatTyID) {
        result->value = *static_cast<float*>(element_ptr);
    } else if (element_type->type_id == Type::TypeID::DoubleTyID) {
        result->value = *static_cast<double*>(element_ptr);
    } else if (element_type->type_id == Type::TypeID::StringTyID) {
        result->value = *static_cast<std::string*>(element_ptr);
    } else {
        throw std::runtime_error("Unsupported type");
    }

    TempOperand temp{context->bid, context->index};
    context->set_temp_value(temp, result);
}

void IRStore::run(FunctionContext* context) {
    auto pointer_value = context->get_value(pointer);
    auto value_value = context->get_value(value);
    auto element_ptr = std::get<void*>(pointer_value->value);

    if (value_value->type->type_id == Type::TypeID::IntegerTyID) {
        *static_cast<uint64_t*>(element_ptr) = std::get<uint64_t>(value_value->value);
    } else if (value_value->type->type_id == Type::TypeID::FloatTyID) {
        *static_cast<float*>(element_ptr) = std::get<float>(value_value->value);
    } else if (value_value->type->type_id == Type::TypeID::DoubleTyID) {
        *static_cast<double*>(element_ptr) = std::get<double>(value_value->value);
    } else if (value_value->type->type_id == Type::TypeID::StringTyID) {
        *static_cast<std::string*>(element_ptr) = std::get<std::string>(value_value->value);
    } else {
        throw std::runtime_error("Unsupported type");
    }
}

void IRCast::run(FunctionContext* context) {
    auto value_reg = context->get_value(value);
    auto result = std::make_shared<Register>();
    result->type = target_type;

    if (target_type->type_id == Type::TypeID::IntegerTyID) {
        if (value_reg->type->type_id == Type::TypeID::FloatTyID) {
            result->value = static_cast<uint64_t>(std::get<float>(value_reg->value));
        } else if (value_reg->type->type_id == Type::TypeID::DoubleTyID) {
            result->value = static_cast<uint64_t>(std::get<double>(value_reg->value));
        }
    } else if (target_type->type_id == Type::TypeID::FloatTyID) {
        if (value_reg->type->type_id == Type::TypeID::IntegerTyID) {
            result->value = static_cast<float>(std::get<uint64_t>(value_reg->value));
        } else if (value_reg->type->type_id == Type::TypeID::DoubleTyID) {
            result->value = static_cast<float>(std::get<double>(value_reg->value));
        }
    } else if (target_type->type_id == Type::TypeID::DoubleTyID) {
        if (value_reg->type->type_id == Type::TypeID::IntegerTyID) {
            result->value = static_cast<double>(std::get<uint64_t>(value_reg->value));
        } else if (value_reg->type->type_id == Type::TypeID::FloatTyID) {
            result->value = static_cast<double>(std::get<float>(value_reg->value));
        }
    } else {
        throw std::runtime_error("Unsupported type cast");
    }

    TempOperand temp{context->bid, context->index};
    context->set_temp_value(temp, result);
}