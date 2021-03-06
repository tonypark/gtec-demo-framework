#ifndef FSLGRAPHICS_TEXTUREATLAS_BASICTEXTUREATLAS_HPP
#define FSLGRAPHICS_TEXTUREATLAS_BASICTEXTUREATLAS_HPP
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
#include <FslBase/Math/Pixel/PxRectangleU32.hpp>
#include <FslBase/Math/Pixel/PxThicknessU.hpp>
#include <FslGraphics/TextureAtlas/AtlasNineSliceFlags.hpp>
#include <FslGraphics/TextureAtlas/AtlasNineSlicePatchInfo.hpp>
#include <FslGraphics/TextureAtlas/ITextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/NamedAtlasTexture.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasNineSlicePatch.hpp>
#include <utility>
#include <vector>

namespace Fsl
{
  struct Rectangle;
  class UTF8String;

  class BasicTextureAtlas final : public ITextureAtlas
  {
    std::vector<NamedAtlasTexture> m_entries;
    std::vector<TextureAtlasNineSlicePatch> m_nineSlices;

  public:
    uint32_t Count() const final;
    const NamedAtlasTexture& GetEntry(const uint32_t index) const final;
    uint32_t NineSliceCount() const final;
    const TextureAtlasNineSlicePatch& GetNineSlicePatch(const uint32_t index) const final;

    void Reset(const uint32_t capacity);

    void SetEntry(const uint32_t index, const PxRectangleU32& rectanglePx, const PxThicknessU& trimPx, const uint32_t dpi, IO::Path path);

    void AddNineSlice(const uint32_t textureIndex, const PxThicknessU& nineSlicePx, const PxThicknessU& contentMarginPx,
                      const AtlasNineSliceFlags flags);
  };
}

#endif
