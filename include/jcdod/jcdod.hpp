//=============================================================================
// Name        : jcdod.hpp
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

namespace jcdod {

class nodelist {
public:
    using nodefreq = std::uint32_t;
    using nodeindex = short;

    // Number of values representable with a char
    static constexpr nodeindex char_count = 1 << (sizeof(char) * 8);

    // Maximum number of leaf nodes (character) in an Huffman tree
    static constexpr nodeindex leaf_count = char_count;

    // Maximum number of nodes (leaf + bind) in an Huffman tree
    static constexpr nodeindex node_count = leaf_count * 2;

    static constexpr nodeindex invalid_index =
        std::numeric_limits<nodeindex>::max();

    template <typename T>
    using nodedata = std::array<T, node_count>;

    // Compile-time initialization of nodedata
    template <typename T>
    static constexpr nodedata<T> initialize_nodedata(T value) {
        nodedata<T> data{};
        for (auto& element : data) {
            element = value;
        }
        return data;
    }

    void set(nodeindex node, nodefreq f) noexcept;

    void set(nodeindex node, unsigned f, nodeindex l, nodeindex r) noexcept;

    nodedata<nodefreq> freq{};
    nodedata<nodeindex> left = initialize_nodedata(invalid_index);
    nodedata<nodeindex> right = initialize_nodedata(invalid_index);
};

class huffman_tree {
public:
    using nodeindex = nodelist::nodeindex;
    using nodefreq = nodelist::nodefreq;

    struct node_compare {
        explicit node_compare(const nodelist& nodes) : nodes{nodes} {}
        bool operator()(nodeindex l, nodeindex r) {
            return nodes.freq[l] > nodes.freq[r];
        }

    private:
        const nodelist& nodes;
    };

    using minheap =
        std::priority_queue<nodeindex, std::vector<nodeindex>, node_compare>;

    using frequency_map = std::array<nodefreq, 256>;

    static frequency_map count_char(std::string_view text);

    void sum(const frequency_map& a, frequency_map& b);

    frequency_map count_char_multi(std::string_view text);

    nodeindex generate(const frequency_map& freq);

    void print_codes(huffman_tree::nodeindex root, std::string str = "");

    nodelist nodes;
};

}  // namespace jcdod
