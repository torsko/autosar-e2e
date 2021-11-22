/**
 * @file Crc32.c
 * @author Tomas Wester (torsko@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-11-16
 *
 * -----------------------------------------------------------------------
 *
 * The Crc_CalculateCRC32 function is an adaptation of the crcSlow function
 * from Barr Group's "Free CRC Code in C"
 * https://barrgroup.com/downloads/code-crc-c
 *
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 *
 * -----------------------------------------------------------------------
 *
 */

#include "Crc.h"

static uint32_t reflect(uint32_t data, uint8_t nBits) {
    uint32_t  reflection = 0x00000000;
	uint8_t  bit;

	/*
	 * Reflect the data about the center bit.
	 */
	for (bit = 0; bit < nBits; ++bit)
	{
		/*
		 * If the LSB bit is set, set the reflection of it.
		 */
		if (data & 0x01)
		{
			reflection |= (1 << ((nBits - 1) - bit));
		}

		data = (data >> 1);
	}

	return (reflection);
}

uint32_t Crc_CalculateCRC32(const uint8_t* Crc_DataPtr,
                          uint32_t Crc_Length,
                          uint32_t Crc_StartValue32,
                          bool Crc_IsFirstCall) {
    const uint32_t topbit = 0x80000000;
    const uint32_t polynomial = 0x04C11DB7;
    uint32_t remainder = 0;

    if (Crc_IsFirstCall) {
        // SWS_Crc_00014
        remainder = 0xFFFFFFFF;
    } else {
        // SWS_Crc_00041
        remainder = (Crc_StartValue32 ^ 0xFFFFFFFF);
        remainder = reflect(remainder, 32);
    }

    /*
     * Perform modulo-2 division, a byte at a time.
     */
    for (size_t byte = 0; byte < Crc_Length; ++byte)
    {
        /*
         * Bring the next byte into the remainder.
         */
        remainder ^= reflect(Crc_DataPtr[byte], 8) << 24;

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (uint8_t bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
            if (remainder & topbit)
            {
                remainder = (remainder << 1) ^ polynomial;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }

    /*
     * The final remainder is the CRC result.
     */
    return reflect(remainder, 32) ^ 0xFFFFFFFF;
}
