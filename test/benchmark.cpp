#include <benchmark/benchmark.h>
#include <jcbasic/jcbasic.hpp>
#include <jcbasic_fastercount/jcbasic_fastercount.hpp>
#include <jcindex/jcindex.hpp>
#include <jcindex_fastercount/jcindex_fastercount.hpp>
#include <mementar/mementar.hpp>

#include <fstream>
#include <iostream>
#include <stdexcept>

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

static void mementar_bench(benchmark::State& state) {
    std::vector<char> data(input.begin(), input.end());
    for (auto _ : state) {
        mementar::Huffman huffman;
        huffman.analyse(data);
        huffman.generateTree();
    }
}
BENCHMARK(mementar_bench);

static void jcb_bench(benchmark::State& state) {
    for (auto _ : state) {
        auto [data, freq] = jcb::count_char(input);
        auto tree = jcb::get_huffman_tree(data, freq);
    }
}
BENCHMARK(jcb_bench);

static void jcbfc_bench(benchmark::State& state) {
    for (auto _ : state) {
        auto freq = jcbfc::count_char(input);
        auto tree = jcbfc::get_huffman_tree(freq);
    }
}
BENCHMARK(jcbfc_bench);

static void jcidx_bench(benchmark::State& state) {
    for (auto _ : state) {
        jcidx::huffman_tree tree(jcidx::reserve_memory::no);
        auto [data, freq] = tree.count_char(input);
        auto r = tree.generate(data, freq);
    }
}
BENCHMARK(jcidx_bench);

static void jcidxfc_bench(benchmark::State& state) {
    for (auto _ : state) {
        jcidxfc::huffman_tree tree(jcidxfc::reserve_memory::no);
        auto freq = tree.count_char(input);
        auto r = tree.generate(freq);
    }
}
BENCHMARK(jcidxfc_bench);