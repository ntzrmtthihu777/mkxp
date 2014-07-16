/*
** gl-util.h
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
**
** mkxp is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** mkxp is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with mkxp.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GLUTIL_H
#define GLUTIL_H

#include "gl-fun.h"
#include "etc-internal.h"

/* Struct wrapping GLuint for some light type safety */
#define DEF_GL_ID \
struct ID \
{ \
	GLuint gl; \
	explicit ID(GLuint gl = 0)  \
	    : gl(gl)  \
	{}  \
	ID &operator=(const ID &o)  \
	{  \
		gl = o.gl;  \
		return *this; \
	}  \
	bool operator==(const ID &o) const  \
	{  \
		return gl == o.gl;  \
	}  \
};

/* 2D Texture */
namespace TEX
{
	DEF_GL_ID

	inline ID gen()
	{
		ID id;
		gl.GenTextures(1, &id.gl);

		return id;
	}

	inline void del(ID id)
	{
		gl.DeleteTextures(1, &id.gl);
	}

	inline void bind(ID id)
	{
		gl.BindTexture(GL_TEXTURE_2D, id.gl);
	}

	inline void unbind()
	{
		bind(ID(0));
	}

	inline void uploadImage(GLsizei width, GLsizei height, const void *data, GLenum format)
	{
		gl.TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	}

	inline void uploadSubImage(GLint x, GLint y, GLsizei width, GLsizei height, const void *data, GLenum format)
	{
		gl.TexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, format, GL_UNSIGNED_BYTE, data);
	}

	inline void allocEmpty(GLsizei width, GLsizei height)
	{
		gl.TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	}

	inline void setRepeat(bool mode)
	{
		gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode ? GL_REPEAT : GL_CLAMP_TO_EDGE);
		gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	}

	inline void setSmooth(bool mode)
	{
		gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode ? GL_LINEAR : GL_NEAREST);
		gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode ? GL_LINEAR : GL_NEAREST);
	}
}

/* Framebuffer Object */
namespace FBO
{
	DEF_GL_ID

	enum Mode
	{
		Draw = 0,
		Read = 1,
		Generic = 2
	};

	enum BlitMode
	{
		Nearest = 0,
		Linear  = 1
	};

	inline ID gen()
	{
		ID id;
		gl.GenFramebuffers(1, &id.gl);

		return id;
	}

	inline void del(ID id)
	{
		gl.DeleteFramebuffers(1, &id.gl);
	}

	inline void bind(ID id, Mode mode)
	{
		static const GLenum modes[] =
		{
			GL_DRAW_FRAMEBUFFER,
			GL_READ_FRAMEBUFFER,
			GL_FRAMEBUFFER
		};

		gl.BindFramebuffer(modes[mode], id.gl);
	}

	inline void unbind(Mode mode)
	{
		bind(ID(0), mode);
	}

	inline void setTarget(TEX::ID target, unsigned colorAttach = 0)
	{
		gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttach, GL_TEXTURE_2D, target.gl, 0);
	}

	inline void blit(int srcX, int srcY,
	                 int srcW, int srcH,
	                 int dstX, int dstY,
	                 int dstW, int dstH,
	                 BlitMode mode = Nearest)
	{
		static const GLenum modes[] =
		{
			GL_NEAREST,
			GL_LINEAR
		};

		gl.BlitFramebuffer(srcX, srcY, srcX+srcW, srcY+srcH,
		                   dstX, dstY, dstX+dstW, dstY+dstH,
		                   GL_COLOR_BUFFER_BIT, modes[mode]);
	}

	inline void blit(int srcX, int srcY,
	                 int dstX, int dstY,
	                 int srcW, int srcH,
	                 BlitMode mode = Nearest)
	{
		blit(srcX, srcY, srcW, srcH, dstX, dstY, srcW, srcH, mode);
	}

	inline void clear()
	{
		gl.Clear(GL_COLOR_BUFFER_BIT);
	}
}

/* Vertex Array Object */
namespace VAO
{
	DEF_GL_ID

	inline ID gen()
	{
		ID id;
		gl.GenVertexArrays(1, &id.gl);

		return id;
	}

	inline void del(ID id)
	{
		gl.DeleteVertexArrays(1, &id.gl);
	}

	inline void bind(ID id)
	{
		gl.BindVertexArray(id.gl);
	}

	inline void unbind()
	{
		bind(ID(0));
	}
}

template<GLenum target>
struct GenericBO
{
	DEF_GL_ID

	inline static ID gen()
	{
		ID id;
		gl.GenBuffers(1, &id.gl);

		return id;
	}

	inline static void del(ID id)
	{
		gl.DeleteBuffers(1, &id.gl);
	}

	inline static void bind(ID id)
	{
		gl.BindBuffer(target, id.gl);
	}

	inline static void unbind()
	{
		bind(ID(0));
	}

	inline static void uploadData(GLsizeiptr size, const GLvoid *data, GLenum usage = GL_STATIC_DRAW)
	{
		gl.BufferData(target, size, data, usage);
	}

	inline static void uploadSubData(GLintptr offset, GLsizeiptr size, const GLvoid *data)
	{
		gl.BufferSubData(target, offset, size, data);
	}

	inline static void allocEmpty(GLsizeiptr size, GLenum usage = GL_STATIC_DRAW)
	{
		uploadData(size, 0, usage);
	}
};

/* Vertex Buffer Object */
typedef struct GenericBO<GL_ARRAY_BUFFER> VBO;

/* Index Buffer Object */
typedef struct GenericBO<GL_ELEMENT_ARRAY_BUFFER> IBO;

#undef DEF_GL_ID

namespace PixelStore
{
	/* Setup a 'glSubTexImage2D()' call where the uploaded image
	 * itself is part of a bigger image in client memory */
	inline void setupSubImage(GLint imgWidth, GLint subX, GLint subY)
	{
		gl.PixelStorei(GL_UNPACK_ROW_LENGTH, imgWidth);
		gl.PixelStorei(GL_UNPACK_SKIP_PIXELS, subX);
		gl.PixelStorei(GL_UNPACK_SKIP_ROWS, subY);
	}

	/* Reset all states set with 'setupSubImage()' */
	inline void reset()
	{
		setupSubImage(0, 0, 0);
	}
}

/* Convenience struct wrapping a framebuffer
 * and a 2D texture as its target */
struct TEXFBO
{
	TEX::ID tex;
	FBO::ID fbo;
	int width, height;

	TEXFBO()
	    : tex(0), fbo(0), width(0), height(0)
	{}

	bool operator==(const TEXFBO &other) const
	{
		return (tex == other.tex) && (fbo == other.fbo);
	}

	static inline void init(TEXFBO &obj)
	{
		obj.tex = TEX::gen();
		obj.fbo = FBO::gen();
		TEX::bind(obj.tex);
		TEX::setRepeat(false);
		TEX::setSmooth(false);
	}

	static inline void allocEmpty(TEXFBO &obj, int width, int height)
	{
		TEX::bind(obj.tex);
		TEX::allocEmpty(width, height);
		obj.width = width;
		obj.height = height;
	}

	static inline void linkFBO(TEXFBO &obj)
	{
		FBO::bind(obj.fbo, FBO::Draw);
		FBO::setTarget(obj.tex);
	}

	static inline void fini(TEXFBO &obj)
	{
		FBO::del(obj.fbo);
		TEX::del(obj.tex);
	}
};

#endif // GLUTIL_H
