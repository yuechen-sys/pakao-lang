/*
 * @Author: yuechen-sys yuechen@kaist.ac.kr
 * @Date: 2024-11-22 11:13:56
 * @LastEditors: yuechen-sys yuechen@kaist.ac.kr
 * @LastEditTime: 2024-11-25 19:16:54
 * @FilePath: /pakao-lang/src/main.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include <iostream>
#include <vector>
#include <memory>

#include "node.hpp"

extern int yyparse();
extern int column;
extern std::vector<std::shared_ptr<NFunctionDeclaration>> programBlocks; /* the top level root node of our final AST */

int main() {
    yyparse();
    for(auto func_decl : programBlocks) {

    }
}
