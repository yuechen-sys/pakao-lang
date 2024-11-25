###
 # @Author: yuechen-sys yuechen@kaist.ac.kr
 # @Date: 2024-11-22 11:06:29
 # @LastEditors: yuechen-sys yuechen@kaist.ac.kr
 # @LastEditTime: 2024-11-25 11:54:17
 # @FilePath: /pakao-lang/build.sh
 # @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
### 

pushd src
flex -o semic_token.cpp semic.l
bison -d -o semic_grammar.cpp semic.y
popd
mv ./src/semic_grammar.hpp ./inlcude/
mkdir -p build/ && cd build
cmake ..
cmake --build . 