#ifndef __BIT_MATRIX_H__
#define __BIT_MATRIX_H__

#include "BitArray.h"
#include "BinaryIO.h"

namespace alex::collections
{
    /*
     * BitMatrix implements the bit matrix ADT. BitMatrix stores bits in
     * row-major order.
     *
     * A (q x p)-bit matrix A consists of q rows and p columns of q * p bits.
     * The bit at row i, column j of bit matrix A is denoted by a(i,j).
     * The following operations are available for bit matrices A and B:
     *
     *     - A.get(i, j) -> Bit, returns a Bit representing a(i,j).
     *           A Bit is an implementation of the bit ADT.
     *     - A.get(i, j) const -> bool, returns a boolean value representing a(i,j),
     *           i.e., returns true if a(i,j) = 1 and false if a(i,j) = 0.
     *     - A.rows() const -> size_type, returns number of rows of A.
     *     - A.columns() const -> size_type, returns number of columns of A.
     *     - A | B -> BitMatrix, logical OR returns a (q x p) bit matrix C such
     *           that c(i,j) = a(i,j) | b(i,j).
     *     - A & B -> BitMatrix, logical AND returns a (q x p) bit matrix C such
     *           that c(i,j) = a(i,j) & b(i,j).
     *     - A ^ B -> BitMatrix, logical XOR returns a (q x p) bit matrix C such
     *           that c(i,j) = a(i,j) ^ b(i,j).
     *     - ~A -> BitMatrix, logical NOT returns a (q x p) bit matrix B such
     *           that b(i,j) = ~a(i,j).
     *     - A.set_all() -> void, sets all bits in A to 1.
     *     - A.clear_all() -> void, sets all bits in A to 0.
     */
    class BitMatrix
    {
    public:
        static const std::string HEADER;
        static const uint8_t VERSION = 1;

        typedef BitArray::size_type size_type;
        typedef BitArray::Bit Bit;
        typedef std::vector<bool> Bits;

        class Row
        {
        public:
            Row(size_type row, BitMatrix* bm) : _bm(bm), _row(row) {};

            size_type row() const { return _row; };
            Row& operator=(const Row& copy)
            {

            };

            Row& operator=(const Bits& bits)
            {

            };

            operator Bits() const
            {
            };

        private:
            BitMatrix* _bm;
            size_type _row;
        };

        typedef std::vector<Bit> RowBits;
        typedef std::vector<Bit> ColumnBits;

        BitMatrix(const BitArray& ba, size_type columns) : _ba(ba), _cols(columns) {};
        BitMatrix(const BitMatrix& copy) : _ba(copy._ba), _cols(copy._cols) {};
        BitMatrix(const BitMatrix&& src) : _ba(std::move(src._ba)), _cols(src._cols) {};

        BitMatrix& operator=(const BitMatrix& copy) { _ba = copy._ba; _cols = copy._cols; };
        BitMatrix& operator=(BitMatrix&& src) { _ba = std::move(src._ba); _cols = src._cols; };

        bool operator==(const BitMatrix& other) { return _ba == other._ba && _cols == other._cols; };
        bool operator!=(const BitMatrix& other) { return _ba != other._ba || _cols != other._cols; };

        Row get_row(size_type row)
        {
        };

        Column get_column(size_type column)
        {
        };

        Bit get(size_type row, size_type column) { return _ba[row * _cols + column]; };
        bool get(size_type row, size_type column) const { return _ba[row * _cols + column]; };
        Bit operator()(size_type row, size_type column) { return get(row, column); };
        bool operator()(size_type row, size_type column) const { return get(row, column); };

        operator BitArray() { return _ba; };

        size_type size() const { return _ba.size(); };
        BitMatrix& set_all() { _ba.set_all(); return *this; };
        BitMatrix& clear_all() { _ba.clear_all(); return *this; };
        size_type rows() const { return _ba.size() / _cols; };
        size_type columns() const { return _cols; };

        void read(std::istream& file)
        {
            const auto& hdr = alex::io::binary::read_string(file);
            if (hdr != HEADER)
                throw std::exception(("Unexpected Header: " + hdr).c_str());

            auto version = alex::io::binary::read_u8(file);
            if (version != VERSION)
                throw std::exception(("Unexpected Version: " + std::to_string(version)).c_str());

            _cols = alex::io::binary::read_var_u32(file);
            _ba.read(file);
        };

        void write(std::ostream& file) const
        {
            alex::io::binary::write_string(file, HEADER);
            alex::io::binary::write_u8(file, VERSION);
            alex::io::binary::write_var_u32(file, _cols);
            _ba.write(file);
        };

        BitMatrix operator&(const BitMatrix& other) const
        {
            return BitMatrix(_ba & other._ba, _cols);
        };

        BitMatrix operator|(const BitMatrix& other) const
        {
            return BitMatrix(_ba | other._ba, _cols);
        };

        BitMatrix operator^(const BitMatrix& other) const
        {
            return BitMatrix(_ba ^ other._ba, _cols);
        };

        BitMatrix operator~() const
        {
            return BitMatrix(~_ba, _cols);
        };

        BitMatrix& operator&=(const BitMatrix& other)
        {
            _ba &= other._ba;
            return *this;
        };

        BitMatrix& operator|=(const BitMatrix& other)
        {
            _ba |= other._ba;
            return *this;
        };

        BitMatrix& operator^=(const BitMatrix& other)
        {
            _ba ^= other._ba;
            return *this;
        };

    private:
        uint32_t _cols;
        BitArray _ba;
    };
}

#endif