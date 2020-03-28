//=============================================================================
// Name      : mementar.hpp
// Author    : Julien Combattelli, Guillaume Sarthou
// EMail     : julien.combattelli@gmail.com
// Copyright : This file is a rewrite of mementar Huffman compression module.
//             Mementar is under LGPL-3.0 license.
//             See https://github.com/sarthou/mementar for more information.
//=============================================================================
#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

namespace mementar {

struct HuffCode_t {
    uint32_t value_;
    uint8_t size_;
};

struct HuffNode_t {
    size_t freq_;
    char data_;
    HuffCode_t code_;
    HuffNode_t* left_;
    HuffNode_t* right_;

    HuffNode_t() {
        freq_ = 1;
        data_ = ' ';
        left_ = right_ = nullptr;
    }
    HuffNode_t(char data) {
        freq_ = 1;
        data_ = data;
        left_ = right_ = nullptr;
    }

    ~HuffNode_t() {
        if (left_ != nullptr)
            delete left_;
        if (right_ != nullptr)
            delete right_;
    }
};

class Huffman {
public:
    void analyse(const std::vector<char>& data);
    int generateTree();

    void getTreeCode(std::vector<char>& out);
    void getDataCode(std::vector<char>& data, std::vector<char>& out);
    HuffNode_t* getTreeRoot() { return heap_[0]; }
    size_t setTree(std::vector<char>& in);
    void getFile(std::vector<char>& data, std::string& out);

    ~Huffman();

private:
    std::vector<HuffNode_t*> heap_;
    std::map<char, HuffNode_t*> leaf_map_;

    void generateCode(HuffNode_t* node);
};

void print_tree(HuffNode_t* root, std::string str = "");

}  // namespace mementar
