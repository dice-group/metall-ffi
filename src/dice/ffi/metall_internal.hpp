#ifndef DICE_METALLFFI_METALLINTERNAL_HPP
#define DICE_METALLFFI_METALLINTERNAL_HPP

#define METALL_LOGGER_EXTERN_C 1
#include <metall/metall.hpp>

namespace dice::metall_ffi::internal {
#if METALL_VERSION >= 2800
     /**
     * @brief The metall manager type used internally.
     *      This object type is whats actually behind the opaque ::metall_manager * in the interface
     */
     using metall_manager = metall::manager;
#else

     /**
      * @brief The metall manager type used internally.
      *      This object type is whats actually behind the opaque ::metall_manager * in the interface.
      *      This is a wrapper that contains the read_only flag for older versions that do not support read_only() yet.
      */
     struct metall_manager : metall::manager {
          bool read_only_;

          template<typename T>
          using allocator_type = typename metall::manager::template allocator_type<T>;

          metall_manager(metall::create_only_t op, char const *path) : metall::manager{op, path},
                                                                       read_only_{false} {
          }

          metall_manager(metall::open_only_t op, char const *path) : metall::manager{op, path},
                                                                     read_only_{false} {
          }

          metall_manager(metall::open_read_only_t op, char const *path) : metall::manager{op, path},
                                                                          read_only_{true} {
          }

          [[nodiscard]] bool read_only() const noexcept {
               return read_only_;
          }
     };
#endif
} // namespace

#endif//DICE_METALLFFI_METALLINTERNAL_HPP
