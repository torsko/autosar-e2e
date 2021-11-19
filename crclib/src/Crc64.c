/**
 * @file Crc64.c
 * @author Tomas Wester (torsko@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-11-16
 *
 * -----------------------------------------------------------------------
 * The Crc_CalculateCRC64 function is an adaptation of the crcSlow function
 * from Barr Group's "Free CRC Code in C"
 * https://barrgroup.com/downloads/code-crc-c
 *
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 * -----------------------------------------------------------------------
 *
 */

#include "Crc.h"

static uint64_t reflect(uint64_t data, uint8_t nBits) {
    uint64_t  reflection = 0x0000000000000000;
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
            /**
             * Note the '1ULL' integer literal
             * The type of the result of a bit-shift operation is the same
             * as the type of the left-hand operator.
             */
			reflection |= (1ULL << ((nBits - 1) - bit));
		}

		data = (data >> 1);
	}

	return (reflection);
}

uint64_t Crc_CalculateCRC64(const uint8_t* Crc_DataPtr,
                          uint32_t Crc_Length,
                          uint64_t Crc_StartValue64,
                          bool Crc_IsFirstCall) {
    const uint64_t topbit = 0x8000000000000000;
    const uint64_t polynomial = 0x42F0E1EBA9EA3693;
    uint64_t remainder = 0;

    if (Crc_IsFirstCall) {
        // SWS_Crc_00014
        remainder = 0xFFFFFFFFFFFFFFFF;
    } else {
        // SWS_Crc_00041
        remainder = (Crc_StartValue64 ^ 0xFFFFFFFFFFFFFFFF);
        remainder = reflect(remainder, 64);
    }

    /*
     * Perform modulo-2 division, a byte at a time.
     */
    for (size_t byte = 0; byte < Crc_Length; ++byte)
    {
        /*
         * Bring the next byte into the remainder.
         */
        remainder ^= reflect(Crc_DataPtr[byte], 8) << 56;

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
    return reflect(remainder, 64) ^ 0xFFFFFFFFFFFFFFFF;
}
