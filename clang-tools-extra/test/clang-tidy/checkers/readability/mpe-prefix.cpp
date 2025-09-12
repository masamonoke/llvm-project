#include <memory>
// RUN: %check_clang_tidy %s readability-mpe-prefix %t

// Checker should warn about raw pointers and about pointer-wrapper classes like
// unique_ptr, shared_ptr, weak_ptr

// CHECK-MESSAGES: :[[@LINE+1]]:8: warning: pointer variable 'Var' should be prefixed with 'p'
static void warn1() {
	void* Var = nullptr;
}

// CHECK-MESSAGES: :[[@LINE+1]]:13: warning: member pointer variable 'm_bar' should be prefixed with 'm_p'
static void warn2() {
	class Foo {
		void* m_bar;
	};
}

// CHECK-MESSAGES: :[[@LINE+1]]:20: warning: smart pointer type variable 'Var' should be prefixed with 'p'
static void warn3() {
	std::unique_ptr<int> Var = nullptr;
}

// CHECK-MESSAGES: :[[@LINE+1]]:26: member smart pointer variable 'm_Var' should be prefixed with 'm_p'
static void warn4() {
	class Foo {
		std::unique_ptr<int> m_Var;
	};
}

// CHECK-MESSAGES: :[[@LINE+1]]:32: smart pointer type variable 'Var' should be prefixed with 'p'
static void warn5() {
	std::shared_ptr<int> Var = nullptr;
}

// CHECK-MESSAGES: :[[@LINE+1]]:38: member smart pointer variable 'm_Var' should be prefixed with 'm_p'
static void warn6() {
	class Foo {
		std::shared_ptr<int> m_Var;
	};
}

// CHECK-MESSAGES: :[[@LINE+1]]:32: smart pointer type variable 'Var' should be prefixed with 'p'
static void warn7() {
	std::weak_ptr<int> Var;
}

// CHECK-MESSAGES: :[[@LINE+1]]:38: member smart pointer variable 'm_Var' should be prefixed with 'm_p'
static void warn8() {
	class Foo {
		std::weak_ptr<int> m_Var;
	};
}

// CHECK-MESSAGES: :[[@LINE+1]]:56: warning: pointer variable 'pointer' should be prefixed with 'p'
static void warn9() {
	void* pointer = nullptr;
}

// CHECK-MESSAGES: :[[@LINE+1]]:62: warning: member pointer variable 'm_pointer' should be prefixed with 'm_p'
static void warn10() {
	class Foo {
		void* m_pointer;
	};
}

// CHECK-MESSAGES: :[[@LINE+1]]:68: warning: pointer variable 'p' should be prefixed with 'p'
static void warn11() {
	void* p;
}

// CHECK-MESSAGES: :[[@LINE+1]]:74: warning: member pointer variable 'm_p' should be prefixed with 'm_p'
static void warn12() {
	class Foo {
		void* m_p;
	};
}

// CHECK-MESSAGES: :[[@LINE+1]]:81: warning: member field 'Var' should be prefixed with 'm_'
static void warn13() {
	class Foo {
		int Var;
	};
}

static void noWarn1() {
	void* pPointer = nullptr;
}

static void noWarn2() {
	class Foo {
		void* m_pPointer;
	};
}

static void noWarn3() {
	void* pVar = nullptr;
}
