#ifndef DICE_METALLFFI_METALL_H
#define DICE_METALLFFI_METALL_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Attempts to open the metall datastore at path
 * @return true on success, false on failure. On failure, sets errno to one of the following values:
 *      - ENOTRECOVERABLE if the given metall datastore is inconsistent
 */
bool metall_open(char const *path);

/**
 * @brief Attempts to create a metall datastore at path
 * @return true on success, false on failure. On failure, sets errno to one of the following values:
 *      - EEXIST if the given path already exists
 */
bool metall_create(char const *path);

/**
 * @brief Checks if a metall manager is open for the given path
 * @return true if open otherwise false
 */
bool metall_is_open(char const *path);

/**
 * @brief checks if the datastore at the given path is in consistent state.
 * @return true if the datastore is consistent, false otherwise.
 */
bool metall_is_consistent(char const *path);

/**
 * @brief Creates a snapshot of the metall datastore at src_path and places it at dst_path
 * @return true if the snapshot was successfully created otherwise false. On failure sets errno to one of the following values:
 *      - EINVAL if there is no metall datastore open at src_path
 */
bool metall_snapshot(char const *src_path, char const *dst_path);

/**
 * @brief Attempts to close the metall datastore opened at path
 * @return true on success, false on failure. On failure, sets errno to one of the following values:
 *      - EINVAL if the given path does not have a metall datastore open
 */
bool metall_close(char const *path);

/**
 * @brief Removes the metall datastore at path
 * @return true on successful removal, false otherwise. On failure, sets errno to one of the following values:
 *      - EADDRINUSE if there is a metall manager open for the given path
 */
bool metall_remove(char const *path);


/**
 * @brief Allocates size bytes and associates the allocated memory with a name
 * @param path path of the open datastore
 * @param name A name of the allocated memory
 * @param size number of bytes to allocate
 * @return pointer to the allocated memory if sucessful otherwise returns NULL and sets errno to one of the following values
 *      - EINVAL if the given path does not have a metall datastore open
 *      - ENOMEM if the memory could not be allocated
 */
void *metall_malloc(char const *path, char const *name, size_t size);

/**
 * @brief Finds memory that was previously allocated using metall_named_alloc
 * @param path path of the open datastore
 * @param name name of the allocated memory to find
 * @return pointer to the allocated memory if found. Otherwise, returns NULL and sets errno to one of the following values
 *      - EINVAL if the given path does not have a metall datastore open
 *      - ENOTENT if the object could not be found
 */
void *metall_find(char const *path, char const *name);

/**
 * @brief Frees memory previously allocated by metall_named_malloc
 * @param path path of the open datastore
 * @param name name of the allocated memory to free
 * @return true if sucessfully freed, otherwise returns false and sets errno to one of the following values
 *      - EINVAL if the given path does not have a metall datastore open
 *      - ENOENT if the referred to object does not exist
 */
bool metall_free(char const *path, char const *name);

#ifdef __cplusplus
}
#endif

#endif//DICE_METALLFFI_METALL_H
