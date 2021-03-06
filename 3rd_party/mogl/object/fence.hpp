////////////////////////////////////////////////////////////////////////////////
/// Modern OpenGL Wrapper
///
/// Copyright (c) 2015 Thibault Schueller
/// This file is distributed under the MIT License
///
/// @file fence.hpp
/// @author Thibault Schueller <ryp.sqrt@gmail.com>
/// @brief Buffer object OO wrapper using OpenGL 4.5 DSA features
////////////////////////////////////////////////////////////////////////////////

#ifndef MOGL_FENCE_INCLUDED
#define MOGL_FENCE_INCLUDED

#include <mogl/object/handle.hpp>

namespace mogl
{
    class Fence : public Handle<GLsync>
    {
    public:
        Fence(GLenum condition, GLbitfield flags = 0);
        ~Fence();

        Fence(const Fence& other) = delete;
        Fence& operator=(const Fence& other) = delete;

        Fence(Fence&& other) = default;

    public:
        void    waitSync(GLbitfield flags, GLuint64 timeout = 0);
        GLenum  waitClientSync(GLbitfield flags, GLuint64 timeout = 0);
        bool    isValid() const;
    };
}

#include "fence.inl"

#endif // MOGL_FENCE_INCLUDED
