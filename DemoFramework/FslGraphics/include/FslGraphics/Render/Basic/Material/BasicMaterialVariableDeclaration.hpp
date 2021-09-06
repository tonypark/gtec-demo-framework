#ifndef FSLGRAPHICS_RENDER_BASIC_MATERIAL_BASICMATERIALVARIABLEDECLARATION_HPP
#define FSLGRAPHICS_RENDER_BASIC_MATERIAL_BASICMATERIALVARIABLEDECLARATION_HPP
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

#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableDeclarationSpan.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableElement.hpp>
#include <vector>

namespace Fsl
{
  class BasicMaterialVariableDeclaration
  {
    std::vector<BasicMaterialVariableElement> m_elements;
    uint32_t m_stride{0};

  public:
    // move assignment operator
    BasicMaterialVariableDeclaration& operator=(BasicMaterialVariableDeclaration&& other) noexcept;
    // move constructor
    BasicMaterialVariableDeclaration(BasicMaterialVariableDeclaration&& other) noexcept;
    // Request that the compiler generates a copy constructor and assignment operator
    BasicMaterialVariableDeclaration(const BasicMaterialVariableDeclaration&) = default;
    BasicMaterialVariableDeclaration& operator=(const BasicMaterialVariableDeclaration&) = default;


    BasicMaterialVariableDeclaration();

    //! @brief Create a vertex declaration based on the given elements.
    explicit BasicMaterialVariableDeclaration(BasicMaterialVariableDeclarationSpan span);

    //! @brief Create a vertex declaration based on the given elements.
    //! @note  Beware that the elements will be force sorted according to offset (smallest to largest)
    BasicMaterialVariableDeclaration(const BasicMaterialVariableElement* const pElements, const std::size_t elementCount, const uint32_t stride);

    //! @brief Create a vertex declaration based on the given elements.
    //! @note  Beware that the elements will be force sorted according to offset (smallest to largest)
    void Reset(const BasicMaterialVariableElement* const pElements, const std::size_t elementCount, const uint32_t stride);

    //! @brief Get the vertex stride
    uint32_t Stride() const
    {
      return m_stride;
    }

    //! @brief Get the number of elements
    uint32_t Count() const;


    BasicMaterialVariableElement At(const std::size_t index) const
    {
      return m_elements[index];
    }

    //! @brief Get direct access to the elements
    const BasicMaterialVariableElement* DirectAccess() const;

    //! @brief Get the element index of for the given usage and usageIndex (if not found a NotFoundException is thrown)
    int32_t VertexElementGetIndexOf(const BasicMaterialVariableElementUsage usage, const uint32_t usageIndex) const;

    //! @brief Find the element index of for the given usage and usageIndex (if not found <0 is returned)
    int32_t VertexElementIndexOf(const BasicMaterialVariableElementUsage usage, const uint32_t usageIndex) const;

    //! @brief Get the element for the given usage and usageIndex (if not found a NotFoundException is thrown)
    BasicMaterialVariableElement VertexElementGet(const BasicMaterialVariableElementUsage usage, const uint32_t usageIndex) const;

    BasicMaterialVariableDeclarationSpan AsSpan() const;

    bool operator==(const BasicMaterialVariableDeclaration& rhs) const;
    bool operator!=(const BasicMaterialVariableDeclaration& rhs) const;
  };
}

#endif
