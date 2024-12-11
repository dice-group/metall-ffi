#ifndef DICE_METALLFFI_METALL_H
#define DICE_METALLFFI_METALL_H

#include <stdbool.h>
#include <stddef.h>

#if __has_include(<metall/logger_interface.h>)
#include <metall/logger_interface.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !__has_include(<metall/logger_interface.h>)
/// \brief Log message level
typedef enum metall_log_level {
    /// \brief Critical logger message
    metall_critical = 5,
    /// \brief Error logger message
    metall_error = 4,
    /// \brief Warning logger message
    metall_warning = 3,
    /// \brief Info logger message
    metall_info = 2,
    /// \brief Debug logger message
    metall_debug = 1,
    /// \brief Verbose (lowest priority) logger message
    metall_verbose = 0,
} metall_log_level;
#endif


typedef struct metall_manager metall_manager;

typedef struct metall_path {
    char const *data;
    size_t size;
} metall_path;

/**
 * @brief Attempts to open the metall datastore at path
 * @param path path to datastore
 * @return true on success, false on failure. On failure, sets errno to one of the following values:
 *      - ENOTRECOVERABLE if the given metall datastore is inconsistent
 */
metall_manager *metall_open(metall_path path);

/**
 * @brief Attempts to open the metall datastore at path in read only mode
 * @param path path to datastore
 * @return true on success, false on failure. On failure, sets errno to one of the following values:
 *      - ENOTRECOVERABLE if the given metall datastore is inconsistent
 */
metall_manager *metall_open_read_only(metall_path path);

/**
 * @brief Attempts to create a metall datastore at path
 * @param path path at which to create a datastore
 * @return true on success, false on failure. On failure, sets errno to one of the following values:
 *      - EEXIST if the given path already exists
 *      - ENOTRECOVERABLE if the datastore could not be created for some other reason
 */
metall_manager *metall_create(metall_path path);

/**
 * @brief Attempts to create a metall datastore at path
 * @param path path at which to create a datastore
 * @param capacity maximum capacity for metall manager
 * @return true on success, false on failure. On failure, sets errno to one of the following values:
 *      - EEXIST if the given path already exists
 *      - ENOTRECOVERABLE if the datastore could not be created for some other reason
 */
metall_manager *metall_create_with_capacity_limit(metall_path path, size_t capacity);

/**
 * @brief Returns true if the metall manager was opened as read-only
 * @param manager manager to check
 * @return true if the given manager was openened as read-only
 */
bool metall_is_read_only(metall_manager const *manager);

/**
 * @brief Creates a snapshot of the metall datastore of manager and places it at dst_path
 * @param manager manager to perform snapshot
 * @param dst_path path where to place the snapshot
 * @return true if the snapshot was successfully created otherwise false.
 */
bool metall_snapshot(metall_manager *manager, metall_path dst_path);

/**
 * @brief Closes a metall manager
 */
void metall_close(metall_manager *manager);

/**
 * @brief Removes the metall datastore at path
 * @param path path to datastore to remove
 * @return true on successful removal, false otherwise. On failure, sets errno to one of the following values:
 *      - EADDRINUSE if there is a metall manager open for the given path
 *
 * @warning Behaviour is undefined if there is still a metall manager for path open
 */
bool metall_remove(metall_path path);

/**
 * @brief Allocates size bytes and associates the allocated memory with a name
 * @param manager metall manager to allocate with
 * @param name A name of the allocated memory
 * @param size number of bytes to allocate
 * @return pointer to the allocated memory if sucessful otherwise returns NULL and sets errno to one of the following values
 *      - EINVAL if the given path does not have a metall datastore open
 *      - ENOMEM if the memory could not be allocated
 */
void *metall_malloc(metall_manager *manager, char const *name, size_t size);

/**
 * @brief Finds memory that was previously allocated using metall_named_alloc
 * @param manager manager to find the object in
 * @param name name of the allocated memory to find
 * @return pointer to the allocated memory if found. Otherwise, returns NULL and sets errno to one of the following values
 *      - EINVAL if the given path does not have a metall datastore open
 *      - ENOTENT if the object could not be found
 */
void *metall_find(metall_manager *manager, char const *name);

/**
 * @brief Frees memory previously allocated by metall_named_malloc
 * @param manager manager from which to free
 * @param name name of the allocated memory to free
 * @return true if sucessfully freed, otherwise returns false and sets errno to one of the following values
 *      - EINVAL if the given path does not have a metall datastore open
 *      - ENOENT if the referred to object does not exist
 */
bool metall_free(metall_manager *manager, char const *name);

#ifdef __cplusplus
}
#endif

#endif//DICE_METALLFFI_METALL_H
