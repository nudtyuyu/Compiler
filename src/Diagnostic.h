#pragma once

#include "antlr4-runtime.h"
#include <cstdlib>
#include <iostream>
#include <string>

namespace sysy {

class Diagnostic {
private:
  const std::string message;

public:
  Diagnostic(const std::string &message) : message(message) {}
  Diagnostic(std::string &&message) : message(std::move(message)) {}
  Diagnostic(const Diagnostic &) = default;
  Diagnostic(Diagnostic &&) = default;

public:
  ~Diagnostic() {
    if (not message.empty()) {
      std::cerr << message << '\n';
      exit(EXIT_FAILURE);
    }
  }
}; // class Diagnostic

Diagnostic error(antlr4::ParserRuleContext *ctx, const std::string &msg);

} // namespace sysy