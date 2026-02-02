// CommandInterpreter.cpp
#include "CommandInterpreter.h"

#include <cerrno>
#include <chrono>
#include <cstring>

namespace {

  bool isOpToken(const std::string& t) {
    return (t == "+") || (t == "-") || (t == "*") || (t == "/");
  }

  int precedence(const std::string& op) {
    if (op == "*" || op == "/") return 2;
    if (op == "+" || op == "-") return 1;
    return 0;
  }

  std::optional<long long> parseIntStrict(const std::string& s) {
    long long v{};
    const char* b = s.data();
    const char* e = s.data() + s.size();
    auto r = std::from_chars(b, e, v);
    if (r.ec != std::errc{} || r.ptr != e) return std::nullopt;
    return v;
  }

  std::optional<long long> applyOp(long long a, long long b, const std::string& op) {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") {
      if (b == 0) return std::nullopt;
      return a / b;
    }
    return std::nullopt;
  }

  // Accepts "$i" or "${i}" or "i" (returns "i"). Returns nullopt on malformed "${...".
  std::optional<std::string> extractVarName(std::string_view token) {
    if (token.empty()) return std::nullopt;

    if (token[0] != '$') {
      return std::string(token);
    }

    if (token.size() >= 2 && token[1] != '{') {
      return std::string(token.substr(1));
    }

    if (token.size() >= 4 && token[1] == '{' && token.back() == '}') {
      return std::string(token.substr(2, token.size() - 3));
    }

    return std::nullopt;
  }

  bool isIfOp(const std::string& op) {
    return op == "==" || op == "!=" || op == "<" || op == "<=" || op == ">" || op == ">=";
  }

} // namespace

CommandInterpreter::CommandInterpreter() = default;

CommandResultCode CommandInterpreter::registerCommand(const std::string& commandName,
                                                      Callback callbackFunction) {
  commandMap[commandName].push_back(std::move(callbackFunction));
  return CommandResultCode::success;
}

void CommandInterpreter::setUnknownCommandHandler(UnknownCommandCallback callbackFunction) {
  unknownCommandHandler = std::move(callbackFunction);
}

CommandResultCode CommandInterpreter::loadFromFile(const std::string& filePath) {
  std::ifstream inStream(filePath);
  if (!inStream.is_open()) {
    return CommandResultCode::fileOpenFailed;
  }
  return parseFromStream(inStream);
}

CommandResultCode CommandInterpreter::loadFromString(const std::string& commands) {
  std::istringstream inStream(commands);
  return parseFromStream(inStream);
}

void CommandInterpreter::reset() {
  instructions.clear();
  instructionIndex = 0;
  noopActive = false;
  loopStack.clear();
}

std::size_t CommandInterpreter::getLastInstructionIndex() const {
  return instructionIndex;
}

void CommandInterpreter::setVariable(std::string name, std::string value) {
  variables[std::move(name)] = std::move(value);
}

bool CommandInterpreter::hasVariable(const std::string& name) const {
  return variables.find(name) != variables.end();
}

std::optional<std::string> CommandInterpreter::getVariable(const std::string& name) const {
  auto it = variables.find(name);
  if (it == variables.end()) return std::nullopt;
  return it->second;
}

void CommandInterpreter::unsetVariable(const std::string& name) {
  variables.erase(name);
}

void CommandInterpreter::clearVariables() {
  variables.clear();
}

CommandResultCode CommandInterpreter::executeCommand(const std::string& command,
                                                     const std::vector<std::string>& args) {
  auto it = commandMap.find(command);
  if (it == commandMap.end()) {
    if (unknownCommandHandler) {
      return unknownCommandHandler(command, args);
    }
    return CommandResultCode::unknownCommand;
  }

  const auto& overloads = it->second;
  for (const auto& cb : overloads) {
    CommandResultCode r = cb(args);
    if (r == CommandResultCode::invalidArguments) {
      continue;
    }
    return r;
  }

  return CommandResultCode::invalidArguments;
}

std::optional<std::string> CommandInterpreter::substituteToken(const std::string& token) const {
  if (token.empty()) return token;

  if (token[0] != '$') {
    return token;
  }

  // $name
  if (token.size() >= 2 && token[1] != '{') {
    std::string name = token.substr(1);
    auto it = variables.find(name);
    if (it == variables.end()) return std::nullopt;
    return it->second;
  }

  // ${name}
  if (token.size() >= 4 && token[1] == '{' && token.back() == '}') {
    std::string name = token.substr(2, token.size() - 3);
    auto it = variables.find(name);
    if (it == variables.end()) return std::nullopt;
    return it->second;
  }

  return std::nullopt;
}

