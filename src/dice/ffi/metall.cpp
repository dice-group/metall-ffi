#include <dice/ffi/metall.h>
#include <dice/ffi/metall_internal.hpp>

#include <functional>

using metall_manager_t = dice::metall_ffi::internal::metall_manager;

inline std::filesystem::path from_ffi_path(metall_path path) {
    return std::filesystem::path{std::string{path.data, path.size}};
}

template<auto open_mode>
metall_manager *open_impl(metall_path path_) {
    auto const path = from_ffi_path(path_);

    if (!metall::manager::consistent(path)) {
        // prevents opening the same datastore twice
        // (because opening removes the properly_closed_mark and this checks for it)
        errno = ENOTRECOVERABLE;
        return nullptr;
    }

    auto *manager = new metall_manager_t{open_mode, path};
    if (!manager->check_sanity()) {
        delete manager;
        errno = ENOTRECOVERABLE;
        return nullptr;
    }

    return reinterpret_cast<metall_manager *>(manager);
}

template<typename Create>
metall_manager *create_impl(metall_path path_, Create &&create) {
    auto const path = from_ffi_path(path_);

    if (std::filesystem::exists(path)) {
        // prevent accidental overwrite
        errno = EEXIST;
        return nullptr;
    }

    auto *manager = std::invoke(std::forward<Create>(create), path);
    if (!manager->check_sanity()) {
        delete manager;
        errno = ENOTRECOVERABLE;
        return nullptr;
    }

    return reinterpret_cast<metall_manager *>(manager);
}

metall_manager *metall_open(metall_path path) {
    return open_impl<metall::open_only>(path);
}

metall_manager *metall_open_read_only(metall_path path) {
    return open_impl<metall::open_read_only>(path);
}

metall_manager *metall_create(metall_path path) {
    return create_impl(path, [](std::filesystem::path const &path) {
        return new metall_manager_t{metall::create_only, path};
    });
}

metall_manager *metall_create_with_capacity_limit(metall_path path, size_t capacity) {
    return create_impl(path, [capacity](std::filesystem::path const &path) {
        return new metall_manager_t{metall::create_only, path, capacity};
    });
}

bool metall_is_read_only(metall_manager const *manager) {
    return reinterpret_cast<metall_manager_t const *>(manager)->read_only();
}

bool metall_snapshot(metall_manager *manager, metall_path dst_path_) {
    auto const dst_path = from_ffi_path(dst_path_);
    return reinterpret_cast<metall_manager_t *>(manager)->snapshot(dst_path);
}

void metall_close(metall_manager *manager) {
    delete reinterpret_cast<metall_manager_t *>(manager);
}

bool metall_remove(metall_path path_) {
    auto const path = from_ffi_path(path_);
    return metall::manager::remove(path);
}

void *metall_malloc(metall_manager *manager, char const *name, size_t size) {
    auto *ptr = reinterpret_cast<metall_manager_t *>(manager)->construct<unsigned char>(name)[size]();
    if (ptr == nullptr) {
        errno = ENOMEM;
    }

    return ptr;
}

void *metall_find(metall_manager *manager, char const *name) {
    auto *ptr = reinterpret_cast<metall_manager_t *>(manager)->find<unsigned char>(name).first;
    if (ptr == nullptr) {
        errno = ENOENT;
    }

    return ptr;
}

bool metall_free(metall_manager *manager, char const *name) {
    auto const res = reinterpret_cast<metall_manager_t *>(manager)->destroy<unsigned char>(name);
    if (!res) {
        errno = ENOENT;
    }

    return res;
}
