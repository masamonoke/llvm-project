// RUN: %check_clang_tidy %s readability-mpe-forbidden-std-check %t

#include <algorithm>
#include <atomic>
#include <limits>
#include <memory>
#include <vector>

static void noWarn1() {
	std::numeric_limits<int>::max();
}

static void noWarn2() {
	std::min(1, 2);
}

static void noWarn3() {
	std::max(1, 2);
}

static void noWarn4() {
	int V[5] { 2, 3, 4, 5, 6 };
	auto begin = std::begin(V); // NOLINT
	auto end = std::end(V); // NOLINT
	std::sort(begin, end);
}

static void noWarn5() {
	std::unique_ptr<int> Ptr;
}

static void noWarn6() {
	auto Ptr = std::make_unique<int>(1);
	Ptr.reset();
}

static void noWarn7() {
	std::vector<int> V;
	std::fill(V.begin(), V.end(), 1);
}

static void noWarn8() {
	std::atomic_int A;
}

static void noWarn9() {
	std::atomic_bool A;
}

static void warn1() {
	std::vector<int> V { 1, 4, 5, 6, 7 };
	std::adjacent_find(V.begin(), V.end());
}

static void warn2() {
	std::shared_ptr<int> Ptr;
}

static void warn3() {
	auto Ptr = std::make_shared<int>(1);
}
