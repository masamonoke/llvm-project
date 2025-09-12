
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
    : ClangTidyCheck(Name, Context), SmartPointerNames(getPointerTypes()) {}

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

  Finder->addMatcher(fieldDecl(unless(hasType(pointerType()))).bind("fieldNonPointer"),this);

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
  memberNonPointerDeclCheck(Result);
}

static bool isVarPrefixCorrect(StringRef Name) {
  return Name.starts_with("p")
             ? Name.size() > 1 && std::isupper(*(Name.begin() + 1))
             : false;
}

static bool isPtrFieldPrefixCorrect(StringRef Name) {
  return Name.starts_with("m_p")
             ? Name.size() > 3 && std::isupper(*(Name.begin() + 3))
             : false;
}

static bool isFieldPrefixCorrect(StringRef Name) {
  return Name.starts_with("m_");
}

static std::string varFix(StringRef Name) {
	std::string Fix = "p";
	if (!std::isupper(Name.front())) {
		Fix += static_cast<char>(std::toupper(Name.front()));
		return Fix + std::string(Name.begin() + 1, Name.end());
	}

	return Fix + std::string(Name);
}

static std::string fieldNonPtrFix(StringRef Name) {
	std::string Fix = "m_";
	return Fix + std::string(Name);
}

static std::string fieldPtrFix(StringRef Name) {
  std::string Fix = "m_p";

  if (Name.starts_with("m_")) {
    if (!std::isupper(*(Name.begin() + 2))) {
      Fix.push_back(static_cast<char>(std::toupper(*(Name.begin() + 2))));
      return Fix + std::string(Name.begin() + 3, Name.end());
    }

    return Fix + std::string(Name.begin() + 2, Name.end());
  }

  if (!std::isupper(Name.front())) {
    Fix.push_back(static_cast<char>(std::toupper(Name.front())));
    return Fix + std::string(Name.begin() + 1, Name.end());
  }

  return Fix + std::string(Name.begin() + 2, Name.end());
}

void MpePrefixCheck::plainPointerCheck(const MatchFinder::MatchResult &Result) {
  const auto *Var = Result.Nodes.getNodeAs<VarDecl>("pointerVar");
  if (!Var || !Var->getIdentifier() || Var->getName().empty()) {
    return;
  }

  StringRef Name = Var->getName();
  if (not isVarPrefixCorrect(Name)) {
    const std::string Fix = varFix(Name);
    diag(Var->getLocation(), "pointer variable %0 should be prefixed with 'p'")
        << Var << FixItHint::CreateReplacement(Var->getLocation(), Fix);
  }
}

void MpePrefixCheck::memberPointerCheck(const MatchFinder::MatchResult &Result) {
  const auto *Var = Result.Nodes.getNodeAs<FieldDecl>("fieldPointer");
  if (!Var || !Var->getIdentifier() || Var->getName().empty()) {
    return;
  }

  StringRef Name = Var->getName();
  if (not isPtrFieldPrefixCorrect(Name)) {
    const std::string Fix = fieldPtrFix(Name);
    diag(Var->getLocation(),
         "member pointer variable %0 should be prefixed with 'm_p'")
        << Var << FixItHint::CreateReplacement(Var->getLocation(), Fix);
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
        << FixItHint::CreateReplacement(PtrVarDecl->getLocation(), Fix);
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
  if (not isPtrFieldPrefixCorrect(FieldDeclName)) {
    const std::string Fix = fieldPtrFix(FieldDeclName);
    diag(PtrFieldDecl->getLocation(),
         "member smart pointer variable %0 should be prefixed with 'm_p'")
        << PtrFieldDecl
        << FixItHint::CreateReplacement(PtrFieldDecl->getLocation(), Fix);
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

void MpePrefixCheck::memberNonPointerDeclCheck(const ast_matchers::MatchFinder::MatchResult &Result) {
  const auto* MatchedFieldDecl = Result.Nodes.getNodeAs<FieldDecl>("fieldNonPointer");

  if (!MatchedFieldDecl) {
    return;
  }

  StringRef DeclName = MatchedFieldDecl->getName();
  if (not isFieldPrefixCorrect(DeclName)) {
    const std::string Fix = fieldNonPtrFix(DeclName);
    diag(MatchedFieldDecl->getLocation(),
         "member field %0 should be prefixed with 'm_'")
        << MatchedFieldDecl
        << FixItHint::CreateReplacement(MatchedFieldDecl->getLocation(), Fix);
  }
}

SmallVector<std::pair<std::string, std::string>, 0> MpePrefixCheck::getPointerTypes() {
  const auto Opts = Options.get("PointerTypes", "std::unique_ptr,std::shared_ptr,std::weak_ptr");
  SmallVector<StringRef, 0> List;
  Opts.split(List, ",");

  SmallVector<std::pair<std::string, std::string>, 0> RetList;
  for (const auto& Item : List) {
    RetList.emplace_back(Item.str(), Item.str());
  }

  return RetList;
}

} // namespace clang::tidy::readability
