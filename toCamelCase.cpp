// bin/camel-casing test.cpp -- -std=c++14

#include <iostream>

#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/RefactoringCallbacks.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Refactoring.h"
#include "llvm/Support/CommandLine.h"

using namespace clang;

namespace detail {
void camelCase(std::string& input)
{
    bool isMakeUpper = false;
    std::string::iterator pos = input.begin();
    for (char c : input)
    {
        if(c == '_')
        {
            isMakeUpper = true;
        }
        else if(isMakeUpper)
        {
            *pos++ = (char)toupper(c);
            isMakeUpper = false;
        }
        else
        {
            *pos++ = c;
        }
    }
    input.resize(pos - input.begin());
}
}

class StmtMatchHandler : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
  using MatchResult = clang::ast_matchers::MatchFinder::MatchResult;
    using Replacements = clang::tooling::Replacements;
    using Replacement = clang::tooling::Replacement;

  StmtMatchHandler(Replacements& Replace) 
      : Replace(Replace) {}

  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) 
  {
      // Todo: figure out whether we can just have one matcher that matches both
      // expressions and declarations, by using some abstract base interface. Now
      // specific handlers are defined.

      const auto* Function = Result.Nodes.getNodeAs<clang::DeclRefExpr>("expr");
      const auto* Function2 = Result.Nodes.getNodeAs<clang::MemberExpr>("expr");

      if (Function)
      {
        Function->dump();

        auto range = CharSourceRange::getTokenRange(
          SourceRange(Function->getNameInfo().getSourceRange()));

        auto functionName = Function->getNameInfo().getAsString();
        detail::camelCase(functionName);

        Replacement Rep(
          *Result.SourceManager,
          range,
          functionName);

        auto result = Replace.add(Rep);

        if (result) {}
      }

      if (Function2)
      {
        Function2->dump();

        auto range = CharSourceRange::getTokenRange(
          SourceRange(Function2->getMemberNameInfo().getSourceRange()));

        auto functionName = Function2->getMemberNameInfo().getAsString();
        detail::camelCase(functionName);

        Replacement Rep(
          *Result.SourceManager,
          range,
          functionName);

        auto result = Replace.add(Rep);

        if (result) {} 
      }
  }

private:
  Replacements& Replace;
};

class DeclMatchHandler : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
	using MatchResult = clang::ast_matchers::MatchFinder::MatchResult;
    using Replacements = clang::tooling::Replacements;
    using Replacement = clang::tooling::Replacement;

  DeclMatchHandler(Replacements& Replace) 
      : Replace(Replace) {}

  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) 
  {
      const auto* Function = Result.Nodes.getNodeAs<clang::FunctionDecl>("decl");
      Function->dump();

      auto range = CharSourceRange::getTokenRange(
        SourceRange(Function->getNameInfo().getSourceRange()));

      auto functionName = Function->getNameInfo().getAsString();
      detail::camelCase(functionName);

      Replacement Rep(
        *Result.SourceManager,
        range,
        functionName);

      auto result = Replace.add(Rep);

      if (result) {}
  }

private:
	Replacements& Replace;
};

class ToCamelCaseConsumer : public clang::ASTConsumer {
public:
  using Replacements = clang::tooling::Replacements;

  ToCamelCaseConsumer(
  	ASTContext* Context,
  	Replacements& Replace)
   : DeclHandler(Replace),
     StmtHandler(Replace)
  {
     using namespace clang::ast_matchers;

     const auto DeclMatcher = functionDecl(unless(anyOf(cxxConstructorDecl(), cxxDestructorDecl()))).bind("decl");
     const auto StmtMatcher = expr(anyOf(declRefExpr(), memberExpr())).bind("expr");

     Finder.addMatcher(DeclMatcher, &DeclHandler);
     Finder.addMatcher(StmtMatcher, &StmtHandler);
  }

  virtual void HandleTranslationUnit(clang::ASTContext &Context) 
  {
     Finder.matchAST(Context);
  }

private:
   DeclMatchHandler DeclHandler;
   StmtMatchHandler StmtHandler;
   clang::ast_matchers::MatchFinder Finder;
};

class ToCamelCaseAction : public clang::ASTFrontendAction {
public:
  using Replacements = clang::tooling::Replacements;

  explicit ToCamelCaseAction(Replacements& Replace)
    : Replace(Replace) 
  {}

  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
       clang::CompilerInstance &Compiler, llvm::StringRef InFile) 
  {
    return std::unique_ptr<clang::ASTConsumer>(
        new ToCamelCaseConsumer(&Compiler.getASTContext(), Replace));
  }

private:
	Replacements& Replace;
};

class ToolFactory : public clang::tooling::FrontendActionFactory
{
public:
   using Replacements = clang::tooling::Replacements;

   ToolFactory(Replacements& Replace)
     : Replace(Replace)
   {}

   clang::FrontendAction* create() override
   {
      return new ToCamelCaseAction(Replace);
   }

private:
   Replacements& Replace;
};

static llvm::cl::OptionCategory MyToolCategory("my-tool options");

auto main(int argc, const char *argv[]) -> int 
{
  using namespace clang::tooling;

  CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);

  RefactoringTool Tool(
  	OptionsParser.getCompilations(),
  	OptionsParser.getSourcePathList());

  auto& replacementMap = Tool.getReplacements();
  replacementMap["test.cpp"] = Replacements();
  auto& replacements = replacementMap["test.cpp"];

  auto result = Tool.runAndSave(new ToolFactory(replacements));

  return result;
}
