#ifndef __BIT_ARRAY_COUNTER_H__
#define __BIT_ARRAY_COUNTER_H__

#include "BitArray.h"
#include <iterator>

namespace alex::collections
{
    class BitArrayCounterIterator;

    class BitArrayCounter
    {
    public:
        typedef BitArray::size_type size_type;

        BitArrayCounter(const BitArray& ba) : _ba(ba) {};
        BitArrayCounter(size_type size) : _ba(size) {};

        size_type size() const { return _ba.size(); };

        operator BitArray() const { return _ba; };

        void increment()
        {
            if (size() == 0)
                return;

            BitArray::BitBlock max_value; // maximum value for current bit block
            BitArray::BitBlock one;       // least significant bit in current bit block

            // handle BitArrayCounter instances that don't use every bit in the last BitBlock
            int i = size() % CHAR_BIT;
            if (i != 0)
            {
                max_value = UCHAR_MAX << (CHAR_BIT - i);
                one = 1 << (CHAR_BIT - i);
            }
            else
            {
                max_value = UCHAR_MAX;
                one = 1;
            }

            for (i = BIT_CHAR(size() - 1); i >= 0; --i)
            {
                if (_ba.data()[i] != max_value)
                {
                    _ba.data()[i] += one;
                    break;
                }
                else
                {
                    _ba.data()[i] = 0;      // need to carry to next byte
                    max_value = UCHAR_MAX;  // remaining characters must use all bits
                    one = 1;
                }
            }
        };

        void decrement()
        {
            if (size() == 0)
                return;

            BitArray::BitBlock max_value; // maximum value for current BitBlock
            BitArray::BitBlock one;       // least significant bit in current BitBlock

            // handle BitArrayCounter instances that don't use every bit in the last character
            int i = size() % CHAR_BIT;
            if (i != 0)
            {
                max_value = UCHAR_MAX << (CHAR_BIT - i);
                one = 1 << (CHAR_BIT - i);
            }
            else
            {
                max_value = UCHAR_MAX;
                one = 1;
            }

            for (i = BIT_CHAR(size() - 1); i >= 0; --i)
            {
                if (_ba.data()[i] >= one)
                {
                    _ba.data()[i] -= one;
                    break;
                }
                else
                {
                    _ba.data()[i] = max_value;  // need to borrow from the next byte
                    max_value = UCHAR_MAX;      // remaining characters must use all bits
                    one = 1;
                }
            }
        }

        typedef BitArrayCounterIterator iterator;

    private:
        BitArray _ba;
    };

    class BitArrayCounterIterator
    {
    public:
        BitArrayCounterIterator (const BitArrayCounterIterator& copy) : _bac(copy._bac) {}; 
        BitArrayCounterIterator& operator=(const BitArrayCounterIterator& other) { _bac = other._bac; return *this; };
        BitArrayCounterIterator& operator++() { _bac.increment(); return *this; };  // pre-increment
        BitArrayCounterIterator& operator--() { _bac.decrement(); return *this; };  // pre-decrement
        BitArrayCounterIterator operator++(int) { auto it = *this; _bac.increment(); return it; }; // post-increment
        BitArrayCounterIterator operator--(int) { auto it = *this; _bac.decrement(); return it; }; // post-decrement
        BitArray& operator*() { return static_cast<BitArray>(_bac); }; // dereference eg: (*iter).set(size_type, bool)

    private:
        BitArrayCounter _bac;
    };
}

#endif