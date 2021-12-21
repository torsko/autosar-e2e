/**
 * @file Crc32P4.c
 * @author Tomas Wester (torsko@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-11-19
 *
 * -----------------------------------------------------------------------
 * The Crc_CalculateCRC32P4 function is an adaptation of the crcFast function
 * with a preinitialized lookup table from Barr Group's "Free CRC Code in C"
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

static uint32_t crcTable[256] = {
    0x00000000, 0xf4acfb13, 0x1df50d35, 0xe959f626, 0x3bea1a6a, 0xcf46e179, 0x261f175f, 0xd2b3ec4c,
    0x77d434d4, 0x8378cfc7, 0x6a2139e1, 0x9e8dc2f2, 0x4c3e2ebe, 0xb892d5ad, 0x51cb238b, 0xa567d898,
    0xefa869a8, 0x1b0492bb, 0xf25d649d, 0x06f19f8e, 0xd44273c2, 0x20ee88d1, 0xc9b77ef7, 0x3d1b85e4,
    0x987c5d7c, 0x6cd0a66f, 0x85895049, 0x7125ab5a, 0xa3964716, 0x573abc05, 0xbe634a23, 0x4acfb130,
    0x2bfc2843, 0xdf50d350, 0x36092576, 0xc2a5de65, 0x10163229, 0xe4bac93a, 0x0de33f1c, 0xf94fc40f,
    0x5c281c97, 0xa884e784, 0x41dd11a2, 0xb571eab1, 0x67c206fd, 0x936efdee, 0x7a370bc8, 0x8e9bf0db,
    0xc45441eb, 0x30f8baf8, 0xd9a14cde, 0x2d0db7cd, 0xffbe5b81, 0x0b12a092, 0xe24b56b4, 0x16e7ada7,
    0xb380753f, 0x472c8e2c, 0xae75780a, 0x5ad98319, 0x886a6f55, 0x7cc69446, 0x959f6260, 0x61339973,
    0x57f85086, 0xa354ab95, 0x4a0d5db3, 0xbea1a6a0, 0x6c124aec, 0x98beb1ff, 0x71e747d9, 0x854bbcca,
    0x202c6452, 0xd4809f41, 0x3dd96967, 0xc9759274, 0x1bc67e38, 0xef6a852b, 0x0633730d, 0xf29f881e,
    0xb850392e, 0x4cfcc23d, 0xa5a5341b, 0x5109cf08, 0x83ba2344, 0x7716d857, 0x9e4f2e71, 0x6ae3d562,
    0xcf840dfa, 0x3b28f6e9, 0xd27100cf, 0x26ddfbdc, 0xf46e1790, 0x00c2ec83, 0xe99b1aa5, 0x1d37e1b6,
    0x7c0478c5, 0x88a883d6, 0x61f175f0, 0x955d8ee3, 0x47ee62af, 0xb34299bc, 0x5a1b6f9a, 0xaeb79489,
    0x0bd04c11, 0xff7cb702, 0x16254124, 0xe289ba37, 0x303a567b, 0xc496ad68, 0x2dcf5b4e, 0xd963a05d,
    0x93ac116d, 0x6700ea7e, 0x8e591c58, 0x7af5e74b, 0xa8460b07, 0x5ceaf014, 0xb5b30632, 0x411ffd21,
    0xe47825b9, 0x10d4deaa, 0xf98d288c, 0x0d21d39f, 0xdf923fd3, 0x2b3ec4c0, 0xc26732e6, 0x36cbc9f5,
    0xaff0a10c, 0x5b5c5a1f, 0xb205ac39, 0x46a9572a, 0x941abb66, 0x60b64075, 0x89efb653, 0x7d434d40,
    0xd82495d8, 0x2c886ecb, 0xc5d198ed, 0x317d63fe, 0xe3ce8fb2, 0x176274a1, 0xfe3b8287, 0x0a977994,
    0x4058c8a4, 0xb4f433b7, 0x5dadc591, 0xa9013e82, 0x7bb2d2ce, 0x8f1e29dd, 0x6647dffb, 0x92eb24e8,
    0x378cfc70, 0xc3200763, 0x2a79f145, 0xded50a56, 0x0c66e61a, 0xf8ca1d09, 0x1193eb2f, 0xe53f103c,
    0x840c894f, 0x70a0725c, 0x99f9847a, 0x6d557f69, 0xbfe69325, 0x4b4a6836, 0xa2139e10, 0x56bf6503,
    0xf3d8bd9b, 0x07744688, 0xee2db0ae, 0x1a814bbd, 0xc832a7f1, 0x3c9e5ce2, 0xd5c7aac4, 0x216b51d7,
    0x6ba4e0e7, 0x9f081bf4, 0x7651edd2, 0x82fd16c1, 0x504efa8d, 0xa4e2019e, 0x4dbbf7b8, 0xb9170cab,
    0x1c70d433, 0xe8dc2f20, 0x0185d906, 0xf5292215, 0x279ace59, 0xd336354a, 0x3a6fc36c, 0xcec3387f,
    0xf808f18a, 0x0ca40a99, 0xe5fdfcbf, 0x115107ac, 0xc3e2ebe0, 0x374e10f3, 0xde17e6d5, 0x2abb1dc6,
    0x8fdcc55e, 0x7b703e4d, 0x9229c86b, 0x66853378, 0xb436df34, 0x409a2427, 0xa9c3d201, 0x5d6f2912,
    0x17a09822, 0xe30c6331, 0x0a559517, 0xfef96e04, 0x2c4a8248, 0xd8e6795b, 0x31bf8f7d, 0xc513746e,
    0x6074acf6, 0x94d857e5, 0x7d81a1c3, 0x892d5ad0, 0x5b9eb69c, 0xaf324d8f, 0x466bbba9, 0xb2c740ba,
    0xd3f4d9c9, 0x275822da, 0xce01d4fc, 0x3aad2fef, 0xe81ec3a3, 0x1cb238b0, 0xf5ebce96, 0x01473585,
    0xa420ed1d, 0x508c160e, 0xb9d5e028, 0x4d791b3b, 0x9fcaf777, 0x6b660c64, 0x823ffa42, 0x76930151,
    0x3c5cb061, 0xc8f04b72, 0x21a9bd54, 0xd5054647, 0x07b6aa0b, 0xf31a5118, 0x1a43a73e, 0xeeef5c2d,
    0x4b8884b5, 0xbf247fa6, 0x567d8980, 0xa2d17293, 0x70629edf, 0x84ce65cc, 0x6d9793ea, 0x993b68f9
};

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

// cppcheck-suppress unusedFunction
uint32_t Crc_CalculateCRC32P4(const uint8_t* Crc_DataPtr,
                              uint32_t Crc_Length,
                              uint32_t Crc_StartValue32,
                              bool Crc_IsFirstCall) {
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
     * Divide the message by the polynomial, a byte at a time.
     */
    for (size_t byte = 0; byte < Crc_Length; ++byte)
    {
        uint8_t data = reflect(Crc_DataPtr[byte], 8) ^ (remainder >> 24);
        remainder = crcTable[data] ^ (remainder << 8);
    }

    /*
     * The final remainder is the CRC result.
     */
    return reflect(remainder, 32) ^ 0xFFFFFFFF;
}
