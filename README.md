# refactor-tool
using llvm libtooling to define source to source transformations

This tool is compiled as part of the llvm project. To obtain clang, llvm, ninja (and updated cmake version), see
https://clang.llvm.org/docs/LibASTMatchersTutorial.html

For information, check:
https://www.youtube.com/watch?v=E6i8jmiy8MY

https://www.youtube.com/watch?v=yuIOGfcOH0k

https://eli.thegreenplace.net/2014/07/29/ast-matchers-and-clang-refactoring-tools


This repo is just for demonstration purposes, so only the core functionality is stored here.

------

The camelCase tools renames methods and functions that possibly contain underscores to camelCase format.
