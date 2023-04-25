#include <sstream>
using namespace std;
#include "Diagnostic.h"

namespace sysy {

Diagnostic error(antlr4::ParserRuleContext *ctx, const std::string &msg) {
  auto line = ctx->start->getLine();
  auto col = ctx->start->getCharPositionInLine();
  ostringstream ss;
  ss << line << ':' << col << ": error " << msg << '\n';
  return Diagnostic(ss.str());
}

} // namespace sysy