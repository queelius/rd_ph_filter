#ifndef __PHFILTER_H__
#define __PHFILTER_H__

#include <iostream>
#include <string>
#include <ostream>
#include "BitArray.h"
#include "FASet.h"
#include "ICmph.h"
#include "Hash.h"
#include "Utils.h"
#include "BinaryIO.h"
#include <iomanip>
#include <cstdint>
#include <vector>

namespace alex::collections::set
{
    // Perfect hash filter: an implementation of the approximate set ADT

    template <typename T>
    class PerfectHashFilter : public FASet<T>
    {
        public:
            const static uint8_t VERSION = 1;
            const static char HEADER[];

            PerfectHashFilter() {};
            PerfectHashFilter(const PerfectHashFilter& copy)
            {
            };

            PerfectHashFilter(PerfectHashFilter&& src)
            {
            };

            PerfectHashFilter& operator=(PerfectHashFilter&& other)
            {
                if (this != &other)
                {
                }
                return *this;
            };

            PerfectHashFilter& operator=(const PerfectHashFilter& other)
            {
                if (this != &other)
                {
                }
                return *this;
            };

            void read(std::istream& file)
            {
                const std::string& hdr = alex::io::binary::readString(file);
                if (hdr != HEADER)
                    throw std::exception(("Unexpected Header: " + hdr).c_str());

                unsigned int version = alex::io::binary::readU8(file);
                if (version != VERSION)
                    throw std::exception("Unexpected Version");

                // de-serialize perfect hash function
                _ph.read(file);

                // de-serialize matrix of hashes
                _hashes.read(file); // read bit array for compactly (packed) storing hash entries
            };

            void write(std::ostream& file)
            {
                alex::io::binary::writeString(file, HEADER);
                alex::io::binary::writeU8(file, VERSION);

                // serialize perfect hash function
                _ph.write(file);

                // write bit matrix of hashes
                _hashes.write(file);
            };

            virtual ~PerfectHashFilter() { clear(); };

            void clear()
            {
                if (_h.empty())
                {
                    _hashes.clear();
                    _ph.clear();
                }
            };

            bool contains(const T& x) const
            {
                const uint32_t j = _ph.search(x);
                const uint32_t value = _hash(x) % get_max_hash();
                if (_hashes.get(j) != value)
                    return false;
                return true;
            };

            std::string preferred_file_extension() const { return HEADER; };

            uint32_t max_hash() const { return 1 << get_hash_width(); };

            uint32_t hash_width() const { return _h.columns(); };

            float load_factor() const
            {
                return 0f;
            };

            float fp_rate() const { return std::pow(2., -hash_width()); };

        private:
            static const unsigned int MAX_BYTE = (1 << CHAR_BIT);
        
            alex::hash::ICmph _ph;
            alex::collections::PackedArray<uint32_t> _hashes;
            HashFn _hash;
    };

    template <typename HashFn>
    const char PerfectHashFilter<HashFn>::HEADER[] = "phf";

    template <typename T, class SerializeFn, class HashFn>
    class PerfectHashFilterT : public ApproximateSet<T>
    {
    public:
    };
}

#endif