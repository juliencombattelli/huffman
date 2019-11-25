#include <benchmark/benchmark.h>
#include <jcbasic/jcbasic.hpp>
#include <jcbasic_fastercount/jcbasic_fastercount.hpp>
#include <jcdod/jcdod.hpp>
#include <jcindex/jcindex.hpp>
#include <jcindex_fastercount/jcindex_fastercount.hpp>
#include <mementar/mementar.hpp>

#include <boost/program_options.hpp>

#include <fstream>
#include <iostream>
#include <random>
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

std::string random_string(std::size_t length, std::uint8_t char_count) {
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, char_count);

    std::string random_string;

    for (std::size_t i = 0; i < length; ++i) {
        random_string += distribution(generator);
    }

    return random_string;
}

// const std::string input = read_file("../resources/input.txt");

std::string input;

static void mementar_bench(benchmark::State& state) {
    std::vector<char> data(input.begin(), input.end());
    int result;
    for (auto _ : state) {
        mementar::Huffman huffman;
        huffman.analyse(data);
        benchmark::DoNotOptimize(result = huffman.generateTree());
        benchmark::ClobberMemory();
    }
}
BENCHMARK(mementar_bench);
/*
static void jcb_bench(benchmark::State& state) {
    for (auto _ : state) {
        auto [data, freq] = jcb::count_char(input);
        auto tree = jcb::get_huffman_tree(data, freq);
    }
}
BENCHMARK(jcb_bench);
*/
static void jcbfc_bench(benchmark::State& state) {
    jcbfc::minheap_node::ptr root;
    for (auto _ : state) {
        auto freq = jcbfc::count_char(input);
        benchmark::DoNotOptimize(root = jcbfc::get_huffman_tree(freq));
        benchmark::ClobberMemory();
    }
}
BENCHMARK(jcbfc_bench);

static void jcbfc_multi_bench(benchmark::State& state) {
    jcbfc::minheap_node::ptr root;
    for (auto _ : state) {
        auto freq = jcbfc::count_char_multi(input);
        benchmark::DoNotOptimize(root = jcbfc::get_huffman_tree(freq));
        benchmark::ClobberMemory();
    }
}
BENCHMARK(jcbfc_multi_bench);
/*
static void jcidx_bench(benchmark::State& state) {
    for (auto _ : state) {
        jcidx::huffman_tree tree(jcidx::reserve_memory::yes);
        auto [data, freq] = tree.count_char(input);
        auto r = tree.generate(data, freq);
    }
}
BENCHMARK(jcidx_bench);

static void jcidxfc_bench(benchmark::State& state) {
    for (auto _ : state) {
        jcidxfc::huffman_tree tree(jcidxfc::reserve_memory::yes);
        auto freq = tree.count_char(input);
        auto r = tree.generate(freq);
    }
}
BENCHMARK(jcidxfc_bench);

static void jcidxfc_multi_bench(benchmark::State& state) {
    for (auto _ : state) {
        jcidxfc::huffman_tree tree(jcidxfc::reserve_memory::yes);
        auto freq = tree.count_char_multi(input);
        auto r = tree.generate(freq);
    }
}
BENCHMARK(jcidxfc_multi_bench);
*/
static void jcdod_bench(benchmark::State& state) {
    size_t root;
    for (auto _ : state) {
        jcdod::huffman_tree tree;
        auto f = tree.count_char(input);
        benchmark::DoNotOptimize(root = tree.generate(f));
        benchmark::ClobberMemory();
    }
}
BENCHMARK(jcdod_bench);

static void jcdod_multi_bench(benchmark::State& state) {
    size_t root;
    for (auto _ : state) {
        jcdod::huffman_tree tree;
        auto f = tree.count_char_multi(input);
        benchmark::DoNotOptimize(root = tree.generate(f));
        benchmark::ClobberMemory();
    }
}
BENCHMARK(jcdod_multi_bench);

int main(int argc, char** argv) {
    std::size_t input_size = 0;
    int char_count = 255;

    ::benchmark::Initialize(&argc, argv);
    // if (::benchmark::ReportUnrecognizedArguments(argc, argv)) {
    //}

    namespace bpo = boost::program_options;
    bpo::options_description desc("Options");
    desc.add_options()                                                   //
        ("input_size,s", bpo::value<decltype(input_size)>(&input_size),  //
         "Size of generated input file in byte")                         //
        ("char_count,c", bpo::value<decltype(char_count)>(&char_count),  //
         "Number of different characters generated");                    //

    bpo::variables_map vm;
    try {
        bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
        bpo::notify(vm);

    } catch (bpo::error& e) {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        std::cerr << desc << std::endl;
        return -1;
    }

    std::clamp(char_count, 0, 255);

    input = random_string(input_size, char_count);

    ::benchmark::RunSpecifiedBenchmarks();
}