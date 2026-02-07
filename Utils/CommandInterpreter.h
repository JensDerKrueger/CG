#pragma once

#include <cctype>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <functional>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

enum class CommandResultCode {
  success = 0,
  finished,
  triggerLoop,
  waitingNoop,
  fileOpenFailed,
  unknownCommand,
  invalidArguments,

  unmatchedRepeat,
  unmatchedEndrepeat,

  unmatchedIf,
  unmatchedElse,
  unmatchedEndif,

  callbackError
};

constexpr std::string_view toString(CommandResultCode v) {
  switch (v) {
    case CommandResultCode::success:            return "success";
    case CommandResultCode::finished:           return "finished";
    case CommandResultCode::triggerLoop:        return "triggerLoop";
    case CommandResultCode::waitingNoop:        return "waitingNoop";
    case CommandResultCode::fileOpenFailed:     return "fileOpenFailed";
    case CommandResultCode::unknownCommand:     return "unknownCommand";
    case CommandResultCode::invalidArguments:   return "invalidArguments";

    case CommandResultCode::unmatchedRepeat:    return "unmatchedRepeat";
    case CommandResultCode::unmatchedEndrepeat: return "unmatchedEndrepeat";

    case CommandResultCode::unmatchedIf:        return "unmatchedIf";
    case CommandResultCode::unmatchedElse:      return "unmatchedElse";
    case CommandResultCode::unmatchedEndif:     return "unmatchedEndif";

    case CommandResultCode::callbackError:      return "callbackError";
  }
  return "CommandResultCode::<unknown>";
}

inline std::ostream& operator<<(std::ostream& os, CommandResultCode v) {
  return os << toString(v);
}

namespace commandInterpreterDetail {

  template <class T>
  using CleanT = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

  template <class T>
  struct Parse;

  template <class T>
  struct ParseInteger {
    static std::optional<T> from(std::string_view s) {
      T v{};
      const char* b = s.data();
      const char* e = s.data() + s.size();
      auto result = std::from_chars(b, e, v);
      if (result.ec != std::errc{} || result.ptr != e) {
        return std::nullopt;
      }
      return v;
    }
  };

  template <>
  struct Parse<int> : ParseInteger<int> {};

  template <>
  struct Parse<long long> : ParseInteger<long long> {};

  template <>
  struct Parse<unsigned long long> : ParseInteger<unsigned long long> {};

  template <>
  struct Parse<std::uint32_t> : ParseInteger<std::uint32_t> {};

  template <>
  struct Parse<bool> {
    static std::optional<bool> from(std::string_view s) {
      if (s == "1" || s == "true" || s == "on" || s == "yes") return true;
      if (s == "0" || s == "false" || s == "off" || s == "no") return false;
      return std::nullopt;
    }
  };

  template <>
  struct Parse<float> {
    static std::optional<float> from(std::string_view s) {
      try {
        std::string tmp(s);
        std::size_t idx = 0;
        float v = std::stof(tmp, &idx);
        if (idx != tmp.size()) return std::nullopt;
        return v;
      } catch (...) {
        return std::nullopt;
      }
    }
  };

  template <>
  struct Parse<double> {
    static std::optional<double> from(std::string_view s) {
      try {
        std::string tmp(s);
        std::size_t idx = 0;
        double v = std::stod(tmp, &idx);
        if (idx != tmp.size()) return std::nullopt;
        return v;
      } catch (...) {
        return std::nullopt;
      }
    }
  };

  template <>
  struct Parse<std::string> {
    static std::optional<std::string> from(std::string_view s) {
      return std::string(s);
    }
  };

  template <class T>
  static std::optional<CleanT<T>> parseArg(const std::string& s) {
    return Parse<CleanT<T>>::from(std::string_view{s});
  }

  template <class T>
  struct CallableTraits : CallableTraits<decltype(&T::operator())> {};

  template <class C, class R, class... Args>
  struct CallableTraits<R (C::*)(Args...) const> {
    using ReturnType = R;
    using ArgsTuple = std::tuple<Args...>;
    static constexpr std::size_t arity = sizeof...(Args);
  };

  template <class R, class... Args>
  struct CallableTraits<R (*)(Args...)> {
    using ReturnType = R;
    using ArgsTuple = std::tuple<Args...>;
    static constexpr std::size_t arity = sizeof...(Args);
  };

