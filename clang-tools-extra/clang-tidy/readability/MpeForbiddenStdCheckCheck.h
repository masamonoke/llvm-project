
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_READABILITY_MPEFORBIDDENSTDCHECKCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_READABILITY_MPEFORBIDDENSTDCHECKCHECK_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::readability {

/// Finds std namespace functions calls and type usages
/// and checks if it is forbidden
class MpeForbiddenStdCheckCheck : public ClangTidyCheck {
public:
  MpeForbiddenStdCheckCheck(StringRef Name, ClangTidyContext *Context);

  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
  bool isLanguageVersionSupported(const LangOptions &LangOpts) const override {
    return LangOpts.CPlusPlus;
  }

private:
  std::set<std::string> Allowed;

  void checkCalls(const ast_matchers::MatchFinder::MatchResult &Result);
  void checkVarDecls(const ast_matchers::MatchFinder::MatchResult &Result);
};

} // namespace clang::tidy::readability

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_READABILITY_MPEFORBIDDENSTDCHECKCHECK_H
