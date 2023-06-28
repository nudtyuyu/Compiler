# Generate lexer and parser with ANTLR
list(APPEND CMAKE_MODULE_PATH "${ANTLR_RUNTIME}/cmake")
include(FindANTLR)
antlr_target(SysYGen SysY.g4
  LEXER PARSER
  OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  VISITOR
)

add_library(SysYParser SHARED ${ANTLR_SysYGen_CXX_OUTPUTS})
target_include_directories(SysYParser PUBLIC ${ANTLR_RUNTIME}/runtime/src)
target_link_libraries(SysYParser PUBLIC antlr4_shared)

#backend/aarch32_target.hpp backend/codegen.hpp backend/target_info.hpp
add_executable(sysyc
  sysyc.cpp
  IR.cpp
  SysYIRGenerator.cpp
  Diagnostic.cpp
  SymTable.cpp
  backend/CodeGen.cpp
  backend/RegManager.cpp
)
target_include_directories(sysyc PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(sysyc PRIVATE "backend")
target_include_directories(sysyc PRIVATE ".")
target_compile_options(sysyc PRIVATE -O0 -Wall -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable)
target_link_libraries(sysyc PRIVATE SysYParser)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(sysyc PRIVATE Threads::Threads)
