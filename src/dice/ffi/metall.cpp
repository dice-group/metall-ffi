#include <dice/ffi/metall.h>
#include <dice/ffi/metall_internal.hpp>

namespace dice::metall_ffi::internal {
    dice::sparse_map::sparse_map<std::string, std::unique_ptr<metall_manager>> metall_managers;
} // dice::metall_ffi::internal

using namespace dice::metall_ffi::internal;

bool metall_open(char const *path) {
    if (auto it = metall_managers.find(path); it != metall_managers.end()) {
        return true;
    }

    if (!metall::manager::consistent(path)) {
        errno = ENOTRECOVERABLE;
        return false;
    }

    metall_managers.emplace(path, std::make_unique<metall_manager>(metall::open_only, path));
    return true;
}

bool metall_create(char const *path) {
    if (auto it = metall_managers.find(path); it != metall_managers.end()) {
        return true;
    }

    if (std::filesystem::exists(path)) {
        errno = EEXIST;
        return false;
    }

    metall_managers.emplace(path, std::make_unique<metall_manager>(metall::create_only, path));
    return true;
}

bool metall_is_open(char const *path) {
    return metall_managers.find(path) != metall_managers.end();
}

bool metall_is_consistent(char const *path) {
    if (metall_is_open(path)) {
        return true;
    }

    return metall::manager::consistent(path);
}

bool metall_snapshot(char const *src_path, char const *dst_path) {
    auto it = metall_managers.find(src_path);
    if (it == metall_managers.end()) {
        errno = EINVAL;
        return false;
    }

    return it->second->snapshot(dst_path);
}

bool metall_close(char const *path) {
    auto it = metall_managers.find(path);
    if (it == metall_managers.end()) {
        errno = EINVAL;
        return false;
    }

    metall_managers.erase(it);
    return true;
}

bool metall_remove(char const *path) {
    if (auto it = metall_managers.find(path); it != metall_managers.end()) {
        errno = EADDRINUSE;
        return false;
    }

    return metall::manager::remove(path);
}

void *metall_malloc(char const *path, char const *name, size_t size) {
    auto it = metall_managers.find(path);
    if (it == metall_managers.end()) {
        errno = EINVAL;
        return nullptr;
    }

    auto *ptr = it->second->construct<char>(name)[size]();
    if (ptr == nullptr) {
        errno = ENOMEM;
    }

    return ptr;
}

void *metall_find(char const *path, char const *name) {
    auto it = metall_managers.find(path);
    if (it == metall_managers.end()) {
        errno = EINVAL;
        return nullptr;
    }

    auto *ptr = it->second->find<char>(name).first;
    if (ptr == nullptr) {
        errno = ENOENT;
    }

    return ptr;
}

bool metall_free(char const *path, char const *name) {
    auto it = metall_managers.find(path);
    if (it == metall_managers.end()) {
        errno = EINVAL;
        return false;
    }

    auto const res = it->second->destroy<char>(name);
    if (!res) {
        errno = ENOENT;
    }

    return res;
}
