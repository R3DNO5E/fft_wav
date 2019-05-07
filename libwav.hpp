//
// Created by shinku on 5/7/19.
//

#ifndef FFT_WAV_LIBWAV_HPP
#define FFT_WAV_LIBWAV_HPP

#include <string>
#include <cstdint>
#include <cstdio>
#include <cstring>

class MSWavFile {
private:
    typedef struct {
        char ChunkID[4];
        uint32_t ChunkSize;
        char Format[4];
    } RIFF_Header_t;
    typedef struct {
        char SubchunkID[4];
        uint32_t SubchunkSize;
    } Subchunk_Header_t;
    typedef struct {
        Subchunk_Header_t Subchunk_Header;
        uint16_t AudioFormat;
        uint16_t NumChannels;
        uint32_t SampleRate;
        uint32_t ByteRate;
        uint16_t BlockAlign;
        uint16_t BitsPerSample;
    } FMT_Subchunk_Header_t;
    typedef struct {
        Subchunk_Header_t Subchunk_Header;
        uint8_t data[];
    } DATA_Subchunk_Header_t;
    bool loaded = false;
    uint8_t* rawfile = nullptr;
    uint8_t* rawdata = nullptr;
    int bitspersample = -1;
    int channels = -1;
    long samplerate = -1;
    long samplecount = -1;
public:
    MSWavFile();
    MSWavFile(std::string path);
    int LoadFile(std::string path);
    long getSampleCount();
    long getSampleRate();
    int getBitsPerSample();
    int getChannels();
    void* getRawBuffer();
};

#endif //FFT_WAV_LIBWAV_HPP
