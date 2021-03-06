////////////////////////////////////////////////////////////////////////////////
/// Modern OpenGL Wrapper
///
/// Copyright (c) 2015 Thibault Schueller
/// This file is distributed under the MIT License
///
/// @file handle.inl
/// @author Thibault Schueller <ryp.sqrt@gmail.com>
////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <cassert>

#include <mogl/function/states.hpp>

namespace mogl
{
    template <class T>
    inline Handle<T>::Handle(GLenum identifier)
    :   _handle(0),
        _identifier(identifier)
    {}

    template <class T>
    inline Handle<T>::Handle(Handle&& other) noexcept
    :   _handle(other._handle),
        _identifier(other._identifier)
    {
        other._handle = 0;
    }

    template <class T>
    inline Handle<T>& Handle<T>::operator=(Handle&& other) noexcept
    {
        assert(_identifier == other._identifier);

        std::swap(_handle, other._handle);
        return *this;
    }

    template <class T>
    inline T Handle<T>::getHandle() const
    {
        return _handle;
    }

    template <>
    inline std::string Handle<GLuint>::getLabel() const
    {
        GLint               maxLabelLen = get<GLint>(GL_MAX_LABEL_LENGTH);
        std::vector<GLchar> buffer(maxLabelLen);
        GLsizei             labelLen = 0;

        glGetObjectLabel(_identifier, _handle, maxLabelLen, &labelLen, &buffer[0]);
        return (&buffer[0]);
    }

    template <>
    inline std::string Handle<GLsync>::getLabel() const
    {
        GLint               maxLabelLen = get<GLint>(GL_MAX_LABEL_LENGTH);
        std::vector<GLchar> buffer(maxLabelLen);
        GLsizei             labelLen = 0;

        glGetObjectPtrLabel(_handle, maxLabelLen, &labelLen, &buffer[0]);
        return (&buffer[0]);
    }

    template <>
    inline void Handle<GLuint>::setLabel(const std::string& name)
    {
        glObjectLabel(_identifier, _handle, name.length(), name.c_str());
    }

    template <>
    inline void Handle<GLsync>::setLabel(const std::string& name)
    {
        glObjectPtrLabel(_handle, name.length(), name.c_str());
    }
}
