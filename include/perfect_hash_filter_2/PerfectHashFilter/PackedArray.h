#ifndef __PACKED_ARRAY_H__
#define __PACKED_ARRAY_H__

#include "BitArray.h"
#include "IBinaryCodec.h"

using alex::codec::IBinaryCodec;

namespace alex::collections
{
    template <typename T>
    class PackedArrayIterator;
    
    template <typename T>
    class PackedArray
    {
    public:
        static const std::string HEADER;
        static const uint8_t VERSION = 1;

        typedef typename IBinaryCodec<T>::binary_code binary_code;
        typedef BitArray::size_type size_type;
        typedef typename PackedArrayIterator<T> iterator;

        template <typename Iter>
        PackedArray(Iter begin, Iter end, IBinaryCodec<T>* codec) : _codec(codec)
        {
            if (_codec == nullptr)
                throw std::invalid_argument("[PackedArray<T>::ctor(Iter, Iter, PackedArrayCodec)] No PackedArrayCodec<T>");

            size_type index = 0;
            _ba.resize((begin - end) * value_bit_length());
            for (auto iter = begin; iter != end; ++iter)
                operator[](index++) = *iter;
        };

        PackedArray(size_type size, IBinaryCodec<T>* codec) : _codec(codec) {};
        PackedArray(const BitArray& ba, IBinaryCodec<T>* codec) : _ba(ba), _codec(codec) {};
        iterator begin() const { return iterator(this, 0); };
        iterator end() const { return iterator(this, size()); };
        PackedArray& resize(size_type size) { _ba.resize(size * encoded_bit_length()); return *this; };
        size_type value_bit_length() const { return _codec.bit_length(); };
        size_type bit_length() const { return _ba.size(); };
        size_type size() const { return bit_length() / value_bit_length(); };

        void write(std::ostream& file) const
        {
            alex::io::binary::write_string(file, HEADER);
            alex::io::binary::write_u8(file, VERSION);
            _codec->write(file);
            _ba.write(file);            
        };

        void read(std::ostream& file)
        {
            const auto& hdr = alex::io::binary::read_string(file);
            if (hdr != HEADER)
                throw std::exception(("[PackedArray<T>::read(istream)] Unexpected Header: " + hdr).c_str());
            const auto version = alex::io::binary::read_u8(file);
            if (version != VERSION)
                throw std::exception(("[PackedArray<T>::read(ostream)] Unexpected Version: " + std::to_string(version)).c_str());

            _codec->read(file);
            _ba.read(file);
        }

        template <typename U>
        class Value
        {
        public:
            Value(PackedArray<T>* pa, size_type bit_index) : _pa(pa), _bit_index(bit_index) {};
            operator T() const { return get(); };
            operator Code() const { return get_code(); };

            Code get_code() const
            {
                Code code(_pa->bit_length());
                for (size_type i = 0; i < _pa->bit_length(); ++i)
                    code[i] = _pa->_ba.is_set(bit_index + i);
                return code;
            };

            U get() const { return _pa->_codec->decode(_pa->_ba, _bit_index); };

            Value<U>& set_code(const Code& code)
            {
                for (size_type i = 0; i < _pa->bit_length(); ++i)
                    _pa->_ba.set(bit_index + i, code[i]);
                return *this;
            };

            Value<T>& set(const T& value) { _pa->_codec->encode(value, _pa->_ba, _bit_index); return *this; };
            Value<T>& operator=(const T& value) { return set(value); };
            Value<T>& operator=(const Code& code) { return set(code); };

        private:
            PackedArray<T>* _pa;
            size_type _bit_index;
        };

        const IBinaryCodec<T> const* codec() const { return _codec; };

        operator BitArray() const { return _ba; };

        T operator[](size_type index) const
        {
            const auto bit_index = index * value_bit_length();
            if (bit_index > bit_length())
                throw std::exception("[PackedArray::operator[](size_type) const] Index Out Of Bounds");
            return _codec->decode(_ba, bit_index);
        };

        Value<T> operator[](size_type index)
        {
            const auto bit_index = index * value_bit_length();
            if (bit_index > bit_length())
                throw std::exception("[PackedArray::operator[](size_type)] Index Out Of Bounds");
            return Value<T>(this, bit_index);
        };

    protected:
        PackedArrayCodec<T>* _codec;
        BitArray _ba;
    };

    template <typename T>
    class PackedArrayIterator
    {
    public:
        PackedArrayIterator (const PackedArrayIterator<T>& copy) : _pa(copy._pa), _index(copy._index) {}; 

        PackedArrayIterator& operator=(const PackedArrayIterator<T>& other) { _pa = other._pa; _index = other._index; return *this; };
        PackedArrayIterator& operator++() { if (_index != _pa->size()) ++_index; return *this; };
        PackedArrayIterator& operator--() { if (_index != 0) --_index; return *this; };
        PackedArrayIterator operator++(int) { auto it = *this; if (_index != _pa->size()) ++_index; return it; };
        PackedArrayIterator operator--(int) { auto it = *this; if (_index != 0) --_index; return it; };

        typename PackedArray<T>::Value<T> operator*() { return _pa->operator[](_index); }; // dereference eg: Value<T> v = *iter;

    private:
        typename PackedArray<T>::size_type _index;
        PackedArray<T>* _pa;
    };

}

#endif