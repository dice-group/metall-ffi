#include <dice/ffi/metall.h>

#if __has_include(<metall/logger_interface.h>)
extern "C" void metall_log(metall_log_level, char const *, size_t, char const *) {
	// noop
}
#endif

int main() {
    metall_open("/tmp/test");
}