CommandResultCode CommandInterpreter::substituteVariablesInPlace(std::vector<std::string>& args) const {
  for (auto& a : args) {
    auto replaced = substituteToken(a);
    if (!replaced.has_value()) {
      return CommandResultCode::invalidArguments;
    }
    a = std::move(*replaced);
  }
  return CommandResultCode::success;
}

std::string CommandInterpreter::trim(const std::string& text) {
  std::size_t start = 0;
  while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start]))) {
    ++start;
  }

  std::size_t end = text.size();
  while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1]))) {
    --end;
  }

  return text.substr(start, end - start);
}

std::vector<std::string> CommandInterpreter::tokenize(const std::string& line) {
  std::vector<std::string> tokens;
  std::istringstream stream(line);
  std::string token;
  while (stream >> token) {
    tokens.push_back(token);
  }
  return tokens;
}

std::optional<long long> CommandInterpreter::parseLoopCount(const std::string& token) {
  return parseIntStrict(token);
}

CommandResultCode CommandInterpreter::evalIntegerExpression(const std::vector<std::string>& exprTokens,
                                                            long long& outValue) const {
  if (exprTokens.empty()) return CommandResultCode::invalidArguments;

  std::vector<std::string> output;
  std::vector<std::string> ops;

  auto flushOps = [&](int minPrec) {
    while (!ops.empty() && precedence(ops.back()) >= minPrec) {
      output.push_back(ops.back());
      ops.pop_back();
    }
  };

  for (const auto& t : exprTokens) {
    if (isOpToken(t)) {
      int p = precedence(t);
      flushOps(p);
      ops.push_back(t);
    } else {
      if (!parseIntStrict(t).has_value()) {
        return CommandResultCode::invalidArguments;
      }
      output.push_back(t);
    }
  }

  while (!ops.empty()) {
    output.push_back(ops.back());
    ops.pop_back();
  }

  std::vector<long long> stack;
  for (const auto& t : output) {
    if (!isOpToken(t)) {
      auto v = parseIntStrict(t);
      if (!v.has_value()) return CommandResultCode::invalidArguments;
      stack.push_back(*v);
      continue;
    }

    if (stack.size() < 2) return CommandResultCode::invalidArguments;
    long long b = stack.back(); stack.pop_back();
    long long a = stack.back(); stack.pop_back();

    auto r = applyOp(a, b, t);
    if (!r.has_value()) return CommandResultCode::invalidArguments;
    stack.push_back(*r);
  }

  if (stack.size() != 1) return CommandResultCode::invalidArguments;
  outValue = stack.back();
  return CommandResultCode::success;
}

CommandResultCode CommandInterpreter::buildRepeatPairing() {
  std::vector<std::size_t> stack;

  for (std::size_t i = 0; i < instructions.size(); ++i) {
    if (instructions[i].kind == InstructionKind::repeatStart) {
      stack.push_back(i);
    } else if (instructions[i].kind == InstructionKind::repeatEnd) {
      if (stack.empty()) {
        return CommandResultCode::unmatchedEndrepeat;
      }

      std::size_t startIdx = stack.back();
      stack.pop_back();

      instructions[startIdx].matchIndex = i;
      instructions[i].matchIndex = startIdx;
    }
  }

  if (!stack.empty()) {
    return CommandResultCode::unmatchedRepeat;
  }

  return CommandResultCode::success;
}

CommandResultCode CommandInterpreter::skipToAfterMatchingEndRepeat(std::size_t repeatStartIndex) {
  if (repeatStartIndex >= instructions.size()) return CommandResultCode::invalidArguments;

  auto& inst = instructions[repeatStartIndex];
  if (inst.kind != InstructionKind::repeatStart) return CommandResultCode::invalidArguments;
  if (inst.matchIndex == static_cast<std::size_t>(-1)) return CommandResultCode::invalidArguments;

  instructionIndex = inst.matchIndex + 1;
  return CommandResultCode::success;
}

