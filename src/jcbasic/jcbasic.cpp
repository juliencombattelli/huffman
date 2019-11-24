#include <jcbasic/jcbasic.hpp>

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <utility>
#include <vector>

namespace jcb {

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

count_char_result count_char(const std::string& text) {
    std::vector<char> data;
    std::vector<int> freqs;
    for (const auto& c : text) {
        auto it = std::find(data.begin(), data.end(), c);
        int index = std::distance(data.begin(), it);
        if (it == data.end()) {
            data.emplace_back(c);
            freqs.emplace_back(1);
        } else {
            ++freqs[index];
        }
    }
    return {data, freqs};
}

minheap_node::ptr huffman_codes(const std::vector<char>& data,
                                const std::vector<int>& freq) {
    minheap heap;

    for (int i = 0; i < data.size(); ++i)
        heap.push(std::make_unique<minheap_node>(data[i], freq[i]));

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

}  // namespace jcb