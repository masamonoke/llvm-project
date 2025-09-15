SRC = \
	  ../clang-tools-extra/clang-tidy/readability/MpePrefixCheck.cpp \
	  ../clang-tools-extra/clang-tidy/readability/MpeTernaryOperatorCheck.cpp

LLVM_LIB=/home/linuxbrew/.linuxbrew/Cellar/llvm/20.1.8/lib
LLVM_INCLUDE=/home/linuxbrew/.linuxbrew/Cellar/llvm/20.1.8/include

NAME=MpeCheckPlugin.so

run:
	clang++ -std=c++17 -fPIC -shared MpeCheckPlugin.cpp $(SRC) -I$(LLVM_INCLUDE) -L$(LLVM_LIB) -lclangTidy -lclangASTMatchers -o $(NAME)

clean:
	rm *.so
