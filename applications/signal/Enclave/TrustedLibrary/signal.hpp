#pragma once
#include "common/cpp_extended.hpp"
#include "oram/pathoram/oram.hpp"
#include "otree/otree.hpp"

using ORAMClient_t = typename _ORAM::PathORAM::ORAMClient::ORAMClient<_OBST::Node,ORAM__Z,false,ORAM_SERVER__LEVELS_PER_PACK>;
using OramClient_t = typename _OBST::OramClient::OramClient<ORAMClient_t>;
using OBST_t = typename _OBST::OBST::OBST<OramClient_t>;


struct Signal {
  void* client_;
  Signal(uint64_t _maxNumUsers) : client_(nullptr) {
    printf("Initializing signal\n");
    printf("%lld", sizeof(ORAMClient_t::ORAMClientInterface_t::LargeBucket_t));
    printf("Calling constructor: %lld\n", _maxNumUsers);
    client_ = new OBST_t(_maxNumUsers, true);
    printf("Constructor done\n");
  }

  ~Signal() {
    if (client_ != nullptr) {
      OBST_t* client = static_cast<OBST_t*>(client_);
      delete client;
      client_ = nullptr;
    }
  }

  void RegisterUser(uint64_t id) {
    OBST_t* client = static_cast<OBST_t*>(client_);
    uint64_t v1 = 1;
    client->Insert(id, v1);
  }

  bool QueryUser(uint64_t id) {  
    OBST_t* client = static_cast<OBST_t*>(client_);
    uint64_t v1 = 0;
    bool contained = client->Get(id, v1);
    return contained * (v1 == 1);    
  }
};
