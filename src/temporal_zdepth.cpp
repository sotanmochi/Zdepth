// This code is licensed under the MIT License. Copyright (c) 2020 Soichiro Sugimoto.

#include "temporal_zdepth.hpp"

namespace zdepth
{
    short AbsDiff(short x, short y)
    {
        if (x > y) { return x - y; }
        else { return y - x; }
    }

    void UpdatePixel(Pixel& pixel, short rawValue, short changeThreshold, int invalidationThreshold)
    {
        if (pixel.value == 0)
        {
            if (rawValue > 0)
            {
                pixel.value = rawValue;
            }
            return;
        }

        // Reset the pixel if the depth value indicates the input was invalid two times in a row.
        if (rawValue == 0)
        {
            ++pixel.invalidCount;
            if (pixel.invalidCount >= invalidationThreshold)
            {
                pixel.value = 0;
                pixel.invalidCount = 0;
            }
            return;
        }
        pixel.invalidCount = 0;

        // Update pixel value when change is detected.
        if (AbsDiff(pixel.value, rawValue) > changeThreshold)
        {
            pixel.value = rawValue;
        }
    }

    TemporalZdepthCompressor::TemporalZdepthCompressor(int width, int height, short changeThreshold, int invalidThreshold)
        : _width(width), _height(height), _pixels(width * height), _changeThreshold(changeThreshold), _invalidThreshold(invalidThreshold), _compressor()
    {
    }

    int TemporalZdepthCompressor::Compress(const uint16_t* depth, uint8_t* compressedData, bool keyFrame)
    {
        int frameSize = _pixels.size();

        if (keyFrame)
        {
            for (int i = 0; i < frameSize; i++)
            {
                _pixels[i].value = depth[i];
                _pixels[i].invalidCount = (depth[i] == 0) ? 1 : 0;
            }

            // Set pixel diff offset into header
            *compressedData++ = 0;
            *compressedData++ = 0;

            // Compressed data
            std::vector<uint8_t> compressed;
            _compressor.Compress(_width, _height, depth, compressed, true);
            for(auto itr = compressed.begin(); itr != compressed.end(); itr++)
            {
                *compressedData++ = *itr;
            }

            return (int)(TEMPORAL_ZDEPTH_HEADER_SIZE + compressed.size());
        }

        int16_t minValue = 0;
        std::vector<int16_t> pixelDiffs(frameSize);
        for (int i = 0; i < frameSize; i++)
        {
            pixelDiffs[i] = _pixels[i].value;
            UpdatePixel(_pixels[i], depth[i], _changeThreshold, _invalidThreshold);
            pixelDiffs[i] = _pixels[i].value - pixelDiffs[i];

            if (pixelDiffs[i] < minValue)
            {
                minValue = pixelDiffs[i];
            }
        }

        uint16_t offset = (uint16_t)std::abs(minValue);
        for (int i = 0; i < frameSize; i++)
        {
            pixelDiffs[i] += offset;
        }

        // Set pixel diff offset into header
        *compressedData++ = (uint8_t)(offset & 0x00FF);
        *compressedData++ = (uint8_t)((offset & 0xFF00) >> 8);

        // Compressed data
        std::vector<uint8_t> compressed;
        _compressor.Compress(_width, _height, reinterpret_cast<uint16_t*>(pixelDiffs.data()), compressed, true);
        for(auto itr = compressed.begin(); itr != compressed.end(); itr++)
        {
            *compressedData++ = *itr;
        }

        return (int)(TEMPORAL_ZDEPTH_HEADER_SIZE + compressed.size());
    }

    TemporalZdepthDecompressor::TemporalZdepthDecompressor(int width, int height) : _width(width), _height(height), _decompressor()
    {
    }

    DepthResult TemporalZdepthDecompressor::Decompress(int compressedSize, const uint8_t* compressedData, uint16_t* depth, bool keyFrame)
    {
        uint16_t pixelDiffOffset = (uint16_t)(compressedData[1] << 8) | compressedData[0];

        uint8_t* compressedImageData = compressedData + TEMPORAL_ZDEPTH_HEADER_SIZE;
        int compressedImageDataSize = compressedSize - TEMPORAL_ZDEPTH_HEADER_SIZE;

        std::vector<uint8_t> compressed(compressedImageData, compressedImageData + compressedImageDataSize);

        if (keyFrame)
        {
            std::vector<uint16_t> decompressed;
            zdepth::DepthResult result = _decompressor.Decompress(compressed, _width, _height, decompressed);

            for(auto itr = decompressed.begin(); itr != decompressed.end(); itr++)
            {
                *depth++ = *itr;
            }

            return result;
        }

        std::vector<uint16_t> pixelDiffs;
        zdepth::DepthResult result = _decompressor.Decompress(compressed, _width, _height, pixelDiffs);
        for(auto itr = pixelDiffs.begin(); itr != pixelDiffs.end(); itr++)
        {
            *depth++ += (*itr - pixelDiffOffset);
        }

        return result;
    }
}
