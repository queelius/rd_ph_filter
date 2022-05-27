/**
 * @file BinaryIO
 * @author Alex Towell (atowell@siue.edu)
 *
 * A binary IO module for reading and writing to binary streams. (serialization interface)
 */

#ifndef __BINARY_IO_H__
#define __BINARY_IO_H__

#include <string.h>
#include <vector>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <inttypes.h>

namespace alex::io::binary
{
    /*************
     * Interface *
     *************/

    #define pack754_32(f) (pack_754((f), 32, 8))
    #define pack754_64(f) (pack_754((f), 64, 11))
    #define unpack754_32(i) (unpack_754((i), 32, 8))
    #define unpack754_64(i) (unpack_754((i), 64, 11))    

    int32_t     read_int32(std::istream& file);
    void        write_int32(std::ostream& file, int32_t value);
    int32_t     read_int16(std::istream& file);
    void        write_int16(std::ostream& file, int16_t value);
    int8_t      read_int8(std::istream& file);
    void        write_int8(std::ostream& file, int8_t value);


    long double unpack_754(uint64_t i, unsigned bits, unsigned expbits);
    uint64_t    pack_754(long double f, unsigned bits, unsigned expbits);

    float       read_float(std::istream& file);
    void        write_float(std::ostream& file, float value);
    double      read_double(std::istream& file);
    void        write_double(std::ostream& file, double value);

    uint8_t     read_u8(std::istream& file);
    void        write_u8(std::ostream& file, uint8_t value);

    uint16_t    read_u16(std::istream& file);
    void        write_u16(std::ostream& file, uint16_t value);

    uint32_t    read_u32(std::istream& file);
    void        write_u32(std::ostream& file, uint32_t value);

    std::string read_string(std::istream& file);
    void        write_string(std::ostream& file, const std::string& str);

    void        write_vec_u32(std::ostream& file, const std::vector<uint32_t>& values);
    void        read_vec_u32(std::istream& file, std::vector<uint32_t>& values);

    void        write_array_u8(std::ostream& file, uint32_t size, uint8_t* data);
    uint32_t    read_array_u8(std::istream& file, uint8_t*& data);

    void        write_var_u32(std::ostream& file, uint32_t value);
    uint32_t    read_var_u32(std::istream& file);

    void        write_u64(std::ostream& file, uint64_t value);
    uint64_t    read_u64(std::istream& file);
    
    /******************
     * Implementation *
     ******************/
    float read_float(std::istream& file)
    {
        return static_cast<float>(unpack754_32(read_u32(file)));
    }
    
    double read_double(std::istream& file)
    {
        return static_cast<double>(unpack754_64(read_u64(file)));
    }   

    void write_float(std::ostream& file, float value)
    {
        write_u32(file, static_cast<uint32_t>(pack754_32(value)));
    }
    
    void write_double(std::ostream& file, double value)
    {
        write_u64(file, static_cast<uint64_t>(pack754_64(value)));
    }

    void write_var_u32(std::ostream& file, uint32_t value)
    {
        while (value > 127)
        {
            write_u8(file, (value & 127) | 128);
            value >>= 7;
        }
        write_u8(file, value & 127);
    }

    uint32_t read_var_u32(std::istream& file)
    {
        uint32_t result = 0;
        for (uint8_t i = 0; i < 4; i++)
        {
            uint8_t tmp = read_u8(file);
            result |= (tmp & 127) << (7*i);
            if(!(tmp & 128))
                break;
        }
        return result;
    }

    void write_array_u8(std::ostream& file, uint32_t size, uint8_t* data)
    {
        write_var_u32(file, size);
        for (uint32_t i = 0; i < size; ++i)
            write_u8(file, data[i]);
    }

    uint32_t read_array_u8(std::istream& file, uint8_t*& data)
    {
        const uint32_t size = read_var_u32(file);
        data = new uint8_t[size];
        for (uint32_t i = 0; i < size; ++i)
            data[i] = read_u8(file);
        return size;
    }

    int32_t read_int32(std::istream& file)
    {
        uint8_t bytes[4];
        file.read((char*)bytes, 4); // read 4 bytes from the file
    
        // construct the 32-bit value from those bytes
        return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
    }

    void write_int32(std::ostream& file, int32_t value)
    {
        uint8_t bytes[4];

        // extract the individual bytes from our value
        bytes[0] = (value >> 24) & 0xFF;
        bytes[1] = (value >> 16) & 0xFF;
        bytes[2] = (value >> 8) & 0xFF;
        bytes[3] = value & 0xFF;

        // write those bytes to the file
        file.write((char*)bytes, 4);
    }

    int32_t read_int16(std::istream& file)
    {
        uint8_t bytes[2];
        file.read((char*)bytes, 2); // read 2 bytes from the file
        // construct the 16-bit value from those bytes
        return (bytes[0] << 8) | bytes[1];
    }

    void write_int16(std::ostream& file, int16_t value)
    {
        uint8_t bytes[2];

        // extract the individual bytes from our value
        bytes[0] = (value >> 8) & 0xFF;
        bytes[1] = value & 0xFF;

        // write those bytes to the file
        file.write((char*)bytes, 2);
    }

