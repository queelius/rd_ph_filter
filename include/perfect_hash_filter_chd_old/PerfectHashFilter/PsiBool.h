#ifndef __PSI_BOOLEAN_H__
#define __PSI_BOOLEAN_H__

#include <iostream>
#include <string>
#include <ostream>
#include "ICmph.h"
#include "Hash.h"
#include "SHA.h"
#include "BinaryIO.h"
#include <iomanip>
#include <cstdint>
#include "HiddenQuery.h"
#include "ISecureIndex.h"
#include <vector>

namespace alex::encrypted_search
{
    class PsiBool : public ISecureIndex<true, true, false, false>
    {
        public:
            const static uint8_t VERSION = 1;
            const static char HEADER[];

            PsiBool() {};
            PsiBool(const PsiBool& copy) :
                _ph(copy._ph),
                _approximateWords(copy._approximateWords),
                _ref(copy._ref)
            {
            };
            PsiBool(PsiBool&& src) :
                _ph(std::move(src._ph)),
                _approximateWords(src._approximateWords),
                _ref(std::move(src._ref)) {};

            PsiBool& operator=(PsiBool&& other)
            {
                if (this != &other)
                {
                    clear();

                    _ph = std::move(other._ph);
                    _ref = std::move(other._ref);
                    _approximateWords = other._approximateWords;
                    other._approximateWords = 0;
                }

                return *this;
            };

            PsiBool& operator=(const PsiBool& other)
            {
                if (this != &other)
                {
                    clear();
                    _ph = other._ph;
                    _ref = other._ref;
                    _maxHash = other._maxHash;
                    _approximateWords = other._approximateWords;
                }

                return *this;
            };

            void read(std::istream& file)
            {
                using namespace alex::io::binary;

                const std::string& hdr = readString(file);
                if (hdr != HEADER)
                    throw std::exception(("Unexpected Header: " + hdr).c_str());

                unsigned int version = readU8(file);
                if (version != VERSION)
                    throw std::exception("Unexpected Version");

                // get reference (e.g., url) for encrypted document
                _ref = readString(file);

                // de-serialize perfect hash function
                _ph.read(file);

                // read hash entries

                _approximateWords = readVarU32(file);
            };

            void write(std::ostream& file)
            {
                using namespace alex::io::binary;

                writeString(file, HEADER);
                writeU8(file, VERSION);

                // write reference (e.g., url) for encrypted document
                writeString(file, _ref);

                // serialize perfect hash function
                _ph.write(file);

                // write approximate number of words in index
                writeVarU32(file, _approximateWords);
            };

            virtual ~PsiBool() { clear(); };

            void clear()
            {
                if (_car.size() != 0)
                {
                    _car.clear();
                    _ref = "";
                    _ph.clear();
                    _approximateWords = 0;
                }
            };
            unsigned int getApproximateWords() const { return _approximateWords; }
            bool hasFrequency() const { return false; }
            bool hasProximity() const { return false; }

            std::string type() const { return HEADER; }

            bool contains(const HiddenQuery::HiddenTerm& t) const
            {
                return getIndex(t) != -1;
            };

            int getIndex(const alex::index::HiddenQuery::HiddenTerm& t) const
            {
                const std::string suffix = "|" + _ref;
                for (const auto& x : t)
                {
                    const std::string& q = alex::crypt::hexdigest<16>(x + suffix);
                    const uint32_t idx = _ph(q);
                    //const uint32_t value = alex::hash::jenkinsHash2(q) % _maxHash;
                }

                return -1;
            };


            bool contains_any(const HiddenQuery& q) const
            {
                for (const auto& t : q.terms)
                {
                    if (contains(t))
                        return true;
                }
                return false;
            };

            bool contains_all(const HiddenQuery& q) const
            {
                for (const auto& t : q.terms)
                {
                    if (!contains(t))
                        return false;
                }

                return true;
            };

            std::string get_id() const { return _id; };
            void set_id(const std::string& id) { _id = id; };
            std::string preferred_file_extension() const { return HEADER; };

        private:
            static const unsigned int MAX_BYTE = (1 << CHAR_BIT);
        
            alex::hash::ICmph _ph;
            uint32_t _maxHash;

            alex::collections::PackedArray _pa;
            std::string _id;
    };

    const char PsiBool::HEADER[] = "psibool";
}

#endif
