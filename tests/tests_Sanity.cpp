#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <dice/ffi/metall.h>

#include <string>
#include <random>

extern "C" void metall_log(metall_log_level lvl, char const *file, size_t line, char const *msg) {
    std::cerr << lvl << " " << file << ":" << line << ": " << msg << std::endl;
}

TEST_SUITE("metall-ffi") {
    TEST_CASE("sanity check") {
        char const *obj_name = "obj";
        std::string const path = "/tmp/metall-ffi" + std::to_string(std::random_device{}());
        std::string const snap_path = path + "-snap";

        {
            metall_manager *manager = metall_create(path.c_str());
            if (manager == nullptr) {
                FAIL("failed to create: ", strerror(errno));
            }

            {
                auto *ptr = static_cast<size_t *>(metall_malloc(manager, obj_name, sizeof(size_t) * 2));
                CHECK_NE(ptr, nullptr);
                CHECK_EQ(reinterpret_cast<uintptr_t>(ptr) % alignof(size_t), 0);

                *ptr = 55;
                CHECK_EQ(*ptr, 55);
            }

            metall_close(manager);
        }

        {
            metall_manager *manager = metall_open(path.c_str());
            if (manager == nullptr) {
                FAIL("failed to create: ", strerror(errno));
            }

            auto *ptr = static_cast<size_t *>(metall_find(manager, obj_name));
            CHECK_NE(ptr, nullptr);
            CHECK_EQ(reinterpret_cast<uintptr_t>(ptr) % alignof(size_t), 0);

            *ptr = 66;
            CHECK_EQ(*ptr, 66);

            if (!metall_snapshot(manager, snap_path.c_str())) {
                FAIL("failed to snapshot: ", strerror(errno));
            }

            metall_close(manager);
        }

        auto check = [obj_name](auto const &path) {
            {
                metall_manager *manager = metall_open_read_only(path.c_str());
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
                metall_manager *manager = metall_open(path.c_str());
                if (manager == nullptr) {
                    FAIL("failed to open: ", strerror(errno));
                }

                if (!metall_free(manager, obj_name)) {
                    FAIL("failed to dealloc: ", strerror(errno));
                }

                metall_close(manager);

                CHECK(metall_remove(path.c_str()));
                CHECK(!metall_open(path.c_str()));
            }
        };

        check(snap_path);
        check(path);
    }

    TEST_CASE("prevent open same datastore twice") {
        std::string const path = "/tmp/" + std::to_string(std::random_device{}());
        metall_manager *manager = metall_create(path.c_str());
        if (manager == nullptr) {
            FAIL("failed to create datastore: ", strerror(errno));
        }

        metall_manager *manager2 = metall_open(path.c_str());
        CHECK_EQ(manager2, nullptr);
        CHECK_EQ(errno, ENOTRECOVERABLE);

        metall_manager *manager3 = metall_open(path.c_str());
        CHECK_EQ(manager3, nullptr);
        CHECK_EQ(errno, ENOTRECOVERABLE);

        metall_close(manager);
        CHECK(metall_remove(path.c_str()));
    }
}
