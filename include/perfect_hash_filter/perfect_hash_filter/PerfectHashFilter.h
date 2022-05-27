#ifndef __PERFECT_HASH_FILTER_H__
#define __PERFECT_HASH_FILTER_H__

#include <iostream>
#include <string>
#include <ostream>
#include "ICmph.h"
#include "BinaryIO.h"
#include <cstdint>
#include <vector>
#include "FASet.h"

namespace alex::collections
{
    template <class PHF>
    class PerfectHashFilter
    {
        public:
            const static uint8_t VERSION = 1;
            const static char HEADER[];

            PerfectHashFilter() {};

            PerfectHashFilter(const PerfectHashFilter& copy) :
                _ph(copy._ph)
            {
            };

            PerfectHashFilter(PerfectHashFilter&& src) :
                _ph(std::move(src._ph))
            {
            };

            PerfectHashFilter& operator=(PerfectHashFilter&& other)
            {
                if (this != &other)
                {
                    clear();
                    _ph = std::move(other._ph);
                }

                return *this;
            };

            PerfectHashFilter& operator=(const PerfectHashFilter& other)
            {
                if (this != &other)
                {
                    clear();
                    _ph = other._ph;
                    _max_hash = other._max_hash;
                }

                return *this;
            };

            void read(std::istream& file)
            {
                using namespace alex::io::binary;

                const std::string& hdr = read_string(file);
                if (hdr != HEADER)
                    throw std::exception(("Unexpected Header: " + hdr).c_str());

                unsigned int version = read_u8(file);
                if (version != VERSION)
                    throw std::exception("Unexpected Version");

                // de-serialize perfect hash function
                _ph.read(file);

                // read hash entries
            };

            void write(std::ostream& file)
            {
                using namespace alex::io::binary;

                write_string(file, HEADER);
                write_u8(file, VERSION);

                // serialize perfect hash function
                _ph.write(file);
            };

            virtual ~PerfectHashFilter() { clear(); };

            void clear()
            {
                _ph.clear();
            };

        private:
            static const unsigned int MAX_BYTE = (1 << CHAR_BIT);
        
            alex::hash::ICmph _ph;
            uint32_t _max_hash;
    };

    const char PerfectHashFilter::HEADER[] = "phf";
}

#endif