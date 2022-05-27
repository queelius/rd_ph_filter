#ifndef __UNSIGNED_INTEGER_PACKED_ARRAY_H__
#define __UNSIGNED_INTEGER_PACKED_ARRAY_H__

#include "PackedArray.h"
#include <iostream>

namespace alex::collections
{
    class UnsignedIntegerPackedArrayCodec: public PackedArrayCodec<uint32_t>
    {
    public:
        UnsignedIntegerPackedArrayCodec(uint32_t min, uint32_t max) : _max(max), _min(min) {};

        using PackedArrayCodec<uint32_t>::size_type;

        size_type bit_length() const
        {
            const static size_type bl = std::ceil(std::log(_max - _min));
            return bl;
        };

        uint32_t max() const { _max; };
        uint32_t min() const { _min; };

        void encode(const uint32_t& value, BitArray& ba, size_type offset)
        {
            if (value > _max || value < _min)
                throw std::invalid_argument("[UnsignedIntegerCodec::encode(uint32_t, BitArray, size_type)] Value Not In Range");

            uint32_t v = value - _min;
            for (size_type i = 0; i < bit_length(); ++i)
            {
                ba.set(offset + bit_length() - i - 1, v % 2 != 0);
                v /= 2;
            }
        };

        uint32_t decode(const BitArray& ba, size_type offset) const
        {
            uint32_t value = _min;
            for (size_type i = 0; i < bit_length(); ++i)
            {
                value <<= 1;
                value += static_cast<uint32_t>(ba.is_set(offset + i));
            }
            return value;
        };

    private:
        uint32_t _max, _min;
    };

    class UnsignedIntegerPackedArray: public PackedArray<uint32_t>
    {
    public:
        using PackedArray<uint32_t>::size_type;

        UnsignedIntegerPackedArray(size_type size, uint32_t min, uint32_t max) : PackedArray<uint32_t>(size, new UnsignedIntegerPackedArrayCodec(min, max))
        {
        };

        void write(std::ostream& file) const
        {
            const auto& codec = dynamic_cast<UnsignedIntegerPackedArrayCodec*>(_codec);
            alex::io::binary::write_var_u32(file, codec->max());
            alex::io::binary::write_var_u32(file, codec->min());
            _ba.write(file);
        };

    private:
    };
}

#endif