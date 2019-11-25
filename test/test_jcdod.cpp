#include <experimental/array>
#include <fstream>
#include <jcdod/jcdod.hpp>

std::string read_file(const std::string& filename) {
    std::ifstream input(filename);
    if (not input)
        throw std::runtime_error{"Error while opening file " + filename};
    std::string str;

    input.seekg(0, std::ios::end);
    str.reserve(input.tellg());
    input.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(input)),
               std::istreambuf_iterator<char>());

    return str;
}

const std::string input = read_file("../resources/input.txt");

int main() {
    jcdod::huffman_tree tree;
    auto freq = tree.count_char(input);
    return tree.generate(freq);
}