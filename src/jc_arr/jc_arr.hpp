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

namespace jca {

namespace huffman {

struct Node {
    using Index = std::uint16_t;
    using Frequency = std::uint32_t;

    static constexpr Index invalid_index = std::numeric_limits<Index>::max();

    Frequency freq{};
    Index left{invalid_index};
    Index right{invalid_index};
};

// Number of values representable with a byte
static constexpr Node::Index values_in_byte = 1 << (sizeof(std::uint8_t) * 8);

// Maximum number of leaf nodes (character) in an Huffman tree
static constexpr Node::Index leaf_count = values_in_byte;

// Maximum number of nodes (leaf + bind) in an Huffman tree
static constexpr Node::Index node_count = leaf_count * 2;

using NodeList = std::array<Node, node_count>;

struct NodeCompare {
    explicit NodeCompare(const NodeList& nodes) : nodes_{nodes} {}
    bool operator()(Node::Index l, Node::Index r) {
        return nodes_[l].freq > nodes_[r].freq;
    }

private:
    const NodeList& nodes_;
};

using FrequencyMap = std::array<Node::Frequency, leaf_count>;

class Tree {
public:
    Node::Index generate(const FrequencyMap& freq);

    void print_codes(Node::Index root, const std::string& str = "");

private:
    NodeList nodes_{};
};

FrequencyMap count_char(std::string_view text, std::size_t jobs = 1);

int encode(std::string_view input, std::size_t jobs);

}  // namespace huffman

}  // namespace jca
