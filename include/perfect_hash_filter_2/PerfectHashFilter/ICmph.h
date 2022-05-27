/**
 * @file ICmph.h
 * @author Alex Towell
 * @date March 2014
 *
 * C++ interface for cmph, a C library for constructing perfect hash functions.
 */

#ifndef __ICMPH_H__
#define __ICMPH_H__

#include "cmph\cmph.h"
#include "BinaryIO.h"
#include <cstdint>
#include <string>

namespace alex::hash
{
    class ICmph
    {
    public:
        typedef uint32_t size_type;

        ICmph() : _data(nullptr), _size(0) {};

        ICmph(const ICmph& copy) :
            _size(copy._size),
            _data(new unsigned char[copy._size])
        {
            std::memcpy(_data, copy._data, copy._size);
        };

        ICmph(ICmph&& src) :
            _size(src._size),
            _data(src._data)
        {
            src._size = 0;
            src._data = nullptr;
        };

        ICmph(char** vec, size_type size, bool verbose = true, float load_factor = 0.9f, cmph_cpp::CMPH_ALGO algo = cmph_cpp::CMPH_CHD_PH) :
            _size(0),
            _data(nullptr)
        {
            build(vec, size, verbose, load_factor, algo);
        };

        ICmph& operator=(const ICmph& other)
        {
            if (this != &other)
            {
                erase();

                _size = other._size;
                _data = new unsigned char[_size];
                std::memcpy(_data, other._data, other._size);
            }

            return *this;
        };

        ICmph& operator=(ICmph&& other)
        {
            if (this != &other)
            {
                erase();

                _size = other._size;
                _data = other._data;
                other._data = nullptr;
                other._size = 0;
            }

            return *this;
        };

        template <typename Iter>
        size_type build(Iter begin, Iter end, bool verbose = true, float load_factor = 0.9f, cmph_cpp::CMPH_ALGO aglo = cmph_cpp::CMPH_CHD_PH)
        {
            const auto& n = end - begin;
            char** tokens = new char*[n];
            size_type i = 0;
            for (auto it = begin; it != end; ++it)
            {
                tokens[i] = new char[it->size() + 1];
                std::memcpy(tokens[i], it->c_str(), it->size());
                tokens[i][it->size()] = 0;
            }
            size_type sz = build(tokens, n, verbose, load_factor, algo);
            for (size_type i = 0; i < n; ++i)
                delete [] tokens[i];
            delete [] tokens;

            return sz;
        }

        size_type build(char** data, size_type size, bool verbose = true, double load_factor = 0.9, cmph_cpp::CMPH_ALGO algo = cmph_cpp::CMPH_CHD_PH)
        {
            const auto source = cmph_cpp::cmph_io_vector_adapter(data, size);
            auto config = cmph_cpp::cmph_config_new(source);
            cmph_cpp::cmph_config_set_verbosity(config, verbose);
            cmph_cpp::cmph_config_set_graphsize(config, load_factor);
            cmph_cpp::cmph_config_set_algo(config, algo);
            const auto hash = cmph_cpp::cmph_new(config);

            erase();
            _size = cmph_cpp::cmph_packed_size(hash);
            _data = new unsigned char[_size];

            cmph_cpp::cmph_pack(hash, _data);
            cmph_cpp::cmph_config_destroy(config);
            const uint32_t hash_size = cmph_cpp::cmph_size(hash);

            cmph_cpp::cmph_io_vector_adapter_destroy(source);
            cmph_cpp::cmph_destroy(hash);

            return hash_size;
        };

        void erase()
        {
            if (_data != nullptr)
            {
                delete [] _data;
                _data = nullptr;
                _size = 0;
            }        
        };

        size_type size() const
        {
            return _size;
        };

        void* data()
        {
            return (void*)_data;
        };

        size_type operator()(const std::string& key) const
        {
            return operator()(key.c_str(), key.size());
        };

        size_type operator()(const char* key, unsigned int len) const
        {
            return cmph_cpp::cmph_search_packed(_data, key, len);
        };

        void write(std::ostream& file) const
        {
            if (_data == nullptr)
                throw std::exception("[ICmph::write(ostream)] Null Cmph");
            alex::io::binary::write_var_u32(file, _size);
            file.write((char*)_data, _size);
        };

        void read(std::istream& file)
        {
            erase();
    
            _size = alex::io::binary::read_var_u32(file);
            _data = new unsigned char[_size];
            file.read((char*)_data, _size);
        };
        
        virtual ~ICmph()
        {
            erase();
        };

    private:
        unsigned char* _data;
        size_type _size;
    };
}}

#endif