    int8_t read_int8(std::istream& file)
    {
        char byte;
        file.read(&byte, 1); // read 1 byte from the file
    
        return byte;
    }

    void write_int8(std::ostream& file, int8_t value)
    {
        // write those bytes to the file
        file.write((char*)(&value), 1);
    }

    uint8_t read_u8(std::istream& file)
    {
        uint8_t value;
        // read 1 byte from the file
        file.read((char*)(&value), 1);
        return value;
    }

    void write_u8(std::ostream& file, uint8_t value)
    {
        // write 1 byte to the file
        file.write((char*)(&value), 1);
    }

    uint16_t read_u16(std::istream& file)
    {
        uint8_t bytes[2];
        // read 2 bytes from the file
        file.read((char*)bytes, 2);
    
        // construct the 16-bit value from those bytes
        return (bytes[0] << 8) | bytes[1];
    }

    void write_u16(std::ostream& file, uint16_t value)
    {
      uint32_t bytes[2];

      // extract the individual bytes from our value
      bytes[0] = (value >> 8) & 0xFF;   // high byte
      bytes[1] = value & 0xFF;          // low byte

      // write those bytes to the file
      file.write((char*)bytes, 2);
    }

    uint32_t read_u32(std::istream& file)
    {
        uint8_t bytes[4];
        file.read((char*)bytes, 4); // read 4 bytes from the file
    
        // construct the 32-bit value from those bytes
        return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
    }

    void write_u32(std::ostream& file, uint32_t value)
    {
      uint8_t bytes[4];

      // extract the individual bytes from our value
      bytes[0] = (value >> 24) & 0xFF;
      bytes[1] = (value >> 16) & 0xFF;
      bytes[2] = (value >> 8) & 0xFF;
      bytes[3] = value & 0xFF;

      // write those bytes to the file
      file.write((char*)bytes, 4);
    }

    uint64_t read_u64(std::istream& file)
    {
        uint64_t value;
        uint8_t bytes[8];

        file.read((char*)bytes, 8); // read 4 bytes from the file
    
        // construct the 64-bit value from those bytes
        value = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
        value <<= 32;
        value |= (bytes[4] << 24) | (bytes[5] << 16) | (bytes[6] << 8) | bytes[7];
        return value;
    }

    void write_u64(std::ostream& file, uint64_t value)
    {
      uint8_t bytes[8];

      // extract the individual bytes from our value
      bytes[0] = (value >> 56) & 0xFF;
      bytes[1] = (value >> 48) & 0xFF;
      bytes[2] = (value >> 40) & 0xFF;
      bytes[3] = (value >> 32) & 0xFF;
      bytes[4] = (value >> 24) & 0xFF;
      bytes[5] = (value >> 16) & 0xFF;
      bytes[6] = (value >> 8) & 0xFF;
      bytes[7] = value & 0xFF;

      // write those bytes to the file
      file.write((char*)bytes, 8);
    }

    std::string read_string(std::istream& file)
    {
        const uint32_t len = read_var_u32(file);

        char* buf = new char[len];
        file.read(buf, len);

        return std::string(std::move(buf), len);
    }

    void write_string(std::ostream& file, const std::string& str)
    {
        const uint32_t len = str.length();

        write_var_u32(file, len);
        file.write(str.c_str(), len);
    }

    uint64_t pack_754(long double f, unsigned bits, unsigned expbits)
    {
        long double fnorm;
        int shift;
        long long sign, exp, significand;
        unsigned significandbits = bits - expbits - 1; // -1 for sign bit

        if (f == 0.0) return 0; // get this special case out of the way

        // check sign and begin normalization
        if (f < 0) { sign = 1; fnorm = -f; }
        else { sign = 0; fnorm = f; }

        // get the normalized form of f and track the exponent
        shift = 0;
        while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
        while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
        fnorm = fnorm - 1.0;

        // calculate the binary form (non-float) of the significand data
        significand = (long long)(fnorm * ((1LL<<significandbits) + 0.5f));

        // get the biased exponent
        exp = shift + ((1<<(expbits-1)) - 1); // shift + bias

        // return the final answer
        return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
    }

    long double unpack_754(uint64_t i, unsigned bits, unsigned expbits)
    {
        long double result;
        long long shift;
        unsigned bias;
        unsigned significandbits = bits - expbits - 1; // -1 for sign bit

        if (i == 0) return 0.0;

        // pull the significand
        result = (long double)(i&((1LL<<significandbits)-1)); // mask
        result /= (1LL<<significandbits); // convert back to float
        result += 1.0f; // add the one back on

        // deal with the exponent
        bias = (1<<(expbits-1)) - 1;
        shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
        while(shift > 0) { result *= 2.0; shift--; }
        while(shift < 0) { result /= 2.0; shift++; }

        // sign it
        result *= (i>>(bits-1))&1? -1.0: 1.0;

        return result;
    }
}

#endif