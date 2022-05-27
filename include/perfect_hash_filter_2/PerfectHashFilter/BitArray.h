#ifndef __BIT_ARRAY_H__
#define __BIT_ARRAY_H__

#include <iostream>
#include <sstream>
#include <ostream>
#include <cstdint>
#include <string>
#include <cstring>
#include "BinaryIO.h"
#include <iterator>

namespace alex::collections
{
    /* position of bit within character */
    #define BIT_CHAR(bit)         ((bit) / CHAR_BIT)



    /* array index for character containing bit */
    #define BIT_IN_CHAR(bit)      (1 << (CHAR_BIT - 1 - ((bit)  % CHAR_BIT)))

    /* number of characters required to contain number of bits */
    #define BITS_TO_CHARS(bits)   ((((bits) - 1) / CHAR_BIT) + 1)

    /* most significant bit in a character */
    #define MS_BIT                (1 << (CHAR_BIT - 1))

    /*
     * BitArray implements the bit array ADT.
     *
     * A p-bit array A consists of p bits. The bit at index i is denoted by A(i).
     * The following operations are available for bit arrays A and B:
     *
     *     - size(A), outputs the number of bits in A.
     *     - is_set(A, i), outputs true if A(i) is set to 1 and false otherwise.
     *
     *     - set(A, i), sets A(i) to 1.
     *     - clear(A, i), sets A(i) to 0.
     *     - negate(A, i), sets A(i) to 1 - A(i).
     *
     *     - set(A), sets A(i) to 1 for i = 1, ..., size(A).
     *     - clear(A), sets A(i) to 0 for i = 1, ..., size(A).
     *     - negate(A), sets A(i) to 1 - A(i) for i = 1, ..., size(A).
     *
     *     - binary_logic_operator(A, B), outputs a p-bit array C such
     *           that C(i) = binary_logic_operator(A(i), B(i)).
     *           A binary_logic_operator is any of the following:
     *               - and, or, xor, ...
     *                 (there are 2^4 = 16 possible such operators)
     */
    class BitArray
    {
    public:
        typedef unsigned char Byte;
        typedef unsigned int size_type;

        class Bit
        {
        public:
            Bit(const Bit& copy) : _index(copy._index), _bits(copy._bits) {};
            Bit(BitArray& bits, size_type index) : _index(index), _bits(bits) {};
            Bit& operator=(bool value) { _bits.set(_index, value); return *this; };
            bool operator==(bool value) const  { return is_set() == value; };
            bool operator==(const Bit& bit) const { return _index == bit._index && is_set() == bit.is_set(); };
            bool operator!=(bool value) const { return is_set() != value; };
            bool operator!=(const Bit& bit) const { return !(*this == bit); };
            Bit& clear() { return *this = false; };
            Bit& set() { return *this = true; };
            Bit& negate() { if (is_set()) clear(); else set(); return *this; };
            bool operator~() { return !is_set(); };
            bool operator!() { return !is_set(); };
            bool operator&(bool value) { return is_set() && value; };
            bool operator|(bool value) { return is_set() || value; };
            bool operator&&(bool value) { return is_set() && value; };
            bool operator||(bool value) { return is_set() || value; };
            bool operator^(bool value) { return (is_set() && !value) || (!is_set() && value); };
            Bit& operator^=(bool value) { return *this = *this ^ value; };
            Bit& operator&=(bool value) { return *this = is_set() && value; };
            Bit& operator|=(bool value) { return *this = is_set() || value; };
            size_type index() const { return _index; };
            bool is_set() const { return _bits.is_set(_index); };
            operator bool() const { return is_set(); };

        private:
            BitArray& _bits;
            size_type _index;
        };

        class BitIterator
        {
        public:
            BitIterator(BitArray& bits, size_type index) : _index(index), _bits(bits) {};
            BitIterator (const BitIterator& copy) : _bits(copy._bits), _index(copy._index) {}; 
            BitIterator& operator=(const BitIterator& other) { _bits = other._bits; _index = other._index; return *this; };
            BitIterator& operator++() { if (_index != _bits.size()) ++_index; return *this; };  // pre-increment
            BitIterator operator++(int) { auto it = *this; if (_index != _bits.size()) ++_index; return it; }; // post-increment
            Bit operator*() { return Bit(_bits, _index); }; // dereference eg: (*iter).set()

