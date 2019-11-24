//=============================================================================
// Name        : jcbasic.hpp
// Author      : Julien Combattelli
// EMail       : julien.combattelli@gmail.com
// Copyright   : This file is part of huffman banchmark project, provided under
//               MIT license. See LICENSE for more information.
// Description : Implementation of huffman encoding based on jcbasic but using
//               unordered_map for counting.
//=============================================================================
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace jcbfc {

struct minheap_node {
    using ptr = std::unique_ptr<minheap_node>;

    std::optional<char> data;
    unsigned freq;
    ptr left{nullptr};
    ptr right{nullptr};

    minheap_node(char data, unsigned freq) : data{data}, freq{freq} {}
    explicit minheap_node(unsigned freq) : freq{freq} {}
};

void print_codes(minheap_node* root, std::string str);

using frequency_map = std::unordered_map<char, int>;
frequency_map count_char(std::string_view text);

frequency_map merge_sum(const frequency_map& a, const frequency_map& b);
frequency_map count_char_multi(const std::string& text);

minheap_node::ptr get_huffman_tree(const frequency_map& freq);

}  // namespace jcbfc