#ifndef __IBINARY_CODEC_H__
#define __IBINARY_CODEC_H__

#include <string>
#include <vector>
#include "BinaryIO.h"

namespace alex::codec
{

    class ISerializable
    {
    public:
        virtual void write(std::ostream& file) const = 0;
        virtual void read(std::istream& file) = 0;
    };

    template <typename T>
    class IClonable
    {
    public:
        virtual T* clone() const = 0;
    };

    template <typename T>
    class IBinaryCodec:
        public ISerializable,
        public IClonable<IBinaryCodec<T>>
    {
    public:
        typedef T value_type;
        typedef std::vector<bool> binary_code;
        
        virtual T decode(const binary_code& code) const = 0;

        virtual binary_code encode(const T& value) const = 0;
    };

    class NaturalNumberBinaryCodec:
        public IBinaryCodec<uint64_t>
    {
    public:
        const static std::string HEADER;
        const static uint8_t VERSION = 1;

        IBinaryCodec<uint64_t>* clone() const
        {
            return new NaturalNumberBinaryCodec();
        };

        void write(std::ostream& file) const override
        {
            alex::io::binary::write_string(file, HEADER);
            alex::io::binary::write_u8(file, VERSION);
        };

        void read(std::istream& file) override
        {
            const auto& hdr = alex::io::binary::read_string(file);
            if (hdr != HEADER)
                throw std::exception("[]");

            const auto ver = alex::io::binary::read_u8(file);
            if (ver != VERSION)
                throw std::exception("[]");            
        };

        NaturalNumberBinaryCodec() {};

        binary_code encode(const uint64_t& value) const override
        {
            binary_code code;
            auto v = value;
            while (v != 0)
            {
                v % 2 != 0;
                v /= 2;
            }
        };

        uint64_t decode(const binary_code& code) const override
        {
            uint64_t v = 0;
            for (const auto& b : code)
            {
                v <<= 1;
                v += static_cast<uint64_t>(b);
            }
            return v;
        };
    };
    const std::string NaturalNumberBinaryCodec::HEADER = "cnn";

}

#endif