CommandResultCode CommandInterpreter::buildIfPairing() {
  std::vector<std::size_t> ifStack;

  for (std::size_t i = 0; i < instructions.size(); ++i) {
    auto& inst = instructions[i];

    if (inst.kind == InstructionKind::ifStart) {
      ifStack.push_back(i);
      continue;
    }

    if (inst.kind == InstructionKind::ifElse) {
      if (ifStack.empty()) {
        return CommandResultCode::unmatchedElse;
      }
      auto& ifInst = instructions[ifStack.back()];
      if (ifInst.elseIndex != static_cast<std::size_t>(-1)) {
        return CommandResultCode::invalidArguments; // multiple else
      }
      ifInst.elseIndex = i;
      continue;
    }

    if (inst.kind == InstructionKind::ifEnd) {
      if (ifStack.empty()) {
        return CommandResultCode::unmatchedEndif;
      }

      std::size_t ifIdx = ifStack.back();
      ifStack.pop_back();

      auto& ifInst = instructions[ifIdx];
      auto& endInst = instructions[i];

      ifInst.matchIndex = i;
      endInst.matchIndex = ifIdx;

      if (ifInst.elseIndex != static_cast<std::size_t>(-1)) {
        auto& elseInst = instructions[ifInst.elseIndex];
        elseInst.matchIndex = i; // else -> endif
      }
    }
  }

  if (!ifStack.empty()) {
    return CommandResultCode::unmatchedIf;
  }

  return CommandResultCode::success;
}

CommandResultCode CommandInterpreter::skipToAfterMatchingEndif(std::size_t ifStartIndex) {
  if (ifStartIndex >= instructions.size()) return CommandResultCode::invalidArguments;

  auto& inst = instructions[ifStartIndex];
  if (inst.kind != InstructionKind::ifStart) return CommandResultCode::invalidArguments;
  if (inst.matchIndex == static_cast<std::size_t>(-1)) return CommandResultCode::invalidArguments;

  instructionIndex = inst.matchIndex + 1;
  return CommandResultCode::success;
}

CommandResultCode CommandInterpreter::skipToAfterIfFalse(std::size_t ifStartIndex) {
  if (ifStartIndex >= instructions.size()) return CommandResultCode::invalidArguments;

  auto& inst = instructions[ifStartIndex];
  if (inst.kind != InstructionKind::ifStart) return CommandResultCode::invalidArguments;

  if (inst.elseIndex != static_cast<std::size_t>(-1)) {
    instructionIndex = inst.elseIndex + 1;
    return CommandResultCode::success;
  }

  return skipToAfterMatchingEndif(ifStartIndex);
}

CommandResultCode CommandInterpreter::evalIfCondition(const Instruction& ifInst, bool& outValue) const {
  if (ifInst.kind != InstructionKind::ifStart) return CommandResultCode::invalidArguments;

  // tokens: ["if", ...]
  if (ifInst.tokens.size() < 2) return CommandResultCode::invalidArguments;

  std::vector<std::string> cond(ifInst.tokens.begin() + 1, ifInst.tokens.end());
  CommandResultCode subst = substituteVariablesInPlace(cond);
  if (subst != CommandResultCode::success) return subst;

  // Form A: if <value>
  if (cond.size() == 1) {
    const std::string& v = cond[0];

    if (auto i = parseIntStrict(v); i.has_value()) {
      outValue = (*i != 0);
      return CommandResultCode::success;
    }

    if (auto b = commandInterpreterDetail::Parse<bool>::from(std::string_view{v}); b.has_value()) {
      outValue = *b;
      return CommandResultCode::success;
    }

    return CommandResultCode::invalidArguments;
  }

  // Form B: if <lhs> <op> <rhs>
  if (cond.size() == 3) {
    const std::string& lhs = cond[0];
    const std::string& op  = cond[1];
    const std::string& rhs = cond[2];

    if (!isIfOp(op)) return CommandResultCode::invalidArguments;

    auto li = parseIntStrict(lhs);
    auto ri = parseIntStrict(rhs);

    if (li.has_value() && ri.has_value()) {
      if (op == "==") outValue = (*li == *ri);
      else if (op == "!=") outValue = (*li != *ri);
      else if (op == "<") outValue = (*li < *ri);
      else if (op == "<=") outValue = (*li <= *ri);
      else if (op == ">") outValue = (*li > *ri);
      else if (op == ">=") outValue = (*li >= *ri);
      else return CommandResultCode::invalidArguments;
      return CommandResultCode::success;
    }

    // String compare only for == and !=
    if (op == "==") {
      outValue = (lhs == rhs);
      return CommandResultCode::success;
    }
    if (op == "!=") {
      outValue = (lhs != rhs);
      return CommandResultCode::success;
    }

    return CommandResultCode::invalidArguments;
  }

  return CommandResultCode::invalidArguments;
}

