#define ENABLE_PROFILING
// #define NO_INMEM_SERVER
// #define ORAM_SERVER__LEVELS_PER_PACK 4
// #define ORAM_SERVER__BUCKET_CACHE_SIZE 8192
// #define ORAM_SERVER__CACHE_SIZE 8192
// #define ORAM_SERVER__DIRECTLY_CACHED_LEVELS 4

#include "oram/pathoram/oram.hpp"
#include "oram/ringoram/oram.hpp"
#include "otree/otree.hpp"
#include <gtest/gtest.h>
#include <map>
using namespace _ORAM;
using namespace std;

using K = _OBST::K;
using V = _OBST::V;

using ORAMClient_t = typename _ORAM::PathORAM::ORAMClient::ORAMClient<_OBST::Node,ORAM__Z,false>;
using OramClient_t = typename _OBST::OramClient::OramClient<ORAMClient_t>;
using OBST = typename _OBST::OBST::OBST<OramClient_t>;
using ORAMServer_t = ORAMClient_t::ORAMClientInterface_t;
using LargeBucket_t = ORAMServer_t::LargeBucket_t;
using Block_t = ORAMClient_t::Block_t;
using StashedBlock_t = ORAMClient_t::StashedBlock_t;
using Bucket_t = ORAMClient_t::Bucket_t;

TEST(Profiling, Sanity) {
  PROFILER_SET(false);
  TRACER_SET(false);

  const int mult = 10;
  const int size = 1 + (1<<20) - 3;

  const K maxK = size * 100;
  std::map<K, V> m;
  OBST t(size);
  K k;
  V v1, v2;


  for (int i = 0; i < std::min(1000, size-1); i++) {
    k = (random() % maxK);
    v1 = random();
    t.Insert(k, v1);
    m[k] = v1;
  }

  const int cycles = 1000 * mult;
  const clock_t t0 = clock();
  for (int i = 0; i < cycles; i++) {
    k = (random() % maxK);
    t.Get(k, v1);
    if (m.count(k) > 0) {
      v2 = m[k];
      (void) (v1 == v2);
    }
  }
  const clock_t t1 = clock();

  PROFILER_RESET(/*log=*/false);
  PROFILER_SET(true);
  t.Insert(0, 73);
  t.Get(0, v1);
  PROFILER_RESET();
  TRACER_RESET();
  PROFILER_SET(false);
}