        private:
            BitArray& _bits;
            size_type _index;            
        };

        typedef BitIterator iterator;
        static const std::string HEADER;
        static const uint8_t VERSION = 1;

        // constructors and assignment
        BitArray(size_type bits) : _size(bits), _data(new Byte[BITS_TO_CHARS(bits)]) {};
        BitArray(size_type bits, bool value);
        BitArray(void* data, size_type bits);
        BitArray() : _size(0), _data(nullptr) {};
        BitArray(BitArray&& src);
        BitArray(const BitArray& copy);
        void copy(void* data, size_type bits);
        void move(void* data, size_type bits);
        virtual ~BitArray() { erase(); };
        BitArray& operator=(const BitArray& copy);
        BitArray& operator=(BitArray&& src);

        iterator begin() { return BitIterator(*this, 0); };
        iterator end() { return BitIterator(*this, _size); };

        // accessors
        size_type bytes() const { return BITS_TO_CHARS(_size); };
        std::string to_string() const;
        std::string to_hex_string() const;
        size_type size() const { return _size; };
        bool is_set(size_type bit) const;
        bool operator[](size_type bit) const { return is_set(bit); };
        bool operator==(const BitArray& other) const;
        bool operator!=(const BitArray& other) const { return !(*this == other); };

        // mutators
        Bit operator[](size_type bit) { return Bit(*this, bit); };
        Byte*& data() { return _data; };
        BitArray& resize(size_type bits);
        BitArray& clear_padding();
        BitArray& erase();
        BitArray& set_all();
        BitArray& clear_all();
        BitArray& negate_all();
        BitArray& set(size_type bit);
        BitArray& set(size_type bit, bool value) { return value ? set(bit) : clear(bit); };
        BitArray& clear(size_type bit);
        BitArray& negate(size_type bit);
        BitArray& operator&=(const BitArray& other);
        BitArray& operator^=(const BitArray& other);
        BitArray& operator|=(const BitArray& other);
        BitArray& operator<<=(size_type shifts);
        BitArray& operator>>=(size_type shifts);

        // serialization interface
        void read(std::istream& file);
        void write(std::ostream& file) const;

        // operators
        BitArray operator&(const BitArray& other) const { return BitArray(*this) &= other; };
        BitArray operator^(const BitArray& other) const { return BitArray(*this) ^= other; };
        BitArray operator|(const BitArray& other) const { return BitArray(*this) |= other; };
        BitArray operator~() const { return BitArray(*this).negate_all(); };
        BitArray operator<<(size_type shifts) const { return BitArray(*this) <<= shifts; };
        BitArray operator>>(size_type shifts) const { return BitArray(*this) >>= shifts; };

    private:
        size_type _size;
        Byte* _data;
    };

    const std::string BitArray::HEADER = "ba";

    void BitArray::read(std::istream& file)
    {
        const auto& hdr = alex::io::binary::read_string(file);
        if (hdr != HEADER)
            throw std::exception(("Unexpected Header: " + hdr).c_str());
        const auto version = alex::io::binary::read_u8(file);
        if (version != VERSION)
            throw std::exception(("Unexpected Version: " + std::to_string(version)).c_str());

        _size = alex::io::binary::read_var_u32(file);
        _data = new Byte[bytes()];
        file.read(reinterpret_cast<char*>(_data), bytes());
    }

    void BitArray::write(std::ostream& file) const
    {
        alex::io::binary::write_string(file, HEADER);
        alex::io::binary::write_u8(file, VERSION);
        alex::io::binary::write_var_u32(file, _size);
        file.write((char*)_data, bytes());
    }

    BitArray::BitArray(size_type bits, bool value) :
        _size(bits), _data(new Byte[BITS_TO_CHARS(bits)])
    {
        std::fill_n(_data, bytes(), value ? UCHAR_MAX : 0);
    }

    BitArray::BitArray(void* data, size_type bits) :
        _size(bits),
        _data(new Byte[BITS_TO_CHARS(bits)])
    {
        std::memcpy(_data, data, bytes());
    }

    BitArray::BitArray(const BitArray& copy) :
        _size(copy._size),
        _data(new Byte[copy.bytes()])
    {
        std::memcpy(_data, copy._data, copy.bytes());
    };

    BitArray::BitArray(BitArray&& other) :
        _data(other._data),
        _size(other._size)
    {
        other._data = nullptr;
        other._size = 0;
    }

    inline void BitArray::move(void* data, size_type bits)
    {
        erase();
        _data = static_cast<Byte*>(data);
        _size = bits;
    }

    inline void BitArray::copy(void* data, size_type bits)
    {
        erase();
        _size = bits;
        const size_type sz = bytes();
        _data = new Byte[sz];
        std::memcpy(_data, data, sz);
    }

    BitArray& BitArray::resize(size_type bits)
    {
        if (bits / CHAR_BIT == _size / CHAR_BIT)
        {
            _size = bits;
            return *this;
        }

        auto old_data = _data;
        const auto old_bytes = bytes();

        _size = bits;
        _data = new Byte[bytes()];
        if (old_data != nullptr)
        {
            std::memcpy(_data, old_data, std::min(old_bytes, bytes()));
            delete [] old_data;
        }
        return *this;
    }

    BitArray& BitArray::erase()
    {
        if (_data != nullptr)
        {
            delete [] _data;
            _data = nullptr;
            _size = 0;
        }
        return *this;
    }

    std::string BitArray::to_string() const
    {
        std::string result;
        for (size_type i = 0; i < _size; ++i)
        {
            if (i % CHAR_BIT == 0 && i != 0)
                result += " ";

            if (operator[](i)) { result += "1"; }
            else               { result += "0"; }
        }
        return result;
    }

    std::string BitArray::to_hex_string() const
    {
        if (_size == 0)
            return "";

        std::stringstream ss;

        auto copy = *this;
        copy.clear_padding();
        for (size_type i = 0; i < copy.bytes(); i++)
        {
            if (i != 0) ss << " ";
            ss.width(2);
            ss.fill('0');
            ss << std::hex << static_cast<int>(copy._data[i]);
        }

        return ss.str();
    }

    inline BitArray& BitArray::clear_padding()
    {
        const size_type sz = _size % CHAR_BIT;
        if (sz != 0)
        {
            Byte mask = UCHAR_MAX << (CHAR_BIT - sz);
            _data[BIT_CHAR(_size - 1)] = mask;
        }
        return *this;
    }

    inline BitArray& BitArray::set_all()
    {
        std::fill_n(_data, bytes(), UCHAR_MAX);
        return *this;
    }

    inline BitArray& BitArray::clear_all()
    {
        std::fill_n(_data, bytes(), 0);
        return *this;
    }

    inline BitArray& BitArray::set(size_type bit)
    {
        if (bit >= _size)
            throw std::exception("[BitArray::set(size_type)] Bit Out Of Bounds");
        _data[BIT_CHAR(bit)] |= BIT_IN_CHAR(bit);
        return *this;
    }

    inline BitArray& BitArray::clear(size_type bit)
    {
        if (bit >= _size)
            throw std::exception("[BitArray::clear(size_type] Bit Out Of Bounds");
        _data[BIT_CHAR(bit)] &= ~BIT_IN_CHAR(bit);
        return *this;
    }

    inline BitArray& BitArray::negate(size_type bit)
    {
        if (is_set(bit)) clear(bit);
        else set(bit);
        return *this;
    }

    inline bool BitArray::operator==(const BitArray& other) const
    {
        if (_size != other._size)
            return false;

        const size_type sz = bytes();
        for (size_type i = 0; i < sz; ++i)
            if (_data[i] != other._data[i]) return false;
        return true;
    }

    BitArray& BitArray::operator=(BitArray&& src)
    {
        if (this == &src)
            return *this;
        move(src._data, src._size);
        src._size = 0;
        src._data = nullptr;
        return *this;
    }

    BitArray& BitArray::operator=(const BitArray& copy)
    {
        if (this == &copy)
            return *this;
        this->copy(copy._data, copy._size);
        return *this;
    }

    BitArray& BitArray::operator&=(const BitArray& other)
    {
        if (_size != other._size)
            throw std::exception("Size Mismatch");

        for (size_type i = 0; i < BITS_TO_CHARS(_size); ++i) // logical-and array a Byte at a time
            _data[i] &= other._data[i];
        return *this;
    }

    BitArray& BitArray::operator^=(const BitArray& other)
    {
        if (_size != other._size)
            throw std::exception("Size Mismatch");

        for (size_type i = 0; i < BITS_TO_CHARS(_size); ++i) // logical-xor array a Byte at a time
            _data[i] ^= other._data[i];
        return *this;
    }

    BitArray& BitArray::operator|=(const BitArray& other)
    {
        if (_size != other._size)
            throw std::exception("Size Mismatch");

        for (size_type i = 0; i < BITS_TO_CHARS(_size); ++i) // logical-or array a Byte at a time
            _data[i] |= other._data[i];
        return *this;
    }

    BitArray& BitArray::negate_all()
    {
        if (_size == 0)
            return *this;

        for (size_type i = 0; i < BITS_TO_CHARS(_size); ++i) // logical-not array a Byte at a time
            _data[i] = ~_data[i];
        return *this;
    }

    BitArray& BitArray::operator<<=(size_type shifts)
    {
        int i;
        int chars = shifts / CHAR_BIT; // number of whole byte shifts

        if (shifts >= _size)
        {
            // all bits have been shifted off
            clear_all();
            return *this;
        }

        // first handle big jumps of bytes
        if (chars > 0)
        {
            for (i = 0; (i + chars) < BITS_TO_CHARS(_size); ++i)
                _data[i] = _data[i + chars];
            // now zero out new bytes on the right
            for (i = BITS_TO_CHARS(_size); chars > 0; --chars)
                _data[i - chars] = 0;
        }

        // now we have at most CHAR_BIT - 1 bit shifts across the whole array
        for (i = 0; i < static_cast<int>(shifts % CHAR_BIT); ++i)
        {
            for (int j = 0; j < BIT_CHAR(_size - 1); ++j)
            {
                _data[j] <<= 1;
                // handle shifts across byte bounds
                if (_data[j + 1] & MS_BIT)
                    _data[j] |= 0x01;
            }
            _data[BIT_CHAR(_size - 1)] <<= 1;
        }

        return *this;
    }

    BitArray& BitArray::operator>>=(size_type shifts)
    {
        if (shifts >= _size) // all bits have been shifted off
        {
            clear_all();
            return *this;
        }

        // first handle big jumps of bytes
        int i;
        int chars = shifts / CHAR_BIT;  // number of whole byte shifts

        if (chars > 0)
        {
            for (i = BIT_CHAR(_size - 1); (i - chars) >= 0; --i)
                _data[i] = _data[i - chars];
            // now zero out new bytes on the right
            for (; chars > 0; chars--)
                _data[chars - 1] = 0;
        }

        // now we have at most CHAR_BIT - 1 bit shifts across the whole array
        for (i = 0; i < static_cast<int>(shifts % CHAR_BIT); ++i)
        {
            for (unsigned int j = BIT_CHAR(_size - 1); j > 0; --j)
            {
                _data[j] >>= 1;
                // handle shifts across byte bounds
                if(_data[j - 1] & 0x01)
                    _data[j] |= MS_BIT;
            }
            _data[0] >>= 1;
        }

        i = _size % CHAR_BIT;
        if (i != 0)
        {
            Byte mask = UCHAR_MAX << (CHAR_BIT - i);
            _data[BIT_CHAR(_size - 1)] &= mask;
        }

        return *this;
    }

    bool BitArray::is_set(size_type bit) const
    {
        if (bit >= _size)
            throw std::exception("[BitArray::is_set(size_type)] Bit Out Of Bounds");
        return (_data[BIT_CHAR(bit)] & BIT_IN_CHAR(bit)) != 0;
    };
 }

#endif