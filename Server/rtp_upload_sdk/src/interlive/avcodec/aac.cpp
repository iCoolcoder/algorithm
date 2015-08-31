#include "aac.h"
#include <stdio.h>

namespace avcodec
{
    static unsigned const samplingFrequencyTable[16] = 
    {
        96000, 88200, 64000, 48000,
        44100, 32000, 24000, 22050,
        16000, 12000, 11025, 8000,
        7350, 0, 0, 0
    };

    uint8_t aac_get_samplingFrequencyIndex(uint32_t samplerate)
    {
        for (uint8_t i = 0; i < sizeof(samplingFrequencyTable); i++)
        {
            if (samplerate >= samplingFrequencyTable[i])
            {
                return i;
            }
        }
    }

    AACConfig::AACConfig()
    {
        audioSpecificConfig[0] = 0;
        audioSpecificConfig[1] = 0;
    }

    AACConfig::AACConfig(uint8_t* input)
    {
        if (input == NULL)
        {
            audioSpecificConfig[0] = 0;
            audioSpecificConfig[1] = 0;
        }
        else
        {
            audioSpecificConfig[0] = input[0];
            audioSpecificConfig[1] = input[1];
        }
    }

    /*
        0000 0|000 0|000 0|000

        audioObjectType: 5 bits
        samplingFrequencyIndex : 4 bits
        channels : 4 bits
        GASpecificConfig
        {
            frameLengthFlag : 1 bit
            dependsOnCoreCoder : 1 bit
            extensionFlag : 1 bit
        }
    */

    uint8_t AACConfig::set_audioObjectType(uint8_t audioObjectType)
    {
        audioSpecificConfig[0] &= 0x07; // 0000 0111
        audioSpecificConfig[0] |= audioObjectType << 3;

        return audioObjectType;
    }

    uint8_t AACConfig::set_samplingFrequencyIndex(uint8_t samplingFrequencyIndex)
    {
        audioSpecificConfig[0] &= 0xf8; // 1111 1000 0111 1111
        audioSpecificConfig[1] &= 0x7f;

        audioSpecificConfig[0] |= samplingFrequencyIndex >> 1;
        audioSpecificConfig[1] |= samplingFrequencyIndex << 7;

        return samplingFrequencyIndex;
    }

    uint8_t AACConfig::set_channelConfig(uint8_t channels)
    {
        audioSpecificConfig[1] &= 0x87; // 1000 0111
        audioSpecificConfig[1] |= channels << 3;

        return channels;
    }
}
