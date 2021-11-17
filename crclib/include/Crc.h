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

#endif  // CRC_H__
