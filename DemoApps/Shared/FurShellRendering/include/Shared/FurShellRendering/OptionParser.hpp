#ifndef SHARED_FURSHELLRENDERING_OPTIONPARSER_HPP
#define SHARED_FURSHELLRENDERING_OPTIONPARSER_HPP
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

#include <FslDemoApp/Base/ADemoOptionParser.hpp>
#include <Shared/FurShellRendering/Config.hpp>

namespace Fsl
{
  class OptionParser : public ADemoOptionParser
  {
    struct ModifiedFlags
    {
      enum Enum
      {
        LayerCount = 0x01,
        FurTextureDimension = 0x02,
        TextureRepeatX = 0x04,
        TextureRepeatY = 0x08,
        HairLength = 0x10,
        BackgroundColor = 0x20,
        TorusMajor = 0x40,
        TorusMinor = 0x80
      };

      uint32_t Value{0};

      ModifiedFlags() = default;

      bool IsFlagged(const ModifiedFlags::Enum flag) const
      {
        return (Value & static_cast<uint32_t>(flag)) != 0;
      }

      void Flag(const ModifiedFlags::Enum flag)
      {
        Value |= static_cast<uint32_t>(flag);
      }
    };

    enum class Quality
    {
      Custom,
      Low,
      Medium,
      High
    };

    Config m_config;
    ModifiedFlags m_modified;
    Quality m_quality;

  public:
    explicit OptionParser(const int32_t defaultRenderMode);
    ~OptionParser() override;

    Config GetConfig() const
    {
      return m_config;
    }

  protected:
    void OnArgumentSetup(std::deque<Option>& rOptions) override;
    OptionParseResult OnParse(const int32_t cmdId, const StringViewLite& strOptArg) override;
    bool OnParsingComplete() override;
  };
}

#endif