  template <class R, class... Args>
  struct CallableTraits<std::function<R(Args...)>> {
    using ReturnType = R;
    using ArgsTuple = std::tuple<Args...>;
    static constexpr std::size_t arity = sizeof...(Args);
  };

  template <class T>
  struct IsRestArgs : std::false_type {};

  template <>
  struct IsRestArgs<std::vector<std::string>> : std::true_type {};

  template <class Fn, std::size_t... I>
  static CommandResultCode invokeParsedFixedImpl(
                                                 Fn&& fn,
                                                 const std::vector<std::string>& args,
                                                 std::index_sequence<I...>) {
    using Tuple = typename CallableTraits<typename std::decay<Fn>::type>::ArgsTuple;

    auto parsed = std::tuple<std::optional<CleanT<typename std::tuple_element<I, Tuple>::type>>...>{
      parseArg<typename std::tuple_element<I, Tuple>::type>(args[I])...};

    bool ok = (std::get<I>(parsed).has_value() && ...);
    if (!ok) return CommandResultCode::invalidArguments;

    using Traits = CallableTraits<typename std::decay<Fn>::type>;
    using R = typename Traits::ReturnType;

    if constexpr (std::is_same<R, CommandResultCode>::value) {
      return std::invoke(std::forward<Fn>(fn), (*std::get<I>(parsed))...);
    } else {
      std::invoke(std::forward<Fn>(fn), (*std::get<I>(parsed))...);
      return CommandResultCode::success;
    }
  }

  template <class Fn, std::size_t... I>
  static CommandResultCode invokeParsedRestImpl(
                                                Fn&& fn,
                                                const std::vector<std::string>& args,
                                                std::size_t fixedCount,
                                                std::index_sequence<I...>) {
    using Tuple = typename CallableTraits<typename std::decay<Fn>::type>::ArgsTuple;

    auto parsed = std::tuple<std::optional<CleanT<typename std::tuple_element<I, Tuple>::type>>...>{
      parseArg<typename std::tuple_element<I, Tuple>::type>(args[I])...};

    bool ok = (std::get<I>(parsed).has_value() && ...);
    if (!ok) return CommandResultCode::invalidArguments;

    std::vector<std::string> rest;
    rest.reserve(args.size() - fixedCount);
    for (std::size_t i = fixedCount; i < args.size(); ++i) {
      rest.push_back(args[i]);
    }

    using Traits = CallableTraits<typename std::decay<Fn>::type>;
    using R = typename Traits::ReturnType;

    if constexpr (std::is_same<R, CommandResultCode>::value) {
      return std::invoke(std::forward<Fn>(fn), (*std::get<I>(parsed))..., std::move(rest));
    } else {
      std::invoke(std::forward<Fn>(fn), (*std::get<I>(parsed))..., std::move(rest));
      return CommandResultCode::success;
    }
  }

  template <class CallbackT, class Fn>
  static CallbackT makeCallback(Fn&& fn) {
    using Traits = CallableTraits<typename std::decay<Fn>::type>;
    using Tuple = typename Traits::ArgsTuple;
    constexpr std::size_t n = Traits::arity;

    if constexpr (n > 0) {
      using LastT = CleanT<typename std::tuple_element<n - 1, Tuple>::type>;
      constexpr bool hasRest = IsRestArgs<LastT>::value;

      if constexpr (hasRest) {
        constexpr std::size_t fixedCount = n - 1;
        return [fn = std::forward<Fn>(fn)](const std::vector<std::string>& args) mutable {
          if (args.size() < fixedCount) return CommandResultCode::invalidArguments;
          return invokeParsedRestImpl(fn, args, fixedCount, std::make_index_sequence<fixedCount>{});
        };
      } else {
        return [fn = std::forward<Fn>(fn)](const std::vector<std::string>& args) mutable {
          if (args.size() != n) return CommandResultCode::invalidArguments;
          return invokeParsedFixedImpl(fn, args, std::make_index_sequence<n>{});
        };
      }
    } else {
      return [fn = std::forward<Fn>(fn)](const std::vector<std::string>& args) mutable {
        if (!args.empty()) return CommandResultCode::invalidArguments;

        using Traits2 = CallableTraits<typename std::decay<Fn>::type>;
        using R = typename Traits2::ReturnType;

        if constexpr (std::is_same<R, CommandResultCode>::value) {
          return std::invoke(fn);
        } else {
          std::invoke(fn);
          return CommandResultCode::success;
        }
      };
    }
  }

} // namespace commandInterpreterDetail

