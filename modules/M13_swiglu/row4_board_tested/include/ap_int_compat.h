#ifndef SWIGLU_AP_INT_COMPAT_H
#define SWIGLU_AP_INT_COMPAT_H

#if defined(__has_include)
#  if __has_include(<ap_int.h>)
#    include <ap_int.h>
#    define SWIGLU_HAS_VENDOR_AP_INT 1
#  endif
#endif

// Vitis HLS provides ap_uint through ap_int.h.  The small fallback below is
// only for the repository's portable g++ interface test; synthesis always
// uses the vendor implementation.
#ifndef SWIGLU_HAS_VENDOR_AP_INT
#include <stdint.h>
#include <type_traits>

template <int Width>
class ap_uint {
    static_assert(Width > 0 && Width <= 128,
                  "portable ap_uint fallback supports widths 1..128");

public:
    typedef unsigned __int128 storage_t;

    ap_uint() : value_(0) {}

    template <typename T,
              typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    ap_uint(T value) : value_((storage_t)value & mask()) {}

    ap_uint operator>>(unsigned shift) const {
        ap_uint result;
        result.value_ = shift >= 128 ? 0 : value_ >> shift;
        return result;
    }

    ap_uint operator<<(unsigned shift) const {
        ap_uint result;
        result.value_ = shift >= 128 ? 0 : (value_ << shift) & mask();
        return result;
    }

    ap_uint &operator|=(const ap_uint &other) {
        value_ = (value_ | other.value_) & mask();
        return *this;
    }

    explicit operator uint16_t() const { return (uint16_t)value_; }
    explicit operator uint32_t() const { return (uint32_t)value_; }
    explicit operator uint64_t() const { return (uint64_t)value_; }

private:
    static storage_t mask() {
        return Width == 128
                   ? ~(storage_t)0
                   : (((storage_t)1 << Width) - (storage_t)1);
    }

    storage_t value_;
};
#endif

#endif  // SWIGLU_AP_INT_COMPAT_H