CommandResultCode CommandInterpreter::parseFromStream(std::istream& inStream) {
  instructions.clear();
  instructionIndex = 0;
  noopActive = false;
  loopStack.clear();

  std::string line;
  bool lastWasSeparator = false;

  while (std::getline(inStream, line)) {
    std::size_t hashPos = line.find('#');
    if (hashPos != std::string::npos) {
      line = line.substr(0, hashPos);
    }

    std::string trimmedLine = trim(line);

    if (trimmedLine.empty()) {
      if (!instructions.empty() && !lastWasSeparator && hashPos == std::string::npos) {
        Instruction sep;
        sep.kind = InstructionKind::separator;
        instructions.push_back(std::move(sep));
        lastWasSeparator = true;
      }
      continue;
    }

    std::vector<std::string> tokens = tokenize(trimmedLine);
    if (tokens.empty()) continue;

    Instruction inst;

    if (tokens[0] == "repeat") {
      inst.kind = InstructionKind::repeatStart;
      inst.tokens = std::move(tokens);
      instructions.push_back(std::move(inst));
      lastWasSeparator = false;
      continue;
    }

    if (tokens[0] == "endrepeat") {
      inst.kind = InstructionKind::repeatEnd;
      inst.tokens = std::move(tokens);
      instructions.push_back(std::move(inst));
      lastWasSeparator = false;
      continue;
    }

    if (tokens[0] == "if") {
      inst.kind = InstructionKind::ifStart;
      inst.tokens = std::move(tokens);
      instructions.push_back(std::move(inst));
      lastWasSeparator = false;
      continue;
    }

    if (tokens[0] == "else") {
      inst.kind = InstructionKind::ifElse;
      inst.tokens = std::move(tokens);
      instructions.push_back(std::move(inst));
      lastWasSeparator = false;
      continue;
    }

    if (tokens[0] == "endif") {
      inst.kind = InstructionKind::ifEnd;
      inst.tokens = std::move(tokens);
      instructions.push_back(std::move(inst));
      lastWasSeparator = false;
      continue;
    }

    inst.kind = InstructionKind::command;
    inst.tokens = std::move(tokens);
    instructions.push_back(std::move(inst));
    lastWasSeparator = false;
  }

  CommandResultCode rep = buildRepeatPairing();
  if (rep != CommandResultCode::success) return rep;

  return buildIfPairing();
}

