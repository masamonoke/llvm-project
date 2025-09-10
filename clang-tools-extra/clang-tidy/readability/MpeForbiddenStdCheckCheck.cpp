
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MpeForbiddenStdCheckCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::readability {

MpeForbiddenStdCheckCheck::MpeForbiddenStdCheckCheck(StringRef Name,
                                                     ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context),
      // TODO: get list from config options
      Allowed({"numeric_limits", "sqrt", "min", "max", "sort", "unique_ptr",
               "make_unique", "fill", "atomic" }) {}

void MpeForbiddenStdCheckCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(callExpr(
  unless(isExpansionInSystemHeader()),
        callee(
          functionDecl(
            hasDeclContext(
              namespaceDecl(
                hasName("std")))))
        ).bind("stdCallee"), this);

  Finder->addMatcher(
  varDecl(hasType(qualType(hasDeclaration(namedDecl(isInStdNamespace()))))
        ).bind("stdVarDecl"), this);
}

void MpeForbiddenStdCheckCheck::check(const MatchFinder::MatchResult &Result) {
  checkCalls(Result);
  checkVarDecls(Result);
}

void MpeForbiddenStdCheckCheck::checkCalls(const MatchFinder::MatchResult &Result) {
  const auto *MatchedCallExpr = Result.Nodes.getNodeAs<CallExpr>("stdCallee");

  if (not MatchedCallExpr) {
    return;
  }

  const std::string CalleeTypeName = MatchedCallExpr->getDirectCallee()->getNameAsString();
  if (Allowed.find(CalleeTypeName) != Allowed.end()) {
    return;
  }

  diag(MatchedCallExpr->getExprLoc(), "forbidden call of std namespace item")
      << MatchedCallExpr << FixItHint::CreateRemoval(MatchedCallExpr->getSourceRange());
}

void MpeForbiddenStdCheckCheck::checkVarDecls(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<VarDecl>("stdVarDecl");
  if (not MatchedDecl or not MatchedDecl->getIdentifier() or MatchedDecl->getName().empty()) {
    return;
  }

  const std::string VarTypeNameSimple = MatchedDecl->getType().getUnqualifiedType().getBaseTypeIdentifier()->getName().str();
  if (Allowed.find(VarTypeNameSimple) != Allowed.end()) {
    return;
  }

  const std::string VarTypeName = MatchedDecl->getType().getUnqualifiedType().getAsString();
  diag(MatchedDecl->getLocation(), "forbidden usage of std namespace type: %1")
      << MatchedDecl << VarTypeName << FixItHint::CreateRemoval(MatchedDecl->getSourceRange());
}

} // namespace clang::tidy::readability
