#include "ir.hpp"
#include "sim.hpp"

void IRBinary::run(FunctionContext* context) {
    auto lhs_value = context->get_value(lhs);
    auto rhs_value = context->get_value(rhs);

    std::cout << '<' << lhs.to_string() << '>' << ' ' <<
        ir_optype_to_str(type)<< '<' <<
        rhs.to_string() << '>' << '\n';

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
}

void IRPrintf::run(FunctionContext* context) {
    if (arguments.empty()) return;

    // 获取 format 字符串
    auto format_value = context->get_value(arguments[0]);
    std::string format = std::get<std::string>(format_value->value);

    // 获取剩余的参数
    std::vector<std::string> args;
    for (size_t i = 1; i < arguments.size(); ++i) {
        auto value = context->get_value(arguments[i]);
        std::visit([&args](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) {
                args.push_back(std::to_string(val));
            } else if constexpr (std::is_same_v<T, double>) {
                args.push_back(std::to_string(val));
            } else if constexpr (std::is_same_v<T, std::string>) {
                args.push_back(val);
            } else {
                throw std::runtime_error("Unsupported type");
            }
        }, value->value);
    }

    // 格式化字符串
    size_t pos = 0;
    for (const auto& arg : args) {
        pos = format.find("%", pos);
        if (pos == std::string::npos) break;
        format.replace(pos, 1, arg);
        pos += arg.length();
    }

    // 打印最终的格式化字符串
    std::printf("%s", format.c_str());
}

void IRMethodCall::run(FunctionContext* context) {
    auto func = context->sim->context->find_func_by_name(name);
    auto new_context = std::make_shared<FunctionContext>(func, context->sim);

    // 将参数传递给新函数上下文
    for (size_t i = 0; i < arguments.size(); ++i) {
        auto arg_value = context->get_value(arguments[i]);
        new_context->symbol_table[func->arg_vec[i].get_name()] = arg_value;
    }

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
        break;
    }
    default:
        break;
    }
}

void IRArrayIndex::run(FunctionContext* context) {
    auto array_value = context->get_value(array);
    auto index_value = context->get_value(index);
    // Implement array index logic if needed
}