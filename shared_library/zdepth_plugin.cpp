// This code is licensed under the MIT License. Copyright (c) 2020 Soichiro Sugimoto.

#define EXPORT_API __declspec(dllexport)

#include "pch.h"
#include "zdepth.hpp"
#include "temporal_zdepth.hpp"

extern "C"
{
    EXPORT_API zdepth::DepthCompressor* CreateZdepthCompressor()
    {
        return new zdepth::DepthCompressor();
    }

    EXPORT_API void DeleteZdepthCompressor(zdepth::DepthCompressor* compressor)
    {
        delete compressor;
    }

    EXPORT_API int CompressZdepth(zdepth::DepthCompressor* compressor, int width, int height, const uint16_t *depth, uint8_t* compressedData, bool keyframe)
    {
        std::vector<uint8_t> compressed;
        compressor->Compress(width, height, depth, compressed, keyframe);

        for(auto itr = compressed.begin(); itr != compressed.end(); itr++)
        {
            *compressedData++ = *itr;
        }

        return (int) compressed.size();
    }

    EXPORT_API int DecompressZdepth(zdepth::DepthCompressor* decompressor, int width, int height, int compressedSize, const uint8_t* compressedData, uint16_t* depth)
    {
        std::vector<uint8_t> compressed(compressedData, compressedData + compressedSize);
        std::vector<uint16_t> decompressed;

        zdepth::DepthResult result = decompressor->Decompress(compressed, width, height, decompressed);

        for(auto itr = decompressed.begin(); itr != decompressed.end(); itr++)
        {
            *depth++ = *itr;
        }

        return (int) result;
    }

    EXPORT_API zdepth::TemporalZdepthCompressor* CreateTemporalZdepthCompressor(int width, int height, uint16_t changeThreshold, int invalidThreshold)
    {
        return new zdepth::TemporalZdepthCompressor(width, height, changeThreshold, invalidThreshold);
    }

    EXPORT_API zdepth::TemporalZdepthDecompressor* CreateTemporalZdepthDecompressor(int width, int height)
    {
        return new zdepth::TemporalZdepthDecompressor(width, height);
    }

    EXPORT_API void DeleteTemporalZdepthCompressor(zdepth::TemporalZdepthCompressor* compressor)
    {
        delete compressor;
    }

    EXPORT_API void DeleteTemporalZdepthDecompressor(zdepth::TemporalZdepthDecompressor* decompressor)
    {
        delete decompressor;
    }

    EXPORT_API int CompressTemporalZdepth(zdepth::TemporalZdepthCompressor* compressor, const uint16_t* depth, uint8_t* compressedData, bool keyframe)
    {
        return compressor->Compress(depth, compressedData, keyframe);
    }

    EXPORT_API int DecompressTemporalZdepth(zdepth::TemporalZdepthDecompressor* decompressor, int compressedSize, const uint8_t* compressedData, uint16_t* depth, bool keyframe)
    {
        return (int) decompressor->Decompress(compressedSize, compressedData, depth, keyframe);
    }
}
