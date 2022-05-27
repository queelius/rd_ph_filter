/**
 * @file ICmph.h
 * @author Alex Towell
 * @date March 2014
 *
 * C++ interface for cmph, a C library for constructing perfect hash functions.
 */

#ifndef __CMPH_H__
#define __CMPH_H__

#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include "BinaryIO.h"
#include "cmph.h"
//#include "IPerfectHash.h"

namespace alex::hash
{
    class CMPHa//: public IPerfectHash<std::string>
    {
    public:
//        using IPerfectHash<std::string>::cardinality;
 //       using IPerfectHash<std::string>::size_type;

        CMPHa() : _buffer(nullptr), _size(0) {};

        CMPHa(const CMPHa& copy) :
            _size(copy._size),
            _buffer(new unsigned char[copy._size])
        {
            ::std::memcpy(_buffer, copy._buffer, copy._size);
        };

        CMPHa(CMPHa&& src) :
            _size(src._size),
            _buffer(src._buffer)
        {
            src._size = 0;
            src._buffer = nullptr;
        };

        CMPHa(char** data, size_t size, double load_factor = 0.9, CMPH_ALGO algo = CMPH_CHD_PH, bool verbose = false) :
            _size(0),
            _buffer(nullptr)
        {
            build(data, size, load_factor, algo, verbose);
        };

        CMPHa(const std::vector<std::string>& data, double load_factor = 0.9, CMPH_ALGO algo = CMPH_CHD_PH, bool verbose = false) :
            _size(0),
            _buffer(nullptr)
        {
            build(data, load_factor, algo, verbose);
        };

        CMPHa& operator=(const CMPHa& other)
        {
            if (this != &other)
            {
                clear();

                _size = other._size;
                _buffer = new unsigned char[_size];
                std::memcpy(_buffer, other._buffer, other._size);
            }

            return *this;
        };

        Cmph& operator=(Cmph&& other)
        {
            if (this != &other)
            {
                clear();

                _size = other._size;
                _buffer = other._buffer;
                other._buffer = nullptr;
                other._size = 0;
            }

            return *this;
        };

        void build(const std::vector<std::string>& data, double load_factor = 0.9, CMPH_ALGO algo = CMPH_CHD_PH, bool verbose = true)
        {
            std::vector<char*> chars;
            chars.reserve(data.size());

            for (const auto& x : data)
                chars.push_back(const_cast<char*>(x.c_str()));
            return build(&chars[0], data.size(), load_factor, algo, verbose);
        }

        void build(char** data, size_t size, double load_factor = 0.9, CMPH_ALGO algo = CMPH_CHD_PH, bool verbose = true)
        {
            const auto source = cmph_io_vector_adapter(data, size);
            auto config = cmph_config_new(source);
            cmph_config_set_verbosity(config, verbose);
            cmph_config_set_graphsize(config, load_factor);
            cmph_config_set_algo(config, algo);
            const auto hash = cmph_new(config);

            clear();
            _keys = size;
            _size = cmph_packed_size(hash);
            _buffer = new unsigned char[_size];

            cmph_pack(hash, _buffer);
            cmph_config_destroy(config);
            _max_hash = cmph_size(hash);

            cmph_io_vector_adapter_destroy(source);
            cmph_destroy(hash);
        };

        uint32_t max_hash() const
        {
            return _max_hash;
        };

        void clear()
        {
            if (_buffer != nullptr)
            {
                delete [] _buffer;
                _buffer = nullptr;
                _size = 0;
                _keys = 0;
            }        
        };

        void* buffer()
        {
            return (void*)_buffer;
        };

        size_t find(const std::string& key) const
        {
            return cmph_search_packed(_buffer, key.c_str(), key.size());
        };

        size_t find(const char* key, unsigned int len) const
        {
            return cmph_search_packed(_buffer, key, len);
        };

        size_t cardinality() const
        {
            return _keys;
        };

        void write(std::ostream& file) const
        {
            if (_buffer == NULL)
            {
                throw std::exception("Empty CMPH");
            }

            alex::io::binary::write_var_u32(file, _size);
            file.write((char*)_buffer, _size);
        };

        void read(std::istream& file)
        {
            clear();
    
            _size = alex::io::binary::read_var_u32(file);
            _buffer = new unsigned char[_size];
            file.read((char*)_buffer, _size);
        };
        
        virtual ~CMPHa()
        {
            clear();
        };

        double load_factor() const
        {
            return static_cast<double>(_keys) / _max_hash;
        };

        double byte_length() const
        {
            return _size;
        };

    private:
        unsigned char* _buffer;
        uint32_t _size, _max_hash, _keys;
    };
}

#endif