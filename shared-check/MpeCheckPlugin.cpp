#include "../clang-tools-extra/clang-tidy/readability/MpePrefixCheck.h"
#include "../clang-tools-extra/clang-tidy/readability/MpeTernaryOperatorCheck.h"

#include <clang-tidy/ClangTidyModule.h>
#include <clang-tidy/ClangTidyModuleRegistry.h>

class MpeModule : public clang::tidy::ClangTidyModule {
public:
  void addCheckFactories(clang::tidy::ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<clang::tidy::readability::MpePrefixCheck>(
        "readability-mpe-prefix");
    CheckFactories
        .registerCheck<clang::tidy::readability::MpeTernaryOperatorCheck>(
            "readability-mpe-ternary-operator");
  }
};

static clang::tidy::ClangTidyModuleRegistry::Add<MpeModule> X("mpe-plugin-module", "Adds my custom checks");
