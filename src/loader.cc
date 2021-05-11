// Copyright (c) 2021, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "loader.h"

#include <NvInferPlugin.h>
#include <mutex>
#include "logging.h"
#include "triton/backend/backend_common.h"

namespace triton { namespace backend { namespace tensorrt {

TRITONSERVER_Error*
LoadPlan(
    const std::string& plan_path, nvinfer1::IRuntime** runtime,
    nvinfer1::ICudaEngine** engine)
{
  // Create runtime only if it is not provided
  if (*runtime == nullptr) {
    *runtime = nvinfer1::createInferRuntime(tensorrt_logger);
    if (*runtime == nullptr) {
      return TRITONSERVER_ErrorNew(
          TRITONSERVER_ERROR_INTERNAL, "unable to create TensorRT runtime");
    }
  }

  std::string model_data_str;
  RETURN_IF_ERROR(ReadTextFile(plan_path, &model_data_str));
  std::vector<char> model_data(model_data_str.begin(), model_data_str.end());

  *engine =
      (*runtime)->deserializeCudaEngine(&model_data[0], model_data.size());
  if (*engine == nullptr) {
    return TRITONSERVER_ErrorNew(
        TRITONSERVER_ERROR_INTERNAL, "unable to create TensorRT engine");
  }

  return nullptr;
}

}}}  // namespace triton::backend::tensorrt
