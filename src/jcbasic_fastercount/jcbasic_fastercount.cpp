#include <jcbasic_fastercount/jcbasic_fastercount.hpp>

#include <algorithm>
#include <future>
#include <iostream>
#include <queue>

namespace jcbfc {

struct minheap_node_comparator {
    bool operator()(const minheap_node::ptr& l, const minheap_node::ptr& r) {
        return l->freq > r->freq;
    }
};

template <class T,
          class Container = std::vector<T>,
          class Compare = std::less<typename Container::value_type>>
class priority_queue : public std::priority_queue<T, Container, Compare> {
public:
    T top_and_pop() {
        std::pop_heap(c.begin(), c.end(), comp);
        T value = std::move(c.back());
        c.pop_back();
        return value;
    }

protected:
    using std::priority_queue<T, Container, Compare>::c;
    using std::priority_queue<T, Container, Compare>::comp;
};

using minheap = priority_queue<minheap_node::ptr,
                               std::vector<minheap_node::ptr>,
                               minheap_node_comparator>;

void print_codes(minheap_node* root, std::string str) {
    if (!root)
        return;
    if (root->data)
        std::cerr << *root->data << ": " << str << "\n";
    print_codes(root->left.get(), str + "0");
    print_codes(root->right.get(), str + "1");
}

frequency_map count_char(std::string_view text) {
    frequency_map freq;
    freq.reserve(256);
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

frequency_map count_char_multi(const std::string& text) {
    const auto thread_count = std::thread::hardware_concurrency() - 1;
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
        merge_sum(unit.get(), result);
    }

    return result;
}

minheap_node::ptr get_huffman_tree(const frequency_map& freq) {
    minheap heap;

    for (auto [c, f] : freq) {
        heap.push(std::make_unique<minheap_node>(c, f));
    }

    while (heap.size() != 1) {
        auto left = heap.top_and_pop();
        auto right = heap.top_and_pop();

        auto top = std::make_unique<minheap_node>(left->freq + right->freq);
        top->left = std::move(left);
        top->right = std::move(right);

        heap.push(std::move(top));
    }

    return heap.top_and_pop();
}

}  // namespace jcbfc