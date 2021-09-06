#ifndef FSLSIMPLEUI_RENDER_IMBATCH_RENDERSYSTEMBUFFERRECORD_HPP
#define FSLSIMPLEUI_RENDER_IMBATCH_RENDERSYSTEMBUFFERRECORD_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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
 *    * Neither the name of the NXP. nor the names of
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

#include <FslGraphics/Render/Basic/IBasicDynamicBuffer.hpp>
#include <memory>
#include <utility>

namespace Fsl
{
  namespace UI
  {
    namespace RenderIMBatch
    {
      struct RenderSystemBufferRecord
      {
        std::shared_ptr<IBasicDynamicBuffer> VertexBuffer;
        uint32_t VertexCapacity{};
        std::shared_ptr<IBasicDynamicBuffer> IndexBuffer;
        uint32_t IndexCapacity{};

        RenderSystemBufferRecord() = default;

        RenderSystemBufferRecord(std::shared_ptr<IBasicDynamicBuffer> vertexBuffer, const uint32_t vertexCapacity)
          : VertexBuffer(std::move(vertexBuffer))
          , VertexCapacity(vertexCapacity)

        {
        }

        RenderSystemBufferRecord(std::shared_ptr<IBasicDynamicBuffer> vertexBuffer, const uint32_t vertexCapacity,
                                 std::shared_ptr<IBasicDynamicBuffer> indexBuffer, const uint32_t indexCapacity)
          : VertexBuffer(std::move(vertexBuffer))
          , VertexCapacity(vertexCapacity)
          , IndexBuffer(std::move(indexBuffer))
          , IndexCapacity(indexCapacity)
        {
        }
      };
    }
  }
}

#endif
