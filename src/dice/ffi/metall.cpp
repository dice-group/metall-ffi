#include <dice/ffi/metall.h>
#include <dice/ffi/metall_internal.hpp>

using metall_manager_t = dice::metall_ffi::internal::metall_manager;

metall_manager *metall_open(char const *path) {
    if (!metall::manager::consistent(path)) {
        errno = ENOTRECOVERABLE;
        return nullptr;
    }

    return reinterpret_cast<metall_manager *>(new metall_manager_t{metall::open_only, path});
}

metall_manager *metall_create(char const *path) {
    if (std::filesystem::exists(path)) {
        errno = EEXIST;
        return nullptr;
    }

    return reinterpret_cast<metall_manager *>(new metall_manager_t{metall::create_only, path});
}

bool metall_is_consistent(char const *path) {
    return metall::manager::consistent(path);
}

bool metall_snapshot(metall_manager *manager, char const *dst_path) {
    return reinterpret_cast<metall_manager_t *>(manager)->snapshot(dst_path);
}

void metall_close(metall_manager *manager) {
    delete reinterpret_cast<metall_manager_t *>(manager);
}

bool metall_remove(char const *path) {
    return metall::manager::remove(path);
}

void *metall_malloc(metall_manager *manager, char const *name, size_t size) {
    auto *ptr = reinterpret_cast<metall_manager_t *>(manager)->construct<char>(name)[size]();
    if (ptr == nullptr) {
        errno = ENOMEM;
        return nullptr;
    }

    return ptr;
}

void *metall_find(metall_manager *manager, char const *name) {
    auto *ptr = reinterpret_cast<metall_manager_t *>(manager)->find<char>(name).first;
    if (ptr == nullptr) {
        errno = ENOENT;
    }

    return ptr;
}

bool metall_free(metall_manager *manager, char const *name) {
    auto const res = reinterpret_cast<metall_manager_t *>(manager)->destroy<char>(name);
    if (!res) {
        errno = ENOENT;
    }

    return res;
}
