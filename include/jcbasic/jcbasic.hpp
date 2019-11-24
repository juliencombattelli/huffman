//=============================================================================
// Name        : jcbasic.hpp
// Author      : Julien Combattelli
// EMail       : julien.combattelli@gmail.com
// Copyright   : This file is part of huffman banchmark project, provided under
//               MIT license. See LICENSE for more information.
// Description : Basic implementation of huffman encoding using
//               std::priority_queue and std::unique_ptr.
//=============================================================================
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace jcb {

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

using count_char_result = std::pair<std::vector<char>, std::vector<int>>;
count_char_result count_char(const std::string& text);

minheap_node::ptr get_huffman_tree(const std::vector<char>& data,
                                   const std::vector<int>& freq);

}  // namespace jcb