// Copyright NVIDIA Corporation 2007 -- Ignacio Castano <icastano@nvidia.com>
// 
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include "CompressorDXT.h"
#include "OutputOptions.h"

#include "nvtt.h"

#include "nvcore/Memory.h"

#include "nvimage/Image.h"
#include "nvimage/ColorBlock.h"
#include "nvimage/BlockDXT.h"

#include <new> // placement new


// OpenMP
#if defined(HAVE_OPENMP)
#include <omp.h>
#endif

using namespace nv;
using namespace nvtt;


void FixedBlockCompressor::compress(nvtt::AlphaMode alphaMode, uint w, uint h, const float * data, const nvtt::CompressionOptions::Private & compressionOptions, const nvtt::OutputOptions::Private & outputOptions)
{
    const uint bs = blockSize();
    const uint bw = (w + 3) / 4;
    const uint bh = (h + 3) / 4;

#if defined(HAVE_OPENMP)
    bool singleThreaded = false;
#else
    bool singleThreaded = true;
#endif

    // Use a single thread to compress small textures.
    if (bw * bh < 16) singleThreaded = true;

    if (singleThreaded)
    {
        nvDebugCheck(bs <= 16);
        uint8 mem[16]; // @@ Output one row at a time!

        for (int y = 0; y < int(h); y += 4) {
            for (uint x = 0; x < w; x += 4) {

                ColorBlock rgba;
                rgba.init(w, h, data, x, y);

                compressBlock(rgba, alphaMode, compressionOptions, mem);

                if (outputOptions.outputHandler != NULL) {
                    outputOptions.outputHandler->writeData(mem, bs);
                }
            }
        }
    }
#if defined(HAVE_OPENMP)
    else
    {
        const uint size = bs * bw * bh;
        uint8 * mem = new uint8[size];

        #pragma omp parallel
        {
            #pragma omp for
            for (int i = 0; i < int(bw*bh); i++)
            {
                const uint x = i % bw;
                const uint y = i / bw;

		ColorBlock rgba;
		rgba.init(w, h, data, 4*x, 4*y);

		uint8 * ptr = mem + (y * bw + x) * bs;
		compressBlock(rgba, alphaMode, compressionOptions, ptr);
	    } // omp for
	} // omp parallel

	if (outputOptions.outputHandler != NULL) {
	    outputOptions.outputHandler->writeData(mem, size);
	}

        delete [] mem;
    }
#endif
}


#include "bc6h/tile.h"

void TileCompressor::compress(AlphaMode alphaMode, uint w, uint h, const float * data, const CompressionOptions::Private & compressionOptions, const OutputOptions::Private & outputOptions)
{
    const uint bs = blockSize();
    const uint bw = (w + 3) / 4;
    const uint bh = (h + 3) / 4;

    bool singleThreaded = true;

    if (singleThreaded)
    {
        nvDebugCheck(bs <= 16);
        uint8 mem[16]; // @@ Output one row at a time!

        for (uint y = 0; y < h; y += 4) {
            for (uint x = 0; x < w; x += 4) {

                Tile tile;
                //tile.init((const float *)data, w, h, x, y);

                compressBlock(tile, alphaMode, compressionOptions, mem);

                if (outputOptions.outputHandler != NULL) {
                    outputOptions.outputHandler->writeData(mem, bs);
                }
            }
        }
    }
}