class CommandInterpreter {
public:
  using Callback = std::function<CommandResultCode(const std::vector<std::string>&)>;
  using UnknownCommandCallback =
  std::function<CommandResultCode(const std::string&, const std::vector<std::string>&)>;

  CommandInterpreter();

  CommandResultCode registerCommand(const std::string& commandName, Callback callbackFunction);

  template <class Fn,
  typename std::enable_if<
  !std::is_same<typename std::decay<Fn>::type, Callback>::value &&
  !std::is_invocable<Fn, const std::vector<std::string>&>::value,
  int>::type = 0>
  CommandResultCode registerCommand(const std::string& commandName, Fn&& fn) {
    return registerCommand(
                           commandName,
                           commandInterpreterDetail::makeCallback<Callback>(std::forward<Fn>(fn)));
  }

  void setUnknownCommandHandler(UnknownCommandCallback callbackFunction);

  CommandResultCode loadFromFile(const std::string& filePath);
  CommandResultCode loadFromString(const std::string& commands);

  CommandResultCode runBatch();

  void reset();
  std::size_t getLastInstructionIndex() const;

  void setVariable(std::string name, std::string value);
  bool hasVariable(const std::string& name) const;
  std::optional<std::string> getVariable(const std::string& name) const;
  void unsetVariable(const std::string& name);
  void clearVariables();

private:
  enum class InstructionKind {
    separator,
    command,

    repeatStart,
    repeatEnd,

    ifStart,
    ifElse,
    ifEnd
  };

  struct Instruction {
    InstructionKind kind = InstructionKind::command;
    std::vector<std::string> tokens;

    // For repeatStart/repeatEnd: start.matchIndex=end, end.matchIndex=start
    // For ifStart/ifEnd: if.matchIndex=endif, endif.matchIndex=if
    // For ifElse: else.matchIndex=endif
    std::size_t matchIndex = static_cast<std::size_t>(-1);

    // For ifStart only: elseIndex points to the else instruction if present.
    std::size_t elseIndex  = static_cast<std::size_t>(-1);
  };

  struct LoopFrame {
    std::size_t startIndex = 0;
    std::size_t endIndex = 0;

    long long remaining = 0;      // how many iterations still to execute
    long long iterationIndex = 0; // 0..total-1

    std::string counterVarName;                 // without '$', empty if not used
    std::optional<std::string> savedCounterVar; // prior value if it existed
    bool hadSavedCounterVar = false;            // distinguishes "unset" vs "set to empty"
  };

  std::unordered_map<std::string, std::vector<Callback>> commandMap;
  UnknownCommandCallback unknownCommandHandler{nullptr};

  std::vector<Instruction> instructions;
  std::size_t instructionIndex{0};

  bool noopActive{false};
  std::chrono::steady_clock::time_point noopUntil{};

  std::vector<LoopFrame> loopStack;

  std::unordered_map<std::string, std::string> variables;

  CommandResultCode executeCommand(const std::string& command, const std::vector<std::string>& args);

  CommandResultCode substituteVariablesInPlace(std::vector<std::string>& args) const;
  std::optional<std::string> substituteToken(const std::string& token) const;

  static std::string trim(const std::string& text);
  static std::vector<std::string> tokenize(const std::string& line);

  CommandResultCode parseFromStream(std::istream& inStream);

  CommandResultCode buildRepeatPairing();
  CommandResultCode skipToAfterMatchingEndRepeat(std::size_t repeatStartIndex);

  CommandResultCode buildIfPairing();
  CommandResultCode skipToAfterIfFalse(std::size_t ifStartIndex);
  CommandResultCode skipToAfterMatchingEndif(std::size_t ifStartIndex);

  CommandResultCode evalIfCondition(const Instruction& ifInst, bool& outValue) const;

  static std::optional<long long> parseLoopCount(const std::string& token);

  CommandResultCode evalIntegerExpression(const std::vector<std::string>& exprTokens,
                                          long long& outValue) const;
};

/*
 Copyright (c) 2026 Computer Graphics and Visualization Group, University of
 Duisburg-Essen

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in the
 Software without restriction, including without limitation the rights to use, copy,
 modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
 to permit persons to whom the Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be included in all copies
 or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
