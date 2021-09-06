/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslGraphics/Vertices/VertexPositionTexture3.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Math/LogMatrix.hpp>
#include <FslBase/Log/Math/LogPoint2.hpp>
#include <FslBase/Log/Math/LogVector2.hpp>
#include <FslBase/Log/Math/LogVector3.hpp>
#include <FslBase/Log/Math/LogVector4.hpp>
#include <FslGraphics/Log/LogColor.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <FslGraphics/Vertices/VertexDeclaration.hpp>
#include <cstddef>

using namespace Fsl;

namespace
{
  using TestVertices_VertexPositionTexture3 = TestFixtureFslGraphics;
}


TEST(TestVertices_VertexPositionTexture3, Construct_Default)
{
  VertexPositionTexture3 vertex;

  EXPECT_EQ(Vector3(), vertex.Position);
  EXPECT_EQ(Vector3(), vertex.TextureCoordinate);
}


TEST(TestVertices_VertexPositionTexture3, GetVertexDeclaration)
{
  const VertexElementEx expected0(offsetof(VertexPositionTexture3, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0u);
  const VertexElementEx expected1(offsetof(VertexPositionTexture3, TextureCoordinate), VertexElementFormat::Vector3,
                                  VertexElementUsage::TextureCoordinate, 0u);
  const auto vertexDecl = VertexDeclaration(VertexPositionTexture3::AsVertexDeclarationSpan());

  EXPECT_EQ(sizeof(VertexPositionTexture3), vertexDecl.VertexStride());
  ASSERT_EQ(2u, vertexDecl.Count());
  EXPECT_NE(nullptr, vertexDecl.DirectAccess());

  // Get by element usage
  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(expected0, vertexDecl.VertexElementGet(VertexElementUsage::Position, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0u), 1);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::TextureCoordinate, 0u), 1);
  EXPECT_EQ(expected1, vertexDecl.VertexElementGet(VertexElementUsage::TextureCoordinate, 0u));

  // By index
  EXPECT_EQ(expected0, vertexDecl.At(0u));
  EXPECT_EQ(expected1, vertexDecl.At(1u));

  // Direct access should produce the same as At
  for (uint32_t i = 0; i < vertexDecl.Count(); ++i)
  {
    EXPECT_EQ(vertexDecl.At(i), vertexDecl.DirectAccess()[i]);
  }
}


TEST(TestVertices_VertexPositionTexture3, GetVertexDeclarationArray)
{
  const VertexElementEx expected0(offsetof(VertexPositionTexture3, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0u);
  const VertexElementEx expected1(offsetof(VertexPositionTexture3, TextureCoordinate), VertexElementFormat::Vector3,
                                  VertexElementUsage::TextureCoordinate, 0u);
  const auto vertexDecl = VertexPositionTexture3::GetVertexDeclarationArray();

  EXPECT_EQ(sizeof(VertexPositionTexture3), vertexDecl.VertexStride());
  ASSERT_EQ(2u, vertexDecl.Count());
  EXPECT_NE(nullptr, vertexDecl.DirectAccess());

  // Get by element usage
  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(expected0, vertexDecl.VertexElementGet(VertexElementUsage::Position, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0u), 1);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::TextureCoordinate, 0u), 1);
  EXPECT_EQ(expected1, vertexDecl.VertexElementGet(VertexElementUsage::TextureCoordinate, 0u));

  // By index
  EXPECT_EQ(expected0, vertexDecl.At(0u));
  EXPECT_EQ(expected1, vertexDecl.At(1u));

  // Direct access should produce the same as At
  for (uint32_t i = 0; i < vertexDecl.Count(); ++i)
  {
    EXPECT_EQ(vertexDecl.At(i), vertexDecl.DirectAccess()[i]);
  }
}


TEST(TestVertices_VertexPositionTexture3, AsVertexDeclarationSpan)
{
  const VertexElementEx expected0(offsetof(VertexPositionTexture3, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0u);
  const VertexElementEx expected1(offsetof(VertexPositionTexture3, TextureCoordinate), VertexElementFormat::Vector3,
                                  VertexElementUsage::TextureCoordinate, 0u);
  const auto vertexDecl = VertexPositionTexture3::AsVertexDeclarationSpan();

  EXPECT_EQ(sizeof(VertexPositionTexture3), vertexDecl.VertexStride());
  ASSERT_EQ(2u, vertexDecl.Count());
  EXPECT_NE(nullptr, vertexDecl.DirectAccess());

  // Get by element usage
  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(expected0, vertexDecl.VertexElementGet(VertexElementUsage::Position, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0u), 1);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::TextureCoordinate, 0u), 1);
  EXPECT_EQ(expected1, vertexDecl.VertexElementGet(VertexElementUsage::TextureCoordinate, 0u));

  // By index
  EXPECT_EQ(expected0, vertexDecl.At(0u));
  EXPECT_EQ(expected1, vertexDecl.At(1u));

  // Direct access should produce the same as At
  for (uint32_t i = 0; i < vertexDecl.Count(); ++i)
  {
    EXPECT_EQ(vertexDecl.At(i), vertexDecl.DirectAccess()[i]);
  }
}
