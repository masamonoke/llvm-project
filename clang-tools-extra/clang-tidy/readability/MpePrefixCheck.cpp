
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MpePrefixCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::readability {

MpePrefixCheck::MpePrefixCheck(StringRef Name, ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context), SmartPointerNames({
        { "std::unique_ptr", "uniquePointer" },
        { "std::shared_ptr", "sharedPointer" },
        { "std::weak_ptr", "weakPointer" }
    }) {}

void MpePrefixCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      varDecl(
        unless(isExpansionInSystemHeader()), hasType(pointerType())
      )
      .bind("pointerVar"),this);

  Finder->addMatcher(
      fieldDecl(
        unless(isExpansionInSystemHeader()), hasType(pointerType())
      ).bind("fieldPointer"), this
  );

  for (const auto& [PointerType, BindName] : SmartPointerNames) {
    matchSmartPointer(Finder, PointerType, BindName);
  }
}

void MpePrefixCheck::matchSmartPointer(MatchFinder *Finder,
                                       StringRef PointerTypeName,
                                       StringRef BindName) {
  std::string PointerVar = (BindName + "Var").str();
  std::string PointerMemberDecl = (BindName + "Field").str();

  Finder->addMatcher(
      varDecl(
        hasType(
          hasUnqualifiedDesugaredType(
            recordType(
              hasDeclaration(
                namedDecl(
                  hasName(PointerTypeName))))))
        ).bind(PointerVar),this);

  Finder->addMatcher(
      fieldDecl(
        hasType(
          hasUnqualifiedDesugaredType(
            recordType(
              hasDeclaration(
                namedDecl(
                  hasName(PointerTypeName))))))
        ).bind(PointerMemberDecl), this);
}

void MpePrefixCheck::check(const MatchFinder::MatchResult &Result) {
  pointerVarCheck(Result);
  memberPointerDeclCheck(Result);
}

static bool isVarPrefixCorrect(StringRef Name) {
  return Name.starts_with("p")
             ? Name.size() > 1 && std::isupper(*(Name.begin() + 1))
             : false;
}

static bool isFieldPrefixCorrect(StringRef Name) {
  return Name.starts_with("m_p")
             ? Name.size() > 3 && std::isupper(*(Name.begin() + 3))
             : false;
}

static std::string varFix(StringRef Name) {
  return std::isupper(*(Name.begin()))
             ? "p"
             : std::string("p") + static_cast<char>(std::toupper(*(Name.begin())));
}

static std::string fieldFix(StringRef Name) {
  return std::isupper(*(Name.begin() + 2))
             ? "p"
             : std::string("m_p") + static_cast<char>(std::toupper(*(Name.begin() + 2)));
}

void MpePrefixCheck::plainPointerCheck(const MatchFinder::MatchResult &Result) {
  const auto *Var = Result.Nodes.getNodeAs<VarDecl>("pointerVar");
  if (!Var || !Var->getIdentifier() || Var->getName().empty()) {
    return;
  }

  StringRef Name = Var->getName();
  // TODO: handle cases when variable starts from p but its a word like ptr but should be pPtr
  if (not isVarPrefixCorrect(Name)) {
    const std::string Fix = varFix(Name);
    diag(Var->getLocation(), "pointer variable %0 should be prefixed with 'p'")
        << Var << FixItHint::CreateInsertion(Var->getLocation(), Fix);
  }
}

void MpePrefixCheck::memberPointerCheck(const MatchFinder::MatchResult &Result) {
  const auto *Var = Result.Nodes.getNodeAs<FieldDecl>("fieldPointer");
  if (!Var || !Var->getIdentifier() || Var->getName().empty()) {
    return;
  }

  StringRef Name = Var->getName();
  if (not isFieldPrefixCorrect(Name)) {
    const std::string Fix = fieldFix(Name);
    diag(Var->getLocation(),
         "member pointer variable %0 should be prefixed with 'm_p'")
        << Var << FixItHint::CreateInsertion(Var->getLocation(), Fix);
  }
}

void MpePrefixCheck::stdSmartPointerCheck(const MatchFinder::MatchResult &Result, StringRef BindName) {
  const std::string PointerBindName = (BindName + "Var").str();
  const auto *PtrVarDecl =
      Result.Nodes.getNodeAs<VarDecl>(PointerBindName);
  if (!PtrVarDecl || !PtrVarDecl->getIdentifier() ||
      PtrVarDecl->getName().empty()) {
    return;
  }

  StringRef VarDeclName = PtrVarDecl->getName();
  if (not isVarPrefixCorrect(VarDeclName)) {
    const std::string Fix = varFix(VarDeclName);
    diag(PtrVarDecl->getLocation(),
         "smart pointer type variable %0 should be prefixed with 'p'")
        << PtrVarDecl
        << FixItHint::CreateInsertion(PtrVarDecl->getLocation(), Fix);
  }
}

void MpePrefixCheck::stdSmartPointerFieldCheck(const ast_matchers::MatchFinder::MatchResult &Result, StringRef BindName) {
  const std::string PointerBindName = (BindName + "Field").str();
  const auto *PtrFieldDecl =
      Result.Nodes.getNodeAs<FieldDecl>(PointerBindName);
  if (!PtrFieldDecl || !PtrFieldDecl->getIdentifier() ||
      PtrFieldDecl->getName().empty()) {
    return;
  }

  StringRef FieldDeclName = PtrFieldDecl->getName();
  if (not isFieldPrefixCorrect(FieldDeclName)) {
    diag(PtrFieldDecl->getLocation(),
         "member smart pointer variable %0 should be prefixed with 'm_p'")
        << PtrFieldDecl
        << FixItHint::CreateInsertion(PtrFieldDecl->getLocation(), "m_p");
  }
}

void MpePrefixCheck::pointerVarCheck(const ast_matchers::MatchFinder::MatchResult &Result) {
  plainPointerCheck(Result);
  for (const auto& [_, BindName] : SmartPointerNames) {
    stdSmartPointerCheck(Result, BindName);
  }
}

void MpePrefixCheck::memberPointerDeclCheck(const ast_matchers::MatchFinder::MatchResult &Result) {
  memberPointerCheck(Result);
  for (const auto& [_, BindName] : SmartPointerNames) {
    stdSmartPointerFieldCheck(Result, BindName);
  }
}

} // namespace clang::tidy::readability
