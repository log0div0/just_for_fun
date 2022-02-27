////////////////////////////////////////////////////////////////////////////////
/// Modern OpenGL Wrapper
///
/// Copyright (c) 2015 Thibault Schueller
/// This file is distributed under the MIT License
///
/// @file texture.inl
/// @author Thibault Schueller <ryp.sqrt@gmail.com>
////////////////////////////////////////////////////////////////////////////////

namespace mogl
{
    template <GLenum target>
    Texture<target>::Texture()
    :   Handle(GL_TEXTURE)
    {
        glCreateTextures(target, 1, &_handle);
    }

    template <GLenum target>
    Texture<target>::~Texture()
    {
        if (_handle)
            glDeleteTextures(1, &_handle);
    }

    template <GLenum target>
    void Texture<target>::bind(GLuint unit)
    {
        glBindTextureUnit(unit, _handle);
        MOGL_ASSERT_GLSTATE();
    }

    template <GLenum target>
    void Texture<target>::setBuffer(GLenum internalformat, GLuint buffer)
    {
        glTextureBuffer(_handle, internalformat, buffer);
    }

    template <GLenum target>
    void Texture<target>::setBufferRange(GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size)
    {
        glTextureBufferRange(_handle, internalformat, buffer, offset, size);
    }

    template <GLenum target>
    void Texture<target>::setStorage(GLsizei levels, GLenum internalformat, GLsizei width) requires (Storage1D<target>)
    {
        glTextureStorage1D(_handle, levels, internalformat, width);
    }

    template <GLenum target>
    void Texture<target>::setStorage(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) requires (Storage2D<target>)
    {
        glTextureStorage2D(_handle, levels, internalformat, width, height);
        MOGL_ASSERT_GLSTATE();
    }

    template <GLenum target>
    void Texture<target>::setStorage(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth) requires (Storage3D<target>)
    {
        glTextureStorage3D(_handle, levels, internalformat, width, height, depth);
    }

    template <GLenum target>
    void Texture<target>::setStorageMultisample(GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations) requires (Storage2DMultisample<target>)
    {
        glTextureStorage2DMultisample(_handle, samples, internalformat, width, height, fixedsamplelocations);
    }

    template <GLenum target>
    void Texture<target>::setStorageMultisample(GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations) requires (Storage3DMultisample<target>)
    {
        glTextureStorage3DMultisample(_handle, samples, internalformat, width, height, depth, fixedsamplelocations);
    }

    template <GLenum target>
    void Texture<target>::setSubImage(GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels) requires (SubImage1D<target>)
    {
        glTextureSubImage1D(_handle, level, xoffset, width, format, type, pixels);
    }

    template <GLenum target>
    void Texture<target>::setSubImage(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels) requires (SubImage2D<target>)
    {
        glTextureSubImage2D(_handle, level, xoffset, yoffset, width, height, format, type, pixels);
        MOGL_ASSERT_GLSTATE();
    }

    template <GLenum target>
    void Texture<target>::setSubImage(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels) requires (SubImage3D<target>)
    {
        glTextureSubImage3D(_handle, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
    }

    template <GLenum target>
    void Texture<target>::setCompressedSubImage(GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* data) requires (SubImage1D<target>)
    {
        glCompressedTextureSubImage1D(_handle, level, xoffset, width, format, imageSize, data);
    }

    template <GLenum target>
    void Texture<target>::setCompressedSubImage(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data) requires (SubImage2D<target>)
    {
        glCompressedTextureSubImage2D(_handle, level, xoffset, yoffset, width, height, format, imageSize, data);
    }

    template <GLenum target>
    void Texture<target>::setCompressedSubImage(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data) requires (SubImage3D<target>)
    {
        glCompressedTextureSubImage3D(_handle, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
    }

    template <GLenum target>
    void Texture<target>::copySubImage(GLint level, GLint xoffset, GLint x, GLint y, GLsizei width) requires (SubImage1D<target>)
    {
        glCopyTextureSubImage1D(_handle, level, xoffset, x, y, width);
    }

    template <GLenum target>
    void Texture<target>::copySubImage(GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) requires (SubImage2D<target>)
    {
        glCopyTextureSubImage2D(_handle, level, xoffset, yoffset, x, y, width, height);
    }

    template <GLenum target>
    void Texture<target>::copySubImage(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height) requires (SubImage3D<target>)
    {
        glCopyTextureSubImage3D(_handle, level, xoffset, yoffset, zoffset, x, y, width, height);
    }

    template <GLenum target>
    void Texture<target>::generateMipmap()
    {
        glGenerateTextureMipmap(_handle);
        MOGL_ASSERT_GLSTATE();
    }

    template <GLenum target>
    void Texture<target>::getImage(GLint level, GLenum format, GLenum type, GLsizei bufSize, void* pixels)
    {
        glGetTextureImage(_handle, level, format, type, bufSize, pixels);
    }

    template <GLenum target>
    void Texture<target>::getCompressedImage(GLint level, GLsizei bufSize, void* pixels)
    {
        glGetCompressedTextureImage(_handle, level, bufSize, pixels);
    }

    /*
     * Templated accessors definitions
     */

    template <GLenum target>
    GLint Texture<target>::getInt(GLenum property)
    {
        GLint value;
        glGetTextureParameteriv(_handle, property, &value);
        return value;
    }

    template <GLenum target>
    GLfloat Texture<target>::getFloat(GLenum property)
    {
        GLfloat value;
        glGetTextureParameterfv(_handle, property, &value);
        return value;
    }

    /*
     * Level specific accessors definitions
     */

    template <GLenum target>
    GLint Texture<target>::getInt(GLint level, GLenum property)
    {
        GLint value;
        glGetTextureLevelParameteriv(_handle, level, property, &value);
        return value;
    }

    template <GLenum target>
    GLfloat Texture<target>::getFloat(GLint level, GLenum property)
    {
        GLfloat value;
        glGetTextureLevelParameterfv(_handle, level, property, &value);
        return value;
    }

    /*
     * Templated mutators definitions
     */

    template <GLenum target>
    void Texture<target>::setInt(GLenum property, GLint value)
    {
        glTextureParameteri(_handle, property, value);
    }

    template <GLenum target>
    void Texture<target>::setFloat(GLenum property, GLfloat value)
    {
        glTextureParameterf(_handle, property, value);
    }

    template <GLenum target>
    void Texture<target>::setInt(GLenum property, const GLint* value)
    {
        glTextureParameteriv(_handle, property, value);
    }

    template <GLenum target>
    void Texture<target>::setFloat(GLenum property, const GLfloat* value)
    {
        glTextureParameterfv(_handle, property, value);
    }

    template <GLenum target>
    bool Texture<target>::isValid() const
    {
        return glIsTexture(_handle) == GL_TRUE;
    }
}
