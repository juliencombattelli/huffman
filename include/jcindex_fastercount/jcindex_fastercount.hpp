//=============================================================================
// Name        : jcindex.hpp
// Author      : Julien Combattelli
// EMail       : julien.combattelli@gmail.com
// Copyright   : This file is part of huffman banchmark project, provided under
//               MIT license. See LICENSE for more information.
// Description : Implementation of huffman encoding based on jcindex but using
//               unordered_map for counting.
//=============================================================================
#pragma once

#include <algorithm>
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

namespace jcidxfc {

enum class reserve_memory : bool { no = false, yes = true };

class huffman_tree {
public:
    struct node;
    using nodelist = std::vector<node>;
    using nodeindex = typename nodelist::size_type;
    static constexpr nodeindex kInvalidIndex =
        std::numeric_limits<nodeindex>::max();

    struct node {
        std::optional<char> data;
        unsigned freq;
        nodeindex left{kInvalidIndex};
        nodeindex right{kInvalidIndex};

        node() = default;
        node(char data, unsigned freq) : data{data}, freq{freq} {}
        node(unsigned freq, nodeindex left, nodeindex right)
            : freq{freq}, left{left}, right{right} {}
    };

    struct node_compare {
        explicit node_compare(const nodelist& nodes) : nodes{nodes} {}
        bool operator()(nodeindex l, nodeindex r) {
            return nodes[l].freq > nodes[r].freq;
        }

    private:
        const nodelist& nodes;
    };

    using minheap =
        std::priority_queue<nodeindex, std::vector<nodeindex>, node_compare>;

    using frequency_map = std::unordered_map<char, int>;

    huffman_tree(reserve_memory reserve = reserve_memory::no)
        : reserve_mem{reserve} {}

    frequency_map count_char(const std::string& text) {
        frequency_map freq;
        for (const auto& c : text) {
            freq[c]++;
        }
        return freq;
    }

    void merge_sum(const frequency_map& a, frequency_map& b) {
        for (auto [k, v] : a) {
            b[k] += v;
        }
    }

    huffman_tree::frequency_map count_char_multi(const std::string& text) {
        const auto thread_count = std::thread::hardware_concurrency() - 1;
        const auto bound = text.size() / thread_count;

        std::vector<std::future<huffman_tree::frequency_map>> counting_units(
            thread_count);

        auto counter = [](std::string_view text) {
            huffman_tree::frequency_map freq;
            for (const auto& c : text) {
                freq[c]++;
            }
            return freq;
        };

        size_t lower_bound = 0;
        for (auto& unit : counting_units) {
            unit =
                std::async(std::launch::async, counter,
                           std::string_view(text).substr(lower_bound, bound));
            lower_bound += bound;
        }
        // Handle leftover
        auto result = counter(std::string_view(text).substr(lower_bound));

        for (auto& unit : counting_units) {
            merge_sum(unit.get(), result);
        }

        return result;
    }

    node& generate(const frequency_map& freqs) {
        nodes.reserve(freqs.size() * 2);
        {
            nodeindex i = 0;
            for (auto [data, freq] : freqs) {
                nodes.emplace_back(data, freq);
                heap.emplace(i++);
            }
        }

        while (heap.size() != 1) {
            auto left = heap.top();
            heap.pop();
            auto right = heap.top();
            heap.pop();

            nodes.emplace_back(nodes[left].freq + nodes[right].freq, left,
                               right);
            heap.push(nodes.size() - 1);
        }
        return nodes[heap.top()];
    }

    void print_codes(const huffman_tree::node& root, std::string str = "") {
        if (root.data)
            std::cerr << *root.data << ": " << str << "\n";
        if (root.left != huffman_tree::kInvalidIndex)
            print_codes(nodes[root.left], str + "0");
        if (root.right != huffman_tree::kInvalidIndex)
            print_codes(nodes[root.right], str + "1");
    }

private:
    nodelist nodes;
    minheap heap{node_compare{nodes}};
    reserve_memory reserve_mem;
};

}  // namespace jcidxfc
