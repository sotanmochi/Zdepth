// This code is licensed under the MIT License. Copyright (c) 2020 Soichiro Sugimoto.

#pragma once

#include <vector>
#include "zdepth.hpp"

namespace zdepth
{
    const int TEMPORAL_ZDEPTH_HEADER_SIZE = 2;

    struct Pixel
    {
    public:
        short value;
        short invalidCount;
    };

    class TemporalZdepthCompressor
    {
    public:
        TemporalZdepthCompressor(int width, int height, short changeThreshold, int invalidThreshold);
        int Compress(const uint16_t* depth, uint8_t* compressedData, bool keyFrame);

    private:
        int _width, _height;
        std::vector<Pixel> _pixels;
        short _changeThreshold;
        int _invalidThreshold;
        DepthCompressor _compressor;
    };

    class TemporalZdepthDecompressor
    {
    public:
        TemporalZdepthDecompressor(int width, int height);
        DepthResult Decompress(int compressedSize, uint8_t* compressedData, uint16_t* depth, bool keyFrame);

    private:
        int _width, _height;
        DepthCompressor _decompressor;
    };
}