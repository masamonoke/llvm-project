
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_READABILITY_MPEPREFIXCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_READABILITY_MPEPREFIXCHECK_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::readability {

/// Finds non-prefixed pointer variables and member fields
///
class MpePrefixCheck : public ClangTidyCheck {
public:
  MpePrefixCheck(StringRef Name, ClangTidyContext *Context);

  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
  bool isLanguageVersionSupported(const LangOptions &LangOpts) const override {
    return LangOpts.CPlusPlus;
  }

private:
  // TODO: what a type to consider as pointer should be in options passed through config
  llvm::SmallVector<std::pair<std::string, std::string>, 6> SmartPointerNames;

  void plainPointerCheck(const ast_matchers::MatchFinder::MatchResult &Result);
  void memberPointerCheck(const ast_matchers::MatchFinder::MatchResult &Result);
  void stdSmartPointerCheck(const ast_matchers::MatchFinder::MatchResult &Result,
                            StringRef BindName);
  void stdSmartPointerFieldCheck(const ast_matchers::MatchFinder::MatchResult &Result,
                                 StringRef BindName);
  void matchSmartPointer(ast_matchers::MatchFinder *Finder,
                         StringRef PointerTypeName, StringRef BindName);

  void pointerVarCheck(const ast_matchers::MatchFinder::MatchResult &Result);
  void memberPointerDeclCheck(const ast_matchers::MatchFinder::MatchResult &Result);
  void memberNonPointerDeclCheck(const ast_matchers::MatchFinder::MatchResult &Result);
};

} // namespace clang::tidy::readability

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_READABILITY_MPEPREFIXCHECK_H
