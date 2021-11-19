#pragma once

#include <array>

/**
 * Testdata for CRC-calculation tests as provided in:
 * https://www.autosar.org/fileadmin/user_upload/standards/classic/20-11/AUTOSAR_SWS_CRCLibrary.pdf
 *
 */

namespace {
    const std::array<uint8_t, 4> sample1{0, 0, 0, 0};
    const uint8_t sample1_crc8{0x59};
    const uint8_t sample1_crc8H2F{0x12};
    const uint16_t sample1_crc16{0x84C0};
    const uint16_t sample1_crc16ARC{0x0000};
    const uint32_t sample1_crc32{0x2144DF1C};
    const uint32_t sample1_crc32P4{0x6FB32240};
    const uint64_t sample1_crc64{0xF4A586351E1B9F4B};

    const std::array<uint8_t, 3> sample2{0xF2, 0x01, 0x83};
    const uint8_t sample2_crc8{0x37};
    const uint8_t sample2_crc8H2F{0xC2};
    const uint16_t sample2_crc16{0xD374};
    const uint16_t sample2_crc16ARC{0xC2E1};
    const uint32_t sample2_crc32{0x24AB9D77};
    const uint32_t sample2_crc32P4{0x4F721A25};
    const uint64_t sample2_crc64{0x319C27668164F1C6};

    const std::array<uint8_t, 4> sample3{0x0F, 0xAA, 0x00, 0x55};
    const uint8_t sample3_crc8{0x79};
    const uint8_t sample3_crc8H2F{0xC6};
    const uint16_t sample3_crc16{0x2023};
    const uint16_t sample3_crc16ARC{0x0BE3};
    const uint32_t sample3_crc32{0xB6C9B287};
    const uint32_t sample3_crc32P4{0x20662DF8};
    const uint64_t sample3_crc64{0x54C5D0F7667C1575};

    const std::array<uint8_t, 4> sample4{0x00, 0xFF, 0x55, 0x11};
    const uint8_t sample4_crc8{0xB8};
    const uint8_t sample4_crc8H2F{0x77};
    const uint16_t sample4_crc16{0xB8F9};
    const uint16_t sample4_crc16ARC{0x6CCF};
    const uint32_t sample4_crc32{0x32A06212};
    const uint32_t sample4_crc32P4{0x9BD7996E};
    const uint64_t sample4_crc64{0xA63822BE7E0704E6};

    const std::array<uint8_t, 9> sample5{0x33, 0x22, 0x55, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    const uint8_t sample5_crc8{0xCB};
    const uint8_t sample5_crc8H2F{0x11};
    const uint16_t sample5_crc16{0xF53F};
    const uint16_t sample5_crc16ARC{0xAE98};
    const uint32_t sample5_crc32{0xB0AE863D};
    const uint32_t sample5_crc32P4{0xA65A343D};
    const uint64_t sample5_crc64{0x701ECEB219A8E5D5};

    const std::array<uint8_t, 3> sample6{0x92, 0x6B, 0x55};
    const uint8_t sample6_crc8{0x8C};
    const uint8_t sample6_crc8H2F{0x33};
    const uint16_t sample6_crc16{0x0745};
    const uint16_t sample6_crc16ARC{0xE24E};
    const uint32_t sample6_crc32{0x9CDEA29B};
    const uint32_t sample6_crc32P4{0xEE688A78};
    const uint64_t sample6_crc64{0x5FAA96A9B59F3E4E};

    const std::array<uint8_t, 4> sample7{0xFF, 0xFF, 0xFF, 0xFF};
    const uint8_t sample7_crc8{0x74};
    const uint8_t sample7_crc8H2F{0x6C};
    const uint16_t sample7_crc16{0x1D0F};
    const uint16_t sample7_crc16ARC{0x9401};
    const uint32_t sample7_crc32{0xFFFFFFFF};
    const uint32_t sample7_crc32P4{0xFFFFFFFF};
    const uint64_t sample7_crc64{0xFFFFFFFF00000000};
}
