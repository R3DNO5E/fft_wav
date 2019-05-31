#include <iostream>
#include <utility>
#include <vector>
#include "libwav.hpp"
#include "libplot.hpp"
#include <alsa/asoundlib.h>
#include <fftw3.h>
#include <cmath>

#define assert_error {cout << "Error: " << __FILE__ << ":" << __LINE__ << endl;return -1;}

using std::cout;
using std::endl;
using std::min;

constexpr int fft_n = 16384;

int main(int args,char** argc) {
    if(args < 3) {
        cout << "Invalid argument" << endl;
        assert_error
    }
    PlotWindow plot(1600,900);
    //PlotWindow plot(1920,1080);
    plot.SetGrid(40,14000,0,0,10,10,PlotWindow::GridMode::Logarithm,PlotWindow::GridMode::Logarithm);
    if(!plot.isReady()) assert_error
    std::vector<std::pair<double,double>> dat,dat2;
    MSWavFile wav(argc[1]);
    cout << "SampleRate: " << wav.getSampleRate() << " Hz" << endl;
    cout << "Channels: " << wav.getChannels() << endl;
    cout << "Samples: " << wav.getSampleCount() << endl;
    cout << "Resolution: " << wav.getBitsPerSample() << endl;
    snd_pcm_t* snd_pcm;
    if(snd_pcm_open(&snd_pcm,"default",SND_PCM_STREAM_PLAYBACK,0) != 0) assert_error
    if(snd_pcm_set_params(snd_pcm,SND_PCM_FORMAT_S16_LE,SND_PCM_ACCESS_RW_INTERLEAVED,wav.getChannels(),wav.getSampleRate(),1,40000) != 0) assert_error
    snd_pcm_sframes_t frames;
    long count = 0;
    fftw_complex *in,*out;
    fftw_plan p,r;

    in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*fft_n);
    out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*fft_n);
    p = fftw_plan_dft_1d(fft_n,in,out,FFTW_FORWARD,FFTW_ESTIMATE);

    while(count + fft_n < wav.getSampleCount()) {
        int r = plot.HandleEvent();
        if(r == 1) break;

        for(int i = 0;i < fft_n;i++) {
            auto d = (int*)((char *) wav.getRawBuffer() + ((count+i) * wav.getChannels() * (wav.getBitsPerSample() / 8)));
            in[i][0] = (double)(*d) / (double)INT16_MAX;
            in[i][1] = 0;
        }
        fftw_execute(p);
        dat.clear();
        for(int i = 1;i < fft_n/2;i++) {
            dat.push_back(std::make_pair((double)i * ((double)wav.getSampleRate()) / ((double)fft_n),abs(out[i][0])));
        }
	if(r == 2) {
            FILE* f = fopen(argc[2],"w");
            for(auto e:dat) {
                fprintf(f,"%024.12f %024.12f\n",e.first,e.second);
	    }
	    fclose(f);
	    cout << "Transformed data dumped." << endl;
	}
        if(dat2.size() < dat.size()) {
            dat2.clear();
            for(auto& e:dat) dat2.push_back(e);
        }
        for(int i = 0;i < dat2.size();i++) {
            //dat2[i].second = std::max(dat[i].second,dat2[i].second * 0.8);
            dat2[i].second = dat[i].second*0.3+dat2[i].second * 0.7;
        }

        plot.DrawLineGraph(dat2);
        long b2wrt = min(wav.getSampleRate() / 50,wav.getSampleCount()-count);
        frames = snd_pcm_writei(snd_pcm, (char *) wav.getRawBuffer() + (count * wav.getChannels() * (wav.getBitsPerSample() / 8)), b2wrt);
        if (frames < 0) frames = snd_pcm_recover(snd_pcm, frames, 0);
        count += frames;

    }
    snd_pcm_close(snd_pcm);
    
    return 0;

    while(1) {
        int r = plot.HandleEvent();
	if(r == 0) break;
    }
    return 0;
}
