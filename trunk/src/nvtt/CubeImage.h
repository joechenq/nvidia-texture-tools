// Copyright (c) 2009-2011 Ignacio Castano <castano@gmail.com>
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

#ifndef NVTT_CUBEIMAGE_H
#define NVTT_CUBEIMAGE_H

#include "nvtt.h"
#include "TexImage.h"

#include "nvimage/FloatImage.h"

#include "nvcore/RefCounted.h"
#include "nvcore/Ptr.h"


namespace nvtt
{

    struct CubeSurface::Private : public nv::RefCounted
    {
        void operator=(const Private &);
    public:
        Private()
        {
            nvDebugCheck( refCount() == 0 );

            size = 0;
        }
        Private(const Private & p) : RefCounted() // Copy ctor. inits refcount to 0.
        {
            nvDebugCheck( refCount() == 0 );

            size = p.size;
            for (uint i = 0; i < 6; i++) {
                face[i] = p.face[6];
            }
        }
        ~Private()
        {
        }

        void allocate(int size)
        {
            this->size = size;
            for (uint i = 0; i < 6; i++) {
                face[i].detach();
                face[i].m->image = new nv::FloatImage;
                face[i].m->image->allocate(size, size, 1);
            }
        }

        int size;
        Surface face[6];
    };

} // nvtt namespace


#endif // NVTT_CUBEIMAGE_H