//=============================================================================
// Name        : jc.hpp
// Author      : Julien Combattelli
// EMail       : julien.combattelli@gmail.com
// Copyright   : This file is part of huffman banchmark project, provided under
//               MIT license. See LICENSE for more information.
// Description : Implementation of huffman encoding using Data-Oriented Design
//               (Struct of Array) instead of Object-Oriented Design (Array of
//               Struct).
//=============================================================================
#pragma once

#include <algorithm>
#include <cassert>
#include <future>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

namespace jc {

namespace huffman {

int encode(std::string_view input, std::size_t jobs);
}

}  // namespace jc
