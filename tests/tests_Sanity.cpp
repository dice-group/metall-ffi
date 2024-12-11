#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <dice/ffi/metall.h>

#include <string>
#include <random>

#if __has_include(<metall/logger_interface.h>)
extern "C" void metall_log(metall_log_level lvl, char const *file, size_t line, char const *msg) {
    std::cerr << lvl << " " << file << ":" << line << ": " << msg << std::endl;
}
#endif

[[nodiscard]] inline metall_path string_to_path(std::string_view str) noexcept {
    return metall_path{str.data(), str.size()};
}

TEST_SUITE("metall-ffi") {
    TEST_CASE("sanity check") {
        char const *obj_name = "obj";
        std::string const path = "/tmp/metall-ffi" + std::to_string(std::random_device{}());
        std::string const snap_path = path + "-snap";

        {
            metall_manager *manager = metall_create(string_to_path(path));
            if (manager == nullptr) {
                FAIL("failed to create: ", strerror(errno));
            }

            {
                auto *ptr = static_cast<size_t *>(metall_malloc(manager, obj_name, sizeof(size_t)));
                CHECK_NE(ptr, nullptr);
                CHECK_EQ(reinterpret_cast<uintptr_t>(ptr) % alignof(size_t), 0);

                *ptr = 55;
                CHECK_EQ(*ptr, 55);
            }

            metall_close(manager);
        }

        {
            metall_manager *manager = metall_open(string_to_path(path));
            if (manager == nullptr) {
                FAIL("failed to create: ", strerror(errno));
            }

            auto *ptr = static_cast<size_t *>(metall_find(manager, obj_name));
            CHECK_NE(ptr, nullptr);
            CHECK_EQ(reinterpret_cast<uintptr_t>(ptr) % alignof(size_t), 0);

            *ptr = 66;
            CHECK_EQ(*ptr, 66);

            if (!metall_snapshot(manager, string_to_path(snap_path))) {
                FAIL("failed to snapshot: ", strerror(errno));
            }

            metall_close(manager);
        }

        auto check = [obj_name](auto const &path) {
            {
                metall_manager *manager = metall_open_read_only(string_to_path(path));
                if (manager == nullptr) {
                    FAIL("failed to open: ", strerror(errno));
                }

                auto *ptr = static_cast<size_t *>(metall_find(manager, obj_name));
                if (ptr == nullptr) {
                    FAIL("failed to load: ", strerror(errno));
                }

                CHECK_EQ(*ptr, 66);

                CHECK_FALSE(metall_free(manager, obj_name));

                metall_close(manager);
            }

            {
                metall_manager *manager = metall_open(string_to_path(path));
                if (manager == nullptr) {
                    FAIL("failed to open: ", strerror(errno));
                }

                if (!metall_free(manager, obj_name)) {
                    FAIL("failed to dealloc: ", strerror(errno));
                }

                metall_close(manager);

                CHECK(metall_remove(string_to_path(path)));
                CHECK(!metall_open(string_to_path(path)));
            }
        };

        check(snap_path);
        check(path);
    }

    TEST_CASE("prevent open same datastore twice") {
        std::string const path = "/tmp/" + std::to_string(std::random_device{}());
        metall_manager *manager = metall_create(string_to_path(path));
        if (manager == nullptr) {
            FAIL("failed to create datastore: ", strerror(errno));
        }

        metall_manager *manager2 = metall_open(string_to_path(path));
        CHECK_EQ(manager2, nullptr);
        CHECK_EQ(errno, ENOTRECOVERABLE);

        metall_manager *manager3 = metall_open(string_to_path(path));
        CHECK_EQ(manager3, nullptr);
        CHECK_EQ(errno, ENOTRECOVERABLE);

        metall_close(manager);
        CHECK(metall_remove(string_to_path(path)));
    }

    TEST_CASE("capacity limit") {
        std::string const path = "/tmp/" + std::to_string(std::random_device{}());
        metall_manager *manager = metall_create_with_capacity_limit(string_to_path(path), 1);
        if (manager == nullptr) {
            FAIL("failed to create datastore: ", strerror(errno));
        }

        auto ret = metall_malloc(manager, "aaa", 1'000'000'000);
        if (ret != nullptr) {
            metall_free(manager, "aaa");
            FAIL("Did not refuse to allocate");
        }
    }
}
