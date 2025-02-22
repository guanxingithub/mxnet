/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*!
 * \file dnnl_concat-inl.h
 * \brief
 * \author
 */
#ifndef MXNET_OPERATOR_NN_DNNL_DNNL_CONCAT_INL_H_
#define MXNET_OPERATOR_NN_DNNL_DNNL_CONCAT_INL_H_

#if MXNET_USE_ONEDNN == 1
#include <utility>
#include <vector>

#include "operator/nn/concat-inl.h"
#include "dnnl_base-inl.h"
#include "dnnl_ops-inl.h"

namespace mxnet {
namespace op {

class DNNLConcatFwd {
 public:
  dnnl::concat::primitive_desc fwd_pd;

  DNNLConcatFwd(int concat_dim, const std::vector<dnnl::memory::desc>& data_md);

  const dnnl::concat& GetFwd() const {
    return *fwd_;
  }

 private:
  std::shared_ptr<dnnl::concat> fwd_;
};

static DNNLConcatFwd& GetConcatForward(int concat_dim,
                                       const std::vector<NDArray>& in_data,
                                       const std::vector<dnnl::memory::desc>& data_md,
                                       int stack_axis = -1 /*used only by stack op*/) {
#if DMLC_CXX11_THREAD_LOCAL
  static thread_local std::unordered_map<OpSignature, DNNLConcatFwd, OpHash> fwds;
#else
  static MX_THREAD_LOCAL std::unordered_map<OpSignature, DNNLConcatFwd, OpHash> fwds;
#endif

  OpSignature key;
  key.AddSign(concat_dim);
  key.AddSign(stack_axis);
  key.AddSign(in_data);

  auto it = fwds.find(key);
  if (it == fwds.end()) {
    DNNLConcatFwd fwd(concat_dim, data_md);
    it = AddToCache(&fwds, key, fwd);
  }
  return it->second;
}

}  // namespace op
}  // namespace mxnet

#endif  // MXNET_USE_ONEDNN == 1
#endif  // MXNET_OPERATOR_NN_DNNL_DNNL_CONCAT_INL_H_
