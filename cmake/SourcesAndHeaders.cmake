set(sources
    src/semic_grammar.cpp
    src/semic_token.cpp
)

set(exe_sources
    src/main.cpp
    ${sources}
)

set(headers
    include/semic_grammar.hpp
    include/node.hpp
    include/ir.hpp
    include/type_system.hpp
)

set(lex_sources
    src/semic.l
)

set(yacc_sources
    src/semic.y
)

set(test_sources
)
