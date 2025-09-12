// RUN: %check_clang_tidy %s readability-mpe-ternary-operator-check %t

static void warn1() {
	bool Val = 0 ? true : false;
}

static void noWarn1() {
	if (0) {
		true;
	} else {
		false;
	}
}
