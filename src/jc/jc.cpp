#include <jc/jc.hpp>

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

template <typename T, std::size_t N>
struct static_alloc {
    using value_type = T;
    static_alloc() = default;
    template <typename U>
    constexpr static_alloc(const static_alloc<U, N>&) noexcept {};
    [[nodiscard]] constexpr value_type* allocate(std::size_t n) {
        return buffer.data();
    }
    constexpr void deallocate(value_type* p, std::size_t n) {}
    template <typename U>
    struct rebind {
        using other = static_alloc<U, N>;
    };
    std::array<value_type, N> buffer{};
};
template <typename T, typename U, std::size_t N>
bool operator==(const static_alloc<T, N>&, const static_alloc<U, N>&) {
    return true;
}
template <typename T, typename U, std::size_t N>
bool operator!=(const static_alloc<T, N>&, const static_alloc<U, N>&) {
    return false;
}

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

FrequencyMap count_char(std::string_view text, std::size_t jobs = 1) {
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

class Tree {
public:
    Node::Index generate(const FrequencyMap& freq) {
        using minheap = std::priority_queue<
            Node::Index,
            std::vector<Node::Index, static_alloc<Node::Index, leaf_count>>,
            NodeCompare>;

        minheap heap{NodeCompare{nodes_}};

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

            nodes_[bind_node_index].freq =
                nodes_[left].freq + nodes_[right].freq;
            nodes_[bind_node_index].left = left;
            nodes_[bind_node_index].right = right;
            heap.push(bind_node_index);

            ++bind_node_index;
        }
        return heap.top();
    }

    void print_codes(Node::Index root, const std::string& str = "") {
        if (root < leaf_count and nodes_[root].freq != 0)  // leaf node
            std::cerr << char(root) << ": " << str << "(" << nodes_[root].left
                      << ", " << nodes_[root].right << ")\n";
        if (nodes_[root].left != Node::invalid_index)
            print_codes(nodes_[root].left, str + "0");
        if (nodes_[root].right != Node::invalid_index)
            print_codes(nodes_[root].right, str + "1");
    }

private:
    NodeList nodes_{};
};

int encode(std::string_view data, std::size_t jobs) {
    auto freqs = count_char(data, jobs);
    Tree tree;
    return tree.generate(freqs);
}

}  // namespace huffman
}  // namespace jc
