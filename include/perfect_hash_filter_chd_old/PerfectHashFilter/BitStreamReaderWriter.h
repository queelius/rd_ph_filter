#ifndef __BIT_STREAM_READER_WRITER_H__
#define __BIT_STREAM_READER_WRITER_H__

#include <iostream>
#include <fstream>

namespace alex::io
{
    class BitStreamWriter
    {
    public:
        BitStreamWriter(std::ostream& s) : _s(s), _bit_buffer(0), _bit_count(0) {};

        int byte_align(void)
        {
            int v = _bit_buffer;
            if (_bit_count != 0)
            {
                _bit_buffer <<= 8 - (_bit_count);
                _s.put(_bit_buffer);
            }

            _bit_buffer = _bit_count = 0;
            return v;
        };

        int flush_output(unsigned char ones_fill)
        {
            int v = -1;
            if (_bit_count != 0)
            {
                _bit_buffer <<= (8 - _bit_count);
                if (ones_fill)
                    _bit_buffer |= (0xFF >> _bit_count);

                _s.put(_bit_buffer);
                v = _bit_buffer;
            }

            _bit_buffer = _bit_count = 0;
            return v;
        };

        void put_char(int c)
        {
            if (_bit_count == 0)
            {
                _s.put(c);
            }
            else
            {
                int tmp = (c & 0xFF) >> _bit_count;
                tmp = tmp | ((_bit_buffer) << (8 - _bit_count));
                _s.put((char)tmp);
                _bit_buffer = (char)c;
            }
        };

        void put_bit(bool c)
        {
            _bit_count++;
            _bit_buffer <<= 1;

            if (c != 0)
            {
                _bit_buffer |= 1;
            }

            if (_bit_count == 8)
            {
                _s.put(_bit_buffer);
                _bit_count = 0;
                _bit_buffer = 0;
            }
        };

    private:
        std::ostream& _s;
        char _bit_buffer;
        unsigned char _bit_count;
    };

    class BitStreamReader
    {
    public:
        BitStreamReader(std::istream& s) : _s(s), _bit_buffer(0), _bit_count(0) {};

        int get_char(void)
        {
            int v = _s.get();

            if (_bit_count == 0)
                return v;

            int tmp = (v >> _bit_count);
            tmp |= _bit_buffer << (8 - _bit_count);
            _bit_buffer = (char)v;

            return tmp & 0xFF;
        };

        int get_bit(void)
        {
            int v;
            if (_bit_count == 0)
            {
                v = _s.get();
                _bit_count = 8;
                _bit_buffer = v;
            }

            _bit_count--;
            v = _bit_buffer >> _bit_count;

            return v & 0x01;
        };

    private:
        std::istream& _s;
        char _bit_buffer;
        unsigned char _bit_count;
    };
}

#endif
