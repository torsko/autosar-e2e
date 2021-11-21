#ifndef CRC_H__
#define CRC_H__

/**
 * @file Crc.h
 * @author Tomas Wester (torsko@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-11-14
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "Std_Types.h"

/**
 * This service makes a CRC8 calculation on Crc_Length data bytes, with SAE J1850
 * parameters
 *
 * 8-bit SAE J1850 CRC Calculation
 *
 * |                        |         |
 * |:-----------------------|:--------|
 * | CRC result width:      | 8 bits  |
 * | Polynomial:            | 1Dh     |
 * | Initial value:         | FFh     |
 * | Input data reflected:  | No      |
 * | Result data reflected: | No      |
 * | XOR value:             | FFh     |
 * | Check:                 | 4Bh     |
 * | Magic check:           | C4h     |
 *
 * @param Crc_DataPtr       Pointer to start address of data block to be calculated.
 * @param Crc_Length        Length of data block to be calculated in bytes.
 * @param Crc_StartValue8   Start value when the algorithm starts.
 * @param Crc_IsFirstCall   TRUE: First call in a sequence or individual CRC calculation; start from
 *                          initial value, ignore Crc_StartValue8. FALSE: Subsequent call in a call
 *                          sequence; Crc_StartValue8 is interpreted to be the return value of the
 *                          previous function call.
 * @return uint8_t
 */
uint8_t Crc_CalculateCRC8(const uint8_t* Crc_DataPtr,
                          uint32_t Crc_Length,
                          uint8_t Crc_StartValue8,
                          bool Crc_IsFirstCall);


/**
 * This service makes a CRC8 calculation with the Polynomial 0x2F on Crc_Length
 *
 * 8-bit 0x2F polynomial CRC Calculation
 *
 * |                        |         |
 * |:-----------------------|:--------|
 * | CRC result width:      | 8 bits  |
 * | Polynomial:            | 2Fh     |
 * | Initial value:         | FFh     |
 * | Input data reflected:  | No      |
 * | Result data reflected: | No      |
 * | XOR value:             | FFh     |
 * | Check:                 | DFh     |
 * | Magic check:           | 42h     |
 *
 * @param Crc_DataPtr           Pointer to start address of data block to be calculated.
 * @param Crc_Length            Length of data block to be calculated in bytes.
 * @param Crc_StartValue8H2F    Start value when the algorithm starts.
 * @param Crc_IsFirstCall       TRUE: First call in a sequence or individual CRC calculation; start
 *                              from initial value, ignore Crc_StartValue8H2F. FALSE: Subsequent
 *                              call in a call sequence; Crc_StartValue8 is interpreted to be the
 *                              return value of the previous function call.
 * @return uint8_t
 */
uint8_t Crc_CalculateCRC8H2F(const uint8_t* Crc_DataPtr,
                             uint32_t Crc_Length,
                             uint8_t Crc_StartValue8H2F,
                             bool Crc_IsFirstCall);


/**
 * This service makes a CRC16 calculation on Crc_Length data bytes
 *
 * 16-bit CCITT-FALSE CRC16
 *
 * |                        |         |
 * |:-----------------------|:--------|
 * | CRC result width:      | 16 bits |
 * | Polynomial:            | 1021h   |
 * | Initial value:         | FFFFh   |
 * | Input data reflected:  | No      |
 * | Result data reflected: | No      |
 * | XOR value:             | 0000h   |
 * | Check:                 | 29B1h   |
 * | Magic check:           | 0000h   |
 *
 * @param Crc_DataPtr       Pointer to start address of data block to be calculated.
 * @param Crc_Length        Length of data block to be calculated in bytes.
 * @param Crc_StartValue16  Start value when the algorithm starts.
 * @param Crc_IsFirstCall   TRUE: First call in a sequence or individual CRC calculation; start from
 *                          initial value, ignore Crc_StartValue16. FALSE: Subsequent call in a call
 *                          sequence; Crc_StartValue16 is interpreted to be the return value of the
 *                          previous function call.
 * @return uint16_t
 */
uint16_t Crc_CalculateCRC16(const uint8_t* Crc_DataPtr,
                          uint32_t Crc_Length,
                          uint16_t Crc_StartValue16,
                          bool Crc_IsFirstCall);


/**
 * This service makes a CRC16 calculation on Crc_Length data bytes, using the poly-
 * nomial 0x8005.
 *
 * 16-bit 0x8005 polynomial CRC calculation
 *
 * |                        |         |
 * |:-----------------------|:--------|
 * | CRC result width:      | 16 bits |
 * | Polynomial:            | 8005h   |
 * | Initial value:         | 0000h   |
 * | Input data reflected:  | Yes     |
 * | Result data reflected: | Yes     |
 * | XOR value:             | 0000h   |
 * | Check:                 | BB3Dh   |
 * | Magic check:           | 0000h   |
 *
 * @param Crc_DataPtr       Pointer to start address of data block to be calculated.
 * @param Crc_Length        Length of data block to be calculated in bytes.
 * @param Crc_StartValue16  Start value when the algorithm starts.
 * @param Crc_IsFirstCall   TRUE: First call in a sequence or individual CRC calculation; start from
 *                          initial value, ignore Crc_StartValue16. FALSE: Subsequent call in a call
 *                          sequence; Crc_StartValue16 is interpreted to be the return value of the
 *                          previous function call.
 * @return uint16_t
 */
uint16_t Crc_CalculateCRC16ARC(const uint8_t* Crc_DataPtr,
                               uint32_t Crc_Length,
                               uint16_t Crc_StartValue16,
                               bool Crc_IsFirstCall);


/**
 * This service makes a CRC32 calculation on Crc_Length data bytes.
 *
 * 32-bit Ethernet CRC Calculation
 *
 * |                        |           |
 * |:-----------------------|:----------|
 * | CRC result width:      | 32 bits   |
 * | Polynomial:            | 04C11DB7h |
 * | Initial value:         | FFFFFFFFh |
 * | Input data reflected:  | Yes       |
 * | Result data reflected: | Yes       |
 * | XOR value:             | FFFFFFFFh |
 * | Check:                 | CBF43926h |
 * | Magic check:           | DEBB20E3h |
 *
 * @param Crc_DataPtr       Pointer to start address of data block to be calculated.
 * @param Crc_Length        Length of data block to be calculated in bytes.
 * @param Crc_StartValue32  Start value when the algorithm starts.
 * @param Crc_IsFirstCall   TRUE: First call in a sequence or individual CRC calculation; start from
 *                          initial value, ignore Crc_StartValue32. FALSE: Subsequent call in a call
 *                          sequence; Crc_StartValue32 is interpreted to be the return value of the
 *                          previous function call.
 * @return uint32_t
 */
uint32_t Crc_CalculateCRC32(const uint8_t* Crc_DataPtr,
                            uint32_t Crc_Length,
                            uint32_t Crc_StartValue32,
                            bool Crc_IsFirstCall);

/**
 * This service makes a CRC32 calculation on Crc_Length data bytes, using the poly-
 * nomial 0xF4ACFB13.
 * This CRC routine is used by E2E Profile 4.
 *
 * 32-bit 0xF4ACFB13 polynomial CRC calculation
 *
 * |                        |           |
 * |:-----------------------|:----------|
 * | CRC result width:      | 32 bits   |
 * | Polynomial:            | F4ACFB13h |
 * | Initial value:         | FFFFFFFFh |
 * | Input data reflected:  | Yes       |
 * | Result data reflected: | Yes       |
 * | XOR value:             | FFFFFFFFh |
 * | Check:                 | 1697D06Ah |
 * | Magic check:           | 904CDDBFh |
 *
 * @param Crc_DataPtr       Pointer to start address of data block to be calculated.
 * @param Crc_Length        Length of data block to be calculated in bytes.
 * @param Crc_StartValue32  Start value when the algorithm starts.
 * @param Crc_IsFirstCall   TRUE: First call in a sequence or individual CRC calculation; start from
 *                          initial value, ignore Crc_StartValue32. FALSE: Subsequent call in a call
 *                          sequence; Crc_StartValue32 is interpreted to be the return value of the
 *                          previous function call.
 * @return uint32_t
 */
uint32_t Crc_CalculateCRC32P4(const uint8_t* Crc_DataPtr,
                              uint32_t Crc_Length,
                              uint32_t Crc_StartValue32,
                              bool Crc_IsFirstCall);



/**
 * This service makes a CRC64 calculation on Crc_Length data bytes, using the poly-
 * nomial 0x42F0E1EBA9EA3693.
 * This CRC routine is used by E2E Profile 7.
 *
 * 64-bit 0x42F0E1EBA9EA3693 polynomial CRC calculation
 *
 * |                        |                   |
 * |:-----------------------|:------------------|
 * | CRC result width:      | 64 bits           |
 * | Polynomial:            | 42F0E1EBA9EA3693h |
 * | Initial value:         | FFFFFFFFFFFFFFFFh |
 * | Input data reflected:  | Yes               |
 * | Result data reflected: | Yes               |
 * | XOR value:             | FFFFFFFFFFFFFFFFh |
 * | Check:                 | 995DC9BBDF1939FAh |
 * | Magic check:           | 49958C9ABD7D353Fh |
 *
 * @param Crc_DataPtr       Pointer to start address of data block to be calculated.
 * @param Crc_Length        Length of data block to be calculated in bytes.
 * @param Crc_StartValue64  Start value when the algorithm starts.
 * @param Crc_IsFirstCall   TRUE: First call in a sequence or individual CRC calculation; start from
 *                          initial value, ignore Crc_StartValue64. FALSE: Subsequent call in a call
 *                          sequence; Crc_StartValue64 is interpreted to be the return value of the
 *                          previous function call.
 * @return uint64_t
 */
uint64_t Crc_CalculateCRC64(const uint8_t* Crc_DataPtr,
                            uint32_t Crc_Length,
                            uint64_t Crc_StartValue64,
                            bool Crc_IsFirstCall);

#endif  // CRC_H__
