////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoadTestModels.h (common)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <common/include/LoadModel.h>
#include <common/include/MapLoadArguments.h>

#include <model/include/Model.h>

#include <string>

namespace ell
{
namespace common
{
    /// <summary> Loads a named test model. </summary>
    ///
    /// <param name="name"> The name of built in test model. </param>
    /// <returns> The loaded test model. </returns>
    model::Model LoadTestModel(const std::string& name);
} // namespace common
} // namespace ell
