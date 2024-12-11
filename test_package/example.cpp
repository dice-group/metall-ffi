#include <dice/ffi/metall.h>
#include <cstring>

#if __has_include(<metall/logger_interface.h>)
extern "C" void metall_log(metall_log_level, char const *, size_t, char const *) {
	// noop
}
#endif

int main() {
    char const *path = "/tmp/test";
    metall_open(metall_path{path, strlen(path)});
}
