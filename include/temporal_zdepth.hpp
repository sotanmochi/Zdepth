// This code is licensed under the MIT License. Copyright (c) 2020 Soichiro Sugimoto.

#pragma once

#include <vector>
#include "zdepth.hpp"

namespace zdepth
{
    struct Pixel
    {
    public:
        uint16_t value;
        uint16_t invalidCount;
    };

    class TemporalZdepthCompressor
    {
    public:
        TemporalZdepthCompressor(int width, int height, uint16_t changeThreshold, int invalidThreshold);
        int Compress(const uint16_t* depth, uint8_t* compressedData, bool keyframe);

    private:
        int _width, _height;
        std::vector<Pixel> _pixels;
        uint16_t _changeThreshold;
        int _invalidThreshold;
        DepthCompressor _compressor;
    };

    class TemporalZdepthDecompressor
    {
    public:
        TemporalZdepthDecompressor(int width, int height);
        DepthResult Decompress(int compressedSize, const uint8_t* compressedData, uint16_t* depth, bool keyFrame);

    private:
        int _width, _height;
        std::vector<uint16_t> _pixelDiffs;
        DepthCompressor _decompressor;
    };
}