/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    * Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
 *      its contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************************************************************************************/

#include "S09_VIV_direct_texture.hpp"
#include <FslBase/NumericCast.hpp>
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <array>
#include <cmath>
#include <string.h>
#include <vector>

#ifndef GL_VIV_direct_texture
#define GL_VIV_YV12 0x8FC0
#define GL_VIV_NV12 0x8FC1
#define GL_VIV_YUY2 0x8FC2
#define GL_VIV_UYVY 0x8FC3
#define GL_VIV_NV21 0x8FC4
#endif

namespace Fsl
{
  namespace
  {
    typedef void(GL_APIENTRY* PFNGLTEXDIRECTVIV)(GLenum Target, GLsizei Width, GLsizei Height, GLenum Format, GLvoid** Pixels);
    typedef void(GL_APIENTRY* PFNGLTEXDIRECTINVALIDATEVIV)(GLenum Target);
    static PFNGLTEXDIRECTVIV pFNglTexDirectVIV = nullptr;
    static PFNGLTEXDIRECTINVALIDATEVIV pFNglTexDirectInvalidateVIV = nullptr;
    // Triangle Vertex positions.
    const GLfloat g_vertices[][2] = {{-0.5f, -0.5f}, {0.5f, -0.5f}, {-0.5f, 0.5f}, {0.5f, 0.5f}};
    const GLfloat g_texcoords[][2] = {{0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}};

    // Start with an identity matrix.
    std::array<GLfloat, 16> g_transformMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};


    const char* g_pszVertexShader =
      "attribute vec4 my_Vertex;\n"
      "uniform   mat4 my_TransformMatrix;\n"
      "attribute vec2 my_Texcoor;\n"
      "varying vec2 vTexcoor;\n"
      "void main()\n"
      "{\n"
      "vTexcoor = my_Texcoor;\n"
      "gl_Position = my_TransformMatrix * my_Vertex;\n"
      "}\n";


    const char* g_pszFragmentShader =
      "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
      "  precision highp float;\n"
      "#else\n"
      "  precision mediump float;\n"
      "#endif\n"
      "uniform sampler2D my_Sampler;\n"
      "varying vec2 vTexcoor; \n"

      "void main (void)\n"
      "{\n"
      "vec4 tex = texture2D(my_Sampler, vTexcoor);\n"
      "gl_FragColor = tex;\n"
      "}\n";


