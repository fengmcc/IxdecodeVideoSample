/*
 * Copyright 2020 NVIDIA Corporation
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *    http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

extern "C" {
#include "libavutil/avstring.h"
#include "libavutil/avutil.h"
#include "libavutil/dict.h"
}

#include "MemoryInterfaces.hpp"
#include "IxCodecCLIOptions.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <list>
#include <sstream>
#include <vector>

using namespace std;
using namespace VPF;

namespace VPF
{
struct NvDecoderClInterface_Impl {
  map<string, string> options;
  AVDictionary* dict = nullptr;

  ~NvDecoderClInterface_Impl() {}
};
} // namespace VPF

NvDecoderClInterface::NvDecoderClInterface(const map<string, string>& opts)
{
  pImpl = new NvDecoderClInterface_Impl;
  pImpl->options = map<string, string>(opts);
}

NvDecoderClInterface::~NvDecoderClInterface()
{
  delete pImpl;
  pImpl = nullptr;
}

AVDictionary* NvDecoderClInterface::GetOptions()
{
  auto AvErrorToString = [](int av_error_code) {
    const auto buf_size = 1024U;
    char* err_string = (char*)calloc(buf_size, sizeof(*err_string));
    if (!err_string) {
      return string();
    }

    if (0 != av_strerror(av_error_code, err_string, buf_size - 1)) {
      free(err_string);
      stringstream ss;
      ss << "Unknown error with code " << av_error_code;
      return ss.str();
    }

    string str(err_string);
    free(err_string);
    return str;
  };

  for (auto& pair : pImpl->options) {
    auto err =
        av_dict_set(&pImpl->dict, pair.first.c_str(), pair.second.c_str(), 0);
    if (err < 0) {
      cerr << "Can't set up dictionary option: " << pair.first << " "
           << pair.second << ": " << AvErrorToString(err) << "\n";
      return nullptr;
    }
  }

  return pImpl->dict;
}