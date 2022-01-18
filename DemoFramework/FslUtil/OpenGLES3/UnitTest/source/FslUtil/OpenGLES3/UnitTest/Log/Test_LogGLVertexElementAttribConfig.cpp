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

#include <FslUtil/OpenGLES3/Log/LogGLVertexElementAttribConfig.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <fmt/format.h>
#include <array>
#include <limits>
#include <sstream>
#include <vector>

using namespace Fsl;

namespace
{
  using Test_Log_LogGLVertexElementAttribConfig = TestFixtureFslBase;
}


TEST(Test_Log_LogGLVertexElementAttribConfig, Log)
{
  GLES3::GLVertexElementAttribConfig value(1, 2, GL_FLOAT, GL_TRUE, nullptr);

  std::stringstream stream;
  stream << value;

  const std::string expected(fmt::format("{{AttribIndex={} Size={} Type={} Normalized={} Pointer={}}}", value.AttribIndex, value.Size, value.Type,
                                         NumericCast<uint32_t>(value.Normalized), reinterpret_cast<uintptr_t>(value.Pointer)));

  EXPECT_EQ(expected, stream.str());
}
