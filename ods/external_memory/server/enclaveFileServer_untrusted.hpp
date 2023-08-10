#pragma once
#include <cinttypes>
#include <fstream>

#include "common/cpp_extended.hpp"

#define DEFAULT_FILENAME "/ssdmount/storage.bin"

static uint64_t N;
static std::unique_ptr<std::fstream> lbios(nullptr);

uint8_t* ocall_InitServer(uint64_t sizeOfT, uint64_t N_) {
  // printf("init file server\n");
  lbios.reset(new std::fstream());
  N = N_;
  lbios->open(DEFAULT_FILENAME, std::fstream::in | std::fstream::out |
                                    std::fstream::binary | std::fstream::trunc);
  Assert(lbios->is_open());
  // for (uint64_t i = 0; i < N; i++) {
  //   uint8_t zeros[sizeOfT];
  //   memset(zeros, 0, sizeOfT);
  //   lbios->write((char*)zeros, sizeOfT);
  // }
  // Assert(lbios->is_open());
  // lbios->flush();
  X_LOG("ORAMClientInterface: Done allocating file (", lbios->tellp(),
        " bytes written)");
  return NULL;
}

void ocall_Read(size_t pos, uint64_t length, uint8_t* page) {
  Assert(length <= 4096);
  std::streampos filePos = pos;
  lbios->seekg(filePos);
  lbios->read((char*)page, length);
}

void ocall_Write(uint64_t pos, uint64_t length, const uint8_t* page) {
  std::streampos filePos = pos;
  lbios->seekp(filePos);
  lbios->write((char*)page, length);
}

uint64_t compressChunks(uint64_t* offsets, uint64_t* sizes, uint64_t chunkNum) {
  if (!chunkNum) {
    return 0;
  }
  uint64_t endOffset = *offsets + *sizes;
  uint64_t j = 0;
  for (uint64_t i = 1; i < chunkNum; ++i) {
    uint64_t offset = *(offsets + i);
    uint64_t size = *(sizes + i);
    if (offset != endOffset) {
      sizes[j] = endOffset - offsets[j];
      offsets[++j] = offset;
    }
    endOffset = offset + size;
  }
  sizes[j] = endOffset - offsets[j];
  return j + 1;
}

void ocall_Read_Batch(uint64_t* offsets, uint64_t* sizes, uint8_t* tmp,
                      uint64_t chunkNum, uint64_t totalSize) {
  // printf("batch read %ld chunks of total size %ld\n", chunkNum, totalSize);
  uint8_t* pos = tmp;
  chunkNum = compressChunks(offsets, sizes, chunkNum);
  for (uint64_t i = 0; i < chunkNum; ++i) {
    uint64_t offset = *(offsets + i);
    uint64_t size = *(sizes + i);
    ocall_Read(offset, size, pos);
    pos += size;
  }
}
void ocall_Write_Batch(uint64_t* offsets, uint64_t* sizes, uint8_t* tmp,
                       uint64_t chunkNum, uint64_t totalSize) {
  //  printf("batch write %ld chunks of total size %ld\n", chunkNum, totalSize);
  uint8_t* pos = tmp;
  chunkNum = compressChunks(offsets, sizes, chunkNum);
  for (uint64_t i = 0; i < chunkNum; ++i) {
    uint64_t offset = *(offsets + i);
    uint64_t size = *(sizes + i);
    ocall_Write(offset, size, pos);
    pos += size;
  }
}

void ocall_DeleteServer() { lbios->close(); }