    const std::array<const char*, 3> g_shaderAttributeArray = {"my_Vertex", "my_Texcoor", nullptr};

  }

  S09_VIV_direct_texture::S09_VIV_direct_texture(const DemoAppConfig& config)
    : DemoAppGLES2(config)
    , m_program(g_pszVertexShader, g_pszFragmentShader, g_shaderAttributeArray.data())
    , m_matTransform()
    , m_angle(0.0)
    , m_raw_video()
    , m_fileSize(0)
    , m_frameSize(0)
    , m_ySize(0)
    , m_uSize(0)
    , m_vSize(0)
    , m_vFrames(0)
    , m_data_index(0)
  {
    const GLuint programHandle = m_program.Get();
    GL_CHECK(glUseProgram(programHandle));
    GLint locVertices = 0;
    GLint locTexcoord = 0;
    GLint locSampler = 0;
    GLuint gTexObj = 0;

    if (pFNglTexDirectVIV == nullptr)
    {
      // Get the pointer to the glTexDirectVIV function.
      pFNglTexDirectVIV = (PFNGLTEXDIRECTVIV)eglGetProcAddress("glTexDirectVIV");

      if (pFNglTexDirectVIV == nullptr)
      {
        throw GraphicsException("Required extension glTexDirectVIVnot supported");
      }
    }

    if (pFNglTexDirectInvalidateVIV == nullptr)
    {
      // Get the pointer to the glTexDirectInvalidate function.
      pFNglTexDirectInvalidateVIV = (PFNGLTEXDIRECTINVALIDATEVIV)eglGetProcAddress("glTexDirectInvalidateVIV");

      if (pFNglTexDirectInvalidateVIV == nullptr)
      {
        throw GraphicsException("Required extension glTexDirectInvalidVIVnot supported");
      }
    }
    // Grab location of shader attributes.
    GL_CHECK(locVertices = glGetAttribLocation(programHandle, "my_Vertex"));

    GL_CHECK(locTexcoord = glGetAttribLocation(programHandle, "my_Texcoor"));
    // Transform Matrix is uniform for all vertices here.
    GL_CHECK(m_locTransformMat = glGetUniformLocation(programHandle, "my_TransformMatrix"));
    GL_CHECK(locSampler = glGetUniformLocation(programHandle, "my_Sampler"));

    GetContentManager()->ReadAllBytes(m_raw_video, "f430_160x120xNV21.yuv");
    m_fileSize = NumericCast<int>(m_raw_video.size());
    gTexObj = Load420Texture(160, 120, GL_VIV_NV21);


    if (gTexObj == 0)
    {
      throw GraphicsException("420 video texture not loaded");
    }
    // enable vertex arrays to push the data.
    GL_CHECK(glEnableVertexAttribArray(locVertices));
    GL_CHECK(glEnableVertexAttribArray(locTexcoord));
    // set data in the arrays.
    GL_CHECK(glVertexAttribPointer(locVertices, 2, GL_FLOAT, GL_FALSE, 0, &g_vertices[0][0]));
    GL_CHECK(glVertexAttribPointer(locTexcoord, 2, GL_FLOAT, GL_FALSE, 0, &g_texcoords[0][0]));
    GL_CHECK(glUniformMatrix4fv(m_locTransformMat, 1, GL_FALSE, g_transformMatrix.data()));
    GL_CHECK(glUniform1i(locSampler, 0));
  }


  S09_VIV_direct_texture::~S09_VIV_direct_texture()
  {
    GLint locVertices = 0;
    GLint locTexcoord = 0;
    const GLuint programHandle = m_program.Get();
    // Grab location of shader attributes.
    locVertices = glGetAttribLocation(programHandle, "my_Vertex");
    locTexcoord = glGetAttribLocation(programHandle, "my_Texcoor");
    // cleanup
    glDisableVertexAttribArray(locVertices);
    glDisableVertexAttribArray(locTexcoord);
  }


  void S09_VIV_direct_texture::Update(const DemoTime& demoTime)
  {
    m_matTransform = Matrix::CreateRotationY(m_angle);
    m_angle += 0.1f;
  }


  void S09_VIV_direct_texture::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    const PxSize2D currentSizePx = GetWindowSizePx();
    glViewport(0, 0, currentSizePx.Width(), currentSizePx.Height());

    // Clear background.
    glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUniformMatrix4fv(m_locTransformMat, 1, GL_FALSE, m_matTransform.DirectAccess());

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glFlush();

    LoadFrame();
  }

  int S09_VIV_direct_texture::LoadFrame()
  {
    uint8_t* videoData = m_raw_video.data() + m_data_index;

    if ((m_fileSize - m_data_index) < m_frameSize)
      m_data_index = 0;

    memcpy(m_planes[0], videoData, m_ySize);
    videoData += m_ySize;

    if (m_vSize > 0)
    {
      memcpy(m_planes[1], videoData, m_vSize);
      videoData += m_vSize;
    }

    if (m_uSize > 0)
    {
      memcpy(m_planes[2], videoData, m_uSize);
      videoData += m_uSize;
    }
    m_data_index += m_frameSize;
    if ((m_fileSize - m_data_index) < m_frameSize)
      m_data_index = 0;
    // Mark as dirty.
    (*pFNglTexDirectInvalidateVIV)(GL_TEXTURE_2D);
    if (GL_NO_ERROR != glGetError())
    {
      throw GraphicsException("Error: while LoadFrame()");
    }
    return 0;
  }

  int S09_VIV_direct_texture::Load420Texture(int Width, int Height, int format)
  {
    uint8_t* videoData = m_raw_video.data() + m_data_index;
    GLuint result = 0;
    do
    {
      GLuint name;
      switch (format)
      {
      case GL_VIV_YV12:
        m_ySize = Width * Height;
        m_vSize = m_ySize / 4;
        m_uSize = m_vSize;
        break;

      case GL_VIV_NV12:
      case GL_VIV_NV21:
        m_ySize = Width * Height;
        m_vSize = m_ySize / 2;
        m_uSize = 0;
        break;

      case GL_VIV_YUY2:
      case GL_VIV_UYVY:
        m_ySize = 2 * Width * Height;
        m_vSize = 0;
        m_uSize = 0;
        break;
        return 0;
      }

      m_frameSize = m_ySize + m_uSize + m_vSize;
      m_vFrames = m_fileSize / m_frameSize;

      // Determine the number of frames in the file.
      if ((m_fileSize <= 0) || (m_frameSize <= 0))
      {
        break;
      }
      m_data_index = 0;
      // Create the texture.
      GL_CHECK(glGenTextures(1, &name));
      GL_CHECK(glBindTexture(GL_TEXTURE_2D, name));

      GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
      GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
      GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
      GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

      (*pFNglTexDirectVIV)(GL_TEXTURE_2D, Width, Height, format, (GLvoid**)&m_planes);
      if (GL_NO_ERROR != glGetError())
      {
        throw GraphicsException("Error: while Tex Direct VIV");
      }
      memcpy(m_planes[0], videoData, m_ySize);
      videoData += m_ySize;

      if (m_vSize > 0)
      {
        memcpy(m_planes[1], videoData, m_vSize);
        videoData += m_vSize;
      }

      if (m_uSize > 0)
      {
        memcpy(m_planes[2], videoData, m_uSize);
        videoData += m_uSize;
      }
      m_data_index += m_frameSize;
      if ((m_fileSize - m_data_index) < m_frameSize)
        m_data_index = 0;

      // Mark as dirty.
      (*pFNglTexDirectInvalidateVIV)(GL_TEXTURE_2D);

      if (GL_NO_ERROR != glGetError())
      {
        throw GraphicsException("Error while load texture");
      }
      // Success.
      result = name;
    } while (0);
    // Return result
    return result;
  }
}
