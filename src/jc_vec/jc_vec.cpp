#include "jc_vec.hpp"

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

namespace jcv {

namespace huffman {

void sum(const FrequencyMap& other, FrequencyMap& into) {
    std::transform(std::begin(other), std::end(other), std::begin(into),
                   std::begin(into), std::plus<>{});
}

FrequencyMap& operator+=(FrequencyMap& into, const FrequencyMap& other) {
    sum(other, into);
    return into;
}

FrequencyMap count_char_impl(std::string_view text) {
    FrequencyMap freq{};  // Zero-initialized array
    for (const std::uint8_t& c : text) {
        freq[c]++;
    }
    return freq;
}

FrequencyMap count_char(std::string_view text, std::size_t jobs) {
    if (jobs <= 1) {
        return count_char_impl(text);
    }

    const auto thread_count = jobs - 1;
    const auto bound = text.size() / thread_count;

    std::vector<std::future<FrequencyMap>> counting_units(thread_count);

    size_t lower_bound = 0;
    for (auto& unit : counting_units) {
        unit = std::async(std::launch::async, count_char_impl,
                          std::string_view{text}.substr(lower_bound, bound));
        lower_bound += bound;
    }
    auto result = count_char_impl(std::string_view{text}.substr(lower_bound));

    for (auto& unit : counting_units) {
        sum(unit.get(), result);
    }

    return result;
}

Node::Index Tree::generate(const FrequencyMap& freq) {
    using minheap =
        std::priority_queue<Node::Index, std::vector<Node::Index>, NodeCompare>;

    minheap heap(NodeCompare{nodes_}, [] {
        std::vector<Node::Index> vec;
        vec.reserve(leaf_count);
        return vec;
    }());

    for (Node::Index i = 0; i < freq.size(); ++i) {
        if (freq[i] == 0)
            continue;  // Do not set unused characters
        nodes_[i].freq = freq[i];
        heap.push(i);
    }

    Node::Index bind_node_index =
        leaf_count;  // bind nodes are stored after all leaves

    while (heap.size() != 1) {
        auto left = heap.top();
        heap.pop();
        auto right = heap.top();
        heap.pop();

        nodes_[bind_node_index].freq = nodes_[left].freq + nodes_[right].freq;
        nodes_[bind_node_index].left = left;
        nodes_[bind_node_index].right = right;
        heap.push(bind_node_index);

        ++bind_node_index;
    }
    return heap.top();
}

void Tree::print_codes(Node::Index root, const std::string& str) {
    if (root < leaf_count and nodes_[root].freq != 0)  // leaf node
        std::cerr << char(root) << ": " << str << "(" << nodes_[root].left
                  << ", " << nodes_[root].right << ")\n";
    if (nodes_[root].left != Node::invalid_index)
        print_codes(nodes_[root].left, str + "0");
    if (nodes_[root].right != Node::invalid_index)
        print_codes(nodes_[root].right, str + "1");
}

int encode(std::string_view data, std::size_t jobs) {
    auto freqs = count_char(data, jobs);
    Tree tree;
    return tree.generate(freqs);
}

}  // namespace huffman
}  // namespace jcv
