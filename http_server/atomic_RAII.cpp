#include <unistd.h>
#include <iostream>
#include "atomic_RAII.h"

using namespace std;

atomic_RAII::atomic_RAII(atomic<uint32_t>& var) : var_(var) {
	cout << "Start Connection" << endl;
	var_++;
}

atomic_RAII::~atomic_RAII() {
	cout << "End Connection" << endl;
	var_--;
}
