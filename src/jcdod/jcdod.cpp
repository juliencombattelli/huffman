#include <jcdod/jcdod.hpp>

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

void nodelist::set(nodeindex node, nodefreq f) noexcept {
    /*if (node >= node_count)
        throw std::logic_error{"node < node_count: node = " + node};*/
    freq[node] = f;
}

void nodelist::set(nodeindex node,
                   unsigned f,
                   nodeindex l,
                   nodeindex r) noexcept {
    /*if (node >= node_count)
        throw std::logic_error{"node < node_count: node = " + node};*/
    freq[node] = f;
    left[node] = l;
    right[node] = r;
}

huffman_tree::frequency_map huffman_tree::count_char(std::string_view text) {
    frequency_map freq{};  // Zero-initialized array
    for (const std::uint8_t& c : text) {
        freq[c]++;
    }
    return freq;
}

void huffman_tree::sum(const huffman_tree::frequency_map& a,
                       huffman_tree::frequency_map& b) {
    std::transform(std::begin(a), std::end(a), std::begin(b), std::begin(b),
                   std::plus<>{});
}

huffman_tree::frequency_map huffman_tree::count_char_multi(
    std::string_view text) {
    const auto thread_count = 3;  // std::thread::hardware_concurrency() -
                                  // 1;
    const auto bound = text.size() / thread_count;

    std::vector<std::future<frequency_map>> counting_units(thread_count);

    size_t lower_bound = 0;
    for (auto& unit : counting_units) {
        unit = std::async(std::launch::async, count_char,
                          std::string_view{text}.substr(lower_bound, bound));
        lower_bound += bound;
    }
    auto result = count_char(std::string_view{text}.substr(lower_bound));

    for (auto& unit : counting_units) {
        sum(unit.get(), result);
    }

    return result;
}

huffman_tree::nodeindex huffman_tree::generate(
    const huffman_tree::frequency_map& freq) {
    minheap heap{node_compare{nodes}};

    for (nodeindex i = 0; i < freq.size(); ++i) {
        if (freq[i] == 0)
            continue;  // Do not set unused characters
        nodes.set(i, freq[i]);
        heap.push(i);
    }

    constexpr nodeindex first_bind_node = nodelist::leaf_count;
    nodeindex bind_node_index = first_bind_node;

    while (heap.size() != 1) {
        auto left = heap.top();
        heap.pop();
        auto right = heap.top();
        heap.pop();

        nodes.set(bind_node_index, nodes.freq[left] + nodes.freq[right], left,
                  right);
        heap.push(bind_node_index);

        ++bind_node_index;
    }
    return heap.top();
}

void huffman_tree::print_codes(huffman_tree::nodeindex root, std::string str) {
    if (root < 256 and nodes.freq[root] != 0)  // leaf node
        std::cerr << char(root) << ": " << str << "(" << nodes.left[root]
                  << ", " << nodes.right[root] << ")\n";
    if (nodes.left[root] != nodelist::invalid_index)
        print_codes(nodes.left[root], str + "0");
    if (nodes.right[root] != nodelist::invalid_index)
        print_codes(nodes.right[root], str + "1");
}

}  // namespace jcdod
