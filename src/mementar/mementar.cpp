#include <mementar/mementar.hpp>

#include <algorithm>
#include <iostream>

#define TREE_CHAR_SIZE 8
#define TREE_VALUE_SIZE 6
#define TREE_VALUE_SIZE_SIZE 31  // do not go over 31

namespace mementar {

void print_tree(HuffNode_t* root, std::string str) {
    if (!root)
        return;
    if (root->data_ != ' ')
        std::cerr << root->data_ << ": " << str << "\n";
    print_tree(root->left_, str + "1");
    print_tree(root->right_, str + "0");
}

bool comparePtrNode(HuffNode_t* a, HuffNode_t* b) {
    return (a->freq_ < b->freq_);
}

bool comparePtrNodeByCode(HuffNode_t* a, HuffNode_t* b) {
    if (a->code_.size_ < b->code_.size_)
        return false;
    else if (a->code_.size_ > b->code_.size_)
        return true;
    else if (a->code_.value_ < b->code_.value_)
        return true;
    else
        return false;
}

Huffman::~Huffman() {
    for (auto it : heap_)
        delete it;
}

void Huffman::analyse(const std::vector<char>& data) {
    for (const auto& c : data) {
        auto it = leaf_map_.find(c);
        if (it == leaf_map_.end()) {
            HuffNode_t* leaf = new HuffNode_t(c);
            heap_.push_back(leaf);
            leaf_map_[c] = leaf;
        } else
            it->second->freq_++;
    }
}

int Huffman::generateTree() {
    while (heap_.size() > 1) {
        std::sort(heap_.begin(), heap_.end(), comparePtrNode);

        HuffNode_t* tmp = new HuffNode_t();
        tmp->right_ = heap_[0];
        tmp->left_ = heap_[1];
        tmp->freq_ = tmp->right_->freq_ + tmp->left_->freq_;
        heap_.erase(heap_.begin());
        heap_.erase(heap_.begin());
        heap_.push_back(tmp);
    }

    heap_[0]->code_.value_ = 0;
    heap_[0]->code_.size_ = 0;
    generateCode(heap_[0]);
    return heap_[0]->data_;
}

void Huffman::generateCode(HuffNode_t* node) {
    if (node->right_ != nullptr) {
        node->right_->code_.value_ = (node->code_.value_ << 1);
        node->right_->code_.size_ = node->code_.size_ + 1;
        generateCode(node->right_);
    }
    if (node->left_ != nullptr) {
        node->left_->code_.value_ = (node->code_.value_ << 1) | 0x01;
        node->left_->code_.size_ = node->code_.size_ + 1;
        generateCode(node->left_);
    }
}

void Huffman::getTreeCode(std::vector<char>& out) {}

void Huffman::getDataCode(std::vector<char>& data, std::vector<char>& out) {}

size_t Huffman::setTree(std::vector<char>& in) {
    return 0;
}

void Huffman::getFile(std::vector<char>& data, std::string& out) {}

}  // namespace mementar