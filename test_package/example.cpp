#include <dice/ffi/metall.h>

extern "C" void metall_log(metall_log_level, char const *, size_t, char const *) {
	// noop
}

int main() {
    metall_open("/tmp/test");
}
