#ifndef DICE_METALLFFI_METALLINTERNAL_HPP
#define DICE_METALLFFI_METALLINTERNAL_HPP

#include <boost/version.hpp>

#if BOOST_VERSION >= 108500
#include <boost/interprocess/detail/segment_manager_helper.hpp>

namespace boost::interprocess::ipcdetail {
     /**
      * This function was originally in boost, but they removed it in recent release (>=1.85)
      * Even though this is an implementation detail of boost, metall still relies on it, so
      * to ensure compatibility with more recent boost releases we have to add it here.
      * (slightly modified)
      *
      * https://github.com/boostorg/interprocess/blob/a0c5a8ff176434c9024d4540ce092a2eebb8c5c3/include/boost/interprocess/detail/segment_manager_helper.hpp#L211-L224
      */
     inline void array_construct(void *mem, std::size_t num, in_place_interface &table) {
          //Try constructors
          BOOST_TRY{
               table.construct_n(mem, num);
          }
          //If there is an exception call destructors and erase index node
          BOOST_CATCH(...){
               table.destroy_n(mem, num);
               BOOST_RETHROW
          } BOOST_CATCH_END
    }
} // namespace boost::interprocess::ipcdetail

#endif // BOOST_VERSION


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
