#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <dice/ffi/metall.h>

#include <string>
#include <random>

TEST_CASE("sanity check") {
    char const *obj_name = "obj";
    std::string const path = "/tmp/" + std::to_string(std::random_device{}());
    std::string const snap_path = path + "-snap";

    {
        if (!metall_create(path.c_str())) {
            FAIL("failed to create: ", strerror(errno));
        }
        CHECK(metall_is_open(path.c_str()));

        auto *ptr = static_cast<size_t *>(metall_malloc(path.c_str(), obj_name, sizeof(size_t)));
        CHECK_NE(ptr, nullptr);
        CHECK_EQ(reinterpret_cast<uintptr_t>(ptr) % alignof(size_t), 0);

        *ptr = 55;
        CHECK_EQ(*ptr, 55);

        if (!metall_snapshot(path.c_str(), snap_path.c_str())) {
            FAIL("failed to snapshot: ", strerror(errno));
        }

        if (!metall_close(path.c_str())) {
            FAIL("failed to close: ", strerror(errno));
        }

        CHECK(!metall_is_open(path.c_str()));
    }

    auto check = [obj_name](auto const &path) {
        CHECK(metall_is_consistent(path.c_str()));

        if (!metall_open(path.c_str())) {
            FAIL("failed to open: ", strerror(errno));
        }

        CHECK(metall_is_open(path.c_str()));

        auto *ptr = static_cast<size_t *>(metall_find(path.c_str(), obj_name));
        if (ptr == nullptr) {
            FAIL("failed to load: ", strerror(errno));
        }

        CHECK_EQ(*ptr, 55);

        if (!metall_free(path.c_str(), obj_name)) {
            FAIL("failed to dealloc: ", strerror(errno));
        }

        if (!metall_close(path.c_str())) {
            FAIL("failed to close: ", strerror(errno));
        }

        CHECK(!metall_is_open(path.c_str()));
        CHECK(metall_remove(path.c_str()));
        CHECK(!metall_open(path.c_str()));
    };

    check(snap_path);
    check(path);
}
