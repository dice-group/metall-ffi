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
        metall_manager *manager = metall_create(path.c_str());
        if (manager == nullptr) {
            FAIL("failed to create: ", strerror(errno));
        }

        auto *ptr = static_cast<size_t *>(metall_malloc(manager, obj_name, sizeof(size_t)));
        CHECK_NE(ptr, nullptr);
        CHECK_EQ(reinterpret_cast<uintptr_t>(ptr) % alignof(size_t), 0);

        *ptr = 55;
        CHECK_EQ(*ptr, 55);

        if (!metall_snapshot(manager, snap_path.c_str())) {
            FAIL("failed to snapshot: ", strerror(errno));
        }

        metall_close(manager);
    }

    auto check = [obj_name](auto const &path) {
        CHECK(metall_is_consistent(path.c_str()));

        metall_manager *manager = metall_open(path.c_str());
        if (manager == nullptr) {
            FAIL("failed to open: ", strerror(errno));
        }

        auto *ptr = static_cast<size_t *>(metall_find(manager, obj_name));
        if (ptr == nullptr) {
            FAIL("failed to load: ", strerror(errno));
        }

        CHECK_EQ(*ptr, 55);

        if (!metall_free(manager, obj_name)) {
            FAIL("failed to dealloc: ", strerror(errno));
        }

        metall_close(manager);

        CHECK(metall_remove(path.c_str()));
        CHECK(!metall_open(path.c_str()));
    };

    check(snap_path);
    check(path);
}
