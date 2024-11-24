### Introduction

`pakao-lang` is a semi-c (the language support part of standard C language grammars). This project is developed based on [C++ Project Template](https://github.com/filipdutescu/modern-cpp-template). And as a reference, my design of `lex` and `yacc` is originated from [Tiny Compiler](https://github.com/Mustafa-khann/Tiny-Compiler)

### Overview

#### Features

- Lexcial analysis & Parsing
- AST generation 
- SSA-based flow graph generation
- Runtime memory allocation & management
- Interpreter-based execution
- (Experimental) IR code optimization

#### Prerequisites

- `gcc` support C++ std17
- `cmake` versino above 3.15.*
- `lex` & `yacc`

### Install