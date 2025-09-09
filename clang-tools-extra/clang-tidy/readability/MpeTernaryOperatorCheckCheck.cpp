
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MpeTernaryOperatorCheckCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::readability {

static constexpr StringRef BindName = "ternaryOperator";

void MpeTernaryOperatorCheckCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(conditionalOperator().bind(BindName), this);
}

void MpeTernaryOperatorCheckCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedTernaryOp =
      Result.Nodes.getNodeAs<ConditionalOperator>(BindName);
  if (not MatchedTernaryOp) {
    return;
  }

  diag(MatchedTernaryOp->getBeginLoc(), "ternary operator used")
      << FixItHint::CreateRemoval(MatchedTernaryOp->getSourceRange());
}

} // namespace clang::tidy::readability