CommandResultCode CommandInterpreter::runBatch() {
  if (noopActive) {
    auto now = std::chrono::steady_clock::now();
    if (now < noopUntil) {
      return CommandResultCode::waitingNoop;
    }
    noopActive = false;
  }

  if (instructionIndex >= instructions.size()) {
    return CommandResultCode::finished;
  }

  bool executedAny = false;

  while (instructionIndex < instructions.size()) {
    Instruction& instruction = instructions[instructionIndex];

    if (instruction.kind == InstructionKind::separator) {
      ++instructionIndex;

      if (!executedAny) {
        continue;
      }

      bool hasMoreCommands = false;
      for (std::size_t i = instructionIndex; i < instructions.size(); ++i) {
        if (instructions[i].kind != InstructionKind::separator) {
          hasMoreCommands = true;
          break;
        }
      }
      return hasMoreCommands ? CommandResultCode::success : CommandResultCode::finished;
    }

    // IF start
    if (instruction.kind == InstructionKind::ifStart) {
      bool condValue = false;
      CommandResultCode ev = evalIfCondition(instruction, condValue);
      if (ev != CommandResultCode::success) return ev;

      if (condValue) {
        ++instructionIndex; // enter then-block
      } else {
        CommandResultCode skip = skipToAfterIfFalse(instructionIndex);
        if (skip != CommandResultCode::success) return skip;
      }
      continue;
    }

    // ELSE: if we reached it, we executed the then-block, so skip to endif
    if (instruction.kind == InstructionKind::ifElse) {
      if (instruction.matchIndex == static_cast<std::size_t>(-1)) {
        return CommandResultCode::invalidArguments;
      }
      instructionIndex = instruction.matchIndex + 1;
      continue;
    }

    // ENDIF: just pass through
    if (instruction.kind == InstructionKind::ifEnd) {
      ++instructionIndex;
      continue;
    }

    // Loop start:
    //   repeat <count>
    //   repeat <count> as <counterVar>
    if (instruction.kind == InstructionKind::repeatStart) {
      const auto& t = instruction.tokens;

      bool hasCounter = false;
      std::string counterToken;

      if (t.size() == 2) {
        hasCounter = false;
      } else if (t.size() == 4 && t[2] == "as") {
        hasCounter = true;
        counterToken = t[3];
      } else {
        return CommandResultCode::invalidArguments;
      }

      std::vector<std::string> countArg{t[1]};
      CommandResultCode substCount = substituteVariablesInPlace(countArg);
      if (substCount != CommandResultCode::success) return substCount;

      auto countOpt = parseLoopCount(countArg[0]);
      if (!countOpt.has_value() || *countOpt < 0) {
        return CommandResultCode::invalidArguments;
      }
      long long count = *countOpt;

      if (count == 0) {
        CommandResultCode skip = skipToAfterMatchingEndRepeat(instructionIndex);
        if (skip != CommandResultCode::success) return skip;
        continue;
      }

      if (instruction.matchIndex == static_cast<std::size_t>(-1)) {
        return CommandResultCode::invalidArguments;
      }

      LoopFrame frame;
      frame.startIndex = instructionIndex + 1;
      frame.endIndex = instruction.matchIndex;
      frame.remaining = count;
      frame.iterationIndex = 0;

      if (hasCounter) {
        auto nameOpt = extractVarName(counterToken);
        if (!nameOpt.has_value() || nameOpt->empty()) {
          return CommandResultCode::invalidArguments;
        }
        frame.counterVarName = *nameOpt;

        auto itPrev = variables.find(frame.counterVarName);
        if (itPrev != variables.end()) {
          frame.savedCounterVar = itPrev->second;
          frame.hadSavedCounterVar = true;
        } else {
          frame.savedCounterVar.reset();
          frame.hadSavedCounterVar = false;
        }

        setVariable(frame.counterVarName, "0");
      }

      loopStack.push_back(std::move(frame));
      ++instructionIndex;
      continue;
    }

    // Loop end
    if (instruction.kind == InstructionKind::repeatEnd) {
      if (loopStack.empty()) {
        return CommandResultCode::invalidArguments;
      }

      LoopFrame& frame = loopStack.back();
      if (frame.endIndex != instructionIndex) {
        return CommandResultCode::invalidArguments;
      }

      frame.remaining -= 1;

      if (frame.remaining > 0) {
        frame.iterationIndex += 1;

        if (!frame.counterVarName.empty()) {
          setVariable(frame.counterVarName, std::to_string(frame.iterationIndex));
        }

        instructionIndex = frame.startIndex;
      } else {
        if (!frame.counterVarName.empty()) {
          if (frame.hadSavedCounterVar) {
            setVariable(frame.counterVarName, *frame.savedCounterVar);
          } else {
            unsetVariable(frame.counterVarName);
          }
        }

        loopStack.pop_back();
        ++instructionIndex;
      }
      continue;
    }

    executedAny = true;

    if (instruction.tokens.empty()) {
      return CommandResultCode::invalidArguments;
    }

    const std::string& command = instruction.tokens[0];
    std::vector<std::string> args(instruction.tokens.begin() + 1, instruction.tokens.end());

    CommandResultCode substArgs = substituteVariablesInPlace(args);
    if (substArgs != CommandResultCode::success) {
      return substArgs;
    }

    // Built-in: noop <ms>
    if (command == "noop") {
      if (args.size() != 1) {
        return CommandResultCode::invalidArguments;
      }
      try {
        long long milliseconds = std::stoll(args[0]);
        if (milliseconds <= 0) {
          ++instructionIndex;
          continue;
        }
        auto now = std::chrono::steady_clock::now();
        noopUntil = now + std::chrono::milliseconds(milliseconds);
        noopActive = true;
      } catch (...) {
        return CommandResultCode::invalidArguments;
      }

      ++instructionIndex;
      return CommandResultCode::waitingNoop;
    }

    // Built-in: set <name> <value...>
    if (command == "set") {
      if (args.size() < 2) return CommandResultCode::invalidArguments;

      const std::string name = args[0];
      std::vector<std::string> rhs(args.begin() + 1, args.end());

      if (rhs.size() == 1) {
        setVariable(name, rhs[0]);
      } else {
        long long value = 0;
        CommandResultCode ev = evalIntegerExpression(rhs, value);
        if (ev != CommandResultCode::success) return ev;
        setVariable(name, std::to_string(value));
      }

      ++instructionIndex;
      continue;
    }

    // Built-in: unset <name>
    if (command == "unset") {
      if (args.size() != 1) return CommandResultCode::invalidArguments;
      unsetVariable(args[0]);
      ++instructionIndex;
      continue;
    }

    CommandResultCode result = executeCommand(command, args);
    if (result != CommandResultCode::success) {
      return result;
    }

    ++instructionIndex;
  }

  return CommandResultCode::finished;
}
