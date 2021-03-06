#ifndef SHARED_MODELVIEWER_OPTIONPARSER_HPP
#define SHARED_MODELVIEWER_OPTIONPARSER_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include <FslBase/IO/Path.hpp>
#include <FslDemoApp/Base/ADemoOptionParser.hpp>

namespace Fsl
{
  enum class MeshMode
  {
    SingleMesh = 0,
    MultipleMeshes = 1,
    Wireframe = 2
  };

  enum class ShaderMode
  {
    PerPixelTextured = 0,
    PerPixelNoTextures = 1,
    NoLightVertexColor = 2
  };

  class OptionParser : public ADemoOptionParser
  {
    int32_t m_scene;
    MeshMode m_meshMode{MeshMode::SingleMesh};
    ShaderMode m_shaderMode{ShaderMode::PerPixelTextured};
    IO::Path m_customModelPath;

  public:
    OptionParser();
    ~OptionParser() override;

    int32_t GetScene() const
    {
      return m_scene;
    }

    MeshMode GetMeshMode() const
    {
      return m_meshMode;
    }

    ShaderMode GetShaderMode() const
    {
      return m_shaderMode;
    }

    IO::Path GetCustomModelPath() const
    {
      return m_customModelPath;
    }

  protected:
    void OnArgumentSetup(std::deque<Option>& rOptions) override;
    OptionParseResult OnParse(const int32_t cmdId, const StringViewLite& strOptArg) override;
    bool OnParsingComplete() override;
  };
}

#endif
