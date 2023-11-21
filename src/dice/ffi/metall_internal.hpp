#ifndef DICE_METALLFFI_METALLINTERNAL_HPP
#define DICE_METALLFFI_METALLINTERNAL_HPP

#include <metall/metall.hpp>

namespace dice::metall_ffi::internal {
    /**
     * @brief The metall manager type used internally.
     *      This object type is whats actually behind the opaque ::metall_manager * in the interface
     */
    using metall_manager = metall::basic_manager<uint32_t, 1ULL << 28>;
} // namespace

#endif//DICE_METALLFFI_METALLINTERNAL_HPP
