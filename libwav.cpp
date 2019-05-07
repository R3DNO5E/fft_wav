//
// Created by shinku on 5/7/19.
//

#include "libwav.hpp"

MSWavFile::MSWavFile() {

}

MSWavFile::MSWavFile(std::string path) {
    LoadFile(path);
}

int MSWavFile::LoadFile(std::string path) {
    FILE* fp = fopen(path.c_str(),"rb");
    if(fp == nullptr) return -1;
    fseek(fp,0,SEEK_END);
    long fsize =  ftell(fp);
    rawfile = (uint8_t*)malloc(fsize);
    if(rawfile == nullptr) return -1;
    rewind(fp);
    if(fread(rawfile,fsize,1,fp) != 1) return -1;
    RIFF_Header_t* riff = (RIFF_Header_t*)rawfile;
    if(strncmp(riff->ChunkID,"RIFF",4) != 0 || strncmp(riff->Format,"WAVE",4) != 0) return -2;
    void* cur = rawfile + sizeof(RIFF_Header_t);
    while(cur < rawfile + riff->ChunkSize + 8) {
        if(strncmp(((Subchunk_Header_t*)cur)->SubchunkID,"fmt ",4) == 0) {
            FMT_Subchunk_Header_t* fmt = (FMT_Subchunk_Header_t*)cur;
            if(fmt->AudioFormat != 1) return -2;
            channels = fmt->NumChannels;
            samplerate = fmt->SampleRate;
            bitspersample = fmt->BitsPerSample;
        }
        if(strncmp(((Subchunk_Header_t*)cur)->SubchunkID,"data",4) == 0) {
            DATA_Subchunk_Header_t* data = (DATA_Subchunk_Header_t*)cur;
            samplecount = data->Subchunk_Header.SubchunkSize;
            rawdata = data->data;
        }
        cur = (uint8_t*)cur + ((Subchunk_Header_t*)cur)->SubchunkSize + 8;
    }
    if(channels != -1 && samplerate != -1 && bitspersample != -1 && samplecount != -1 && rawdata != nullptr) {
        samplecount = samplecount / ((bitspersample/8) * channels);
    } else return -3;
    loaded = true;
    return samplecount;
}

int MSWavFile::getBitsPerSample() {
    return bitspersample;
}

int MSWavFile::getChannels() {
    return channels;
}

void* MSWavFile::getRawBuffer() {
    return rawdata;
}

long MSWavFile::getSampleCount() {
    return samplecount;
}

long MSWavFile::getSampleRate() {
    return samplerate;
}
