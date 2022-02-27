////////////////////////////////////////////////////////////////////////////////
/// Modern OpenGL Wrapper
///
/// Copyright (c) 2015 Thibault Schueller
/// This file is distributed under the MIT License
///
/// @file texture.hpp
/// @author Thibault Schueller <ryp.sqrt@gmail.com>
///
/// @brief Texture object OO wrapper using OpenGL 4.5 DSA features
////////////////////////////////////////////////////////////////////////////////

#ifndef MOGL_TEXTURE_INCLUDED
#define MOGL_TEXTURE_INCLUDED

#include <mogl/object/handle.hpp>

namespace mogl
{
    template <GLenum target>
    concept Storage1D =
        target == GL_TEXTURE_1D;

    template <GLenum target>
    concept Storage2D =
        target == GL_TEXTURE_2D ||
        target == GL_TEXTURE_1D_ARRAY ||
        target == GL_TEXTURE_RECTANGLE ||
        target == GL_TEXTURE_CUBE_MAP;

    template <GLenum target>
    concept Storage3D =
        target == GL_TEXTURE_3D ||
        target == GL_TEXTURE_2D_ARRAY ||
        target == GL_TEXTURE_CUBE_MAP_ARRAY;

    template <GLenum target>
    concept Storage2DMultisample =
        target == GL_TEXTURE_2D_MULTISAMPLE;

    template <GLenum target>
    concept Storage3DMultisample =
        target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY;

    template <GLenum target>
    concept SubImage1D =
        target == GL_TEXTURE_1D;

    template <GLenum target>
    concept SubImage2D =
        target == GL_TEXTURE_2D ||
        target == GL_TEXTURE_CUBE_MAP_POSITIVE_X ||
        target == GL_TEXTURE_CUBE_MAP_NEGATIVE_X ||
        target == GL_TEXTURE_CUBE_MAP_POSITIVE_Y ||
        target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Y ||
        target == GL_TEXTURE_CUBE_MAP_POSITIVE_Z ||
        target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Z ||
        target == GL_TEXTURE_1D_ARRAY;

    template <GLenum target>
    concept SubImage3D =
        target == GL_TEXTURE_3D ||
        target == GL_TEXTURE_2D_ARRAY;

    template <GLenum target>
    class Texture : public Handle<GLuint>
    {
    public:
        Texture();
        ~Texture();

        Texture(Texture&& other) = default;
        Texture& operator=(Texture&& other) = default;

    public:
        void    bind(GLuint unit);
        void    setBuffer(GLenum internalformat, GLuint buffer);
        void    setBufferRange(GLenum internalformat, GLuint buffer,
                               GLintptr offset, GLsizeiptr size);
        void    setStorage(GLsizei levels, GLenum internalformat, GLsizei width) requires (Storage1D<target>);
        void    setStorage(GLsizei levels, GLenum internalformat,
                             GLsizei width, GLsizei height) requires (Storage2D<target>);
        void    setStorage(GLsizei levels, GLenum internalformat,
                             GLsizei width, GLsizei height, GLsizei depth) requires (Storage3D<target>);
        void    setStorageMultisample(GLsizei samples, GLenum internalformat,
                                        GLsizei width, GLsizei height,
                                        GLboolean fixedsamplelocations) requires (Storage2DMultisample<target>);
        void    setStorageMultisample(GLsizei samples, GLenum internalformat,
                                        GLsizei width, GLsizei height, GLsizei depth,
                                        GLboolean fixedsamplelocations) requires (Storage3DMultisample<target>);
        void    setSubImage(GLint level, GLint xoffset, GLsizei width,
                              GLenum format, GLenum type, const void* pixels) requires (SubImage1D<target>);
        void    setSubImage(GLint level, GLint xoffset, GLint yoffset,
                              GLsizei width, GLsizei height, GLenum format,
                              GLenum type, const void* pixels) requires (SubImage2D<target>);
        void    setSubImage(GLint level, GLint xoffset, GLint yoffset,
                              GLint zoffset, GLsizei width, GLsizei height,
                              GLsizei depth, GLenum format, GLenum type,
                              const void* pixels) requires (SubImage3D<target>);
        void    setCompressedSubImage(GLint level, GLint xoffset,
                                        GLsizei width, GLenum format,
                                        GLsizei imageSize, const void* data) requires (SubImage1D<target>);
        void    setCompressedSubImage(GLint level, GLint xoffset,
                                        GLint yoffset, GLsizei width,
                                        GLsizei height, GLenum format,
                                        GLsizei imageSize, const void* data) requires (SubImage2D<target>);
        void    setCompressedSubImage(GLint level, GLint xoffset,
                                        GLint yoffset, GLint zoffset,
                                        GLsizei width, GLsizei height,
                                        GLsizei depth, GLenum format,
                                        GLsizei imageSize, const void* data) requires (SubImage3D<target>);
        void    copySubImage(GLint level, GLint xoffset, GLint x, GLint y,
                               GLsizei width) requires (SubImage1D<target>);
        void    copySubImage(GLint level, GLint xoffset, GLint yoffset,
                               GLint x, GLint y, GLsizei width, GLsizei height) requires (SubImage2D<target>);
        void    copySubImage(GLint level, GLint xoffset, GLint yoffset,
                               GLint zoffset, GLint x, GLint y, GLsizei width,
                               GLsizei height) requires (SubImage3D<target>);
        void    generateMipmap();
        void    getImage(GLint level, GLenum format, GLenum type,
                         GLsizei bufSize, void* pixels);
        void    getCompressedImage(GLint level, GLsizei bufSize, void* pixels);
        GLint   getInt(GLenum property);
        GLfloat getFloat(GLenum property);
        GLint   getInt(GLint level, GLenum property);
        GLfloat getFloat(GLint level, GLenum property);
        void    setInt(GLenum property, GLint value);
        void    setFloat(GLenum property, GLfloat value);
        void    setInt(GLenum property, const GLint* value);
        void    setFloat(GLenum property, const GLfloat* value);
        bool    isValid() const;
    };

    using Texture2D = Texture<GL_TEXTURE_2D>;
}

#include "texture.inl"

#endif // MOGL_TEXTURE_INCLUDED
