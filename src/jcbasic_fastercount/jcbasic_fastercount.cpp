#include <jcbasic_fastercount/jcbasic_fastercount.hpp>

#include <algorithm>
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

frequency_map count_char(const std::string& text) {
    frequency_map freq;
    for (const auto& c : text) {
        freq[c]++;
    }
    return freq;
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