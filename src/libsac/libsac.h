#ifndef CODEC_H
#define CODEC_H

#include "../file/wav.h"
#include "../file/sac.h"
#include "cost.h"
#include "profile.h"

class FrameCoder {
  public:
    const static int size_profile_bytes_normal=35*4;
    const static int size_profile_bytes_high=44*4;
    struct coder_ctx {
      enum SearchCost {L1,Entropy,Golomb,Bitplane};
      enum SearchMethod {DDS,GRS};
      int profile=0;
      int optimize=0;
      int sparse_pcm=0;
      int optimize_maxiter=0;
      int optimize_maxnfunc=0;
      int optimize_mode=0;
      SearchMethod optimize_search=DDS;
      SearchCost optimize_cost=L1;
      int optimize_div=0;
      int optimize_ncycle=0;
      SacProfile profiledata;
    };
    FrameCoder(int numchannels,int framesize,const coder_ctx &opt);
    void SetNumSamples(int nsamples){numsamples_=nsamples;};
    int GetNumSamples(){return numsamples_;};
    void Predict();
    void Unpredict();
    void Encode();
    void Decode();
    void WriteEncoded(AudioFile &fout);
    void ReadEncoded(AudioFile &fin);
    std::vector <std::vector<int32_t>>samples,err0,err1,error,s2u_error,s2u_error_map,pred;
    std::vector <BufIO> encoded,enc_temp1,enc_temp2;
    std::vector <SacProfile::FrameStats> framestats;
  private:
    void EncodeProfile(const SacProfile &profile,std::vector <uint8_t>&buf);
    void DecodeProfile(SacProfile &profile,const std::vector <uint8_t>&buf);
    void AnalyseMonoChannel(int ch, int numsamples);
    //void InterChannel(int ch0,int ch1,int numsamples);
    int EncodeMonoFrame_Normal(int ch,int numsamples,BufIO &buf);
    int EncodeMonoFrame_Mapped(int ch,int numsamples,BufIO &buf);
    void Optimize(SacProfile &profile,const std::vector<int>&params_to_optimize);
    double GetCost(SacProfile &profile,CostFunction *func,int coef,double testval,int start_sample,int samples_to_optimize);

    void AnalyseChannel(int ch,int numsamples);
    void PredictStereoFrame(const SacProfile &profile,int ch0,int ch1,int from,int numsamples,bool optimize=false);
    void UnpredictStereoFrame(const SacProfile &profile,int ch0,int ch1,int numsamples);
    void RemapError(int ch, int numsamples);
    void EncodeMonoFrame(int ch,int numsamples);
    void DecodeMonoFrame(int ch,int numsamples);
    int numchannels_,framesize_,numsamples_;
    int profile_size_bytes_;
    SacProfile baseprofile;
    coder_ctx opt;
};

class Codec {
  public:
    Codec():framesize(0){};
    void EncodeFile(Wav &myWav,Sac &mySac,FrameCoder::coder_ctx &opt);
    //void EncodeFile(Wav &myWav,Sac &mySac,int profile,int optimize,int sparse_pcm);
    void DecodeFile(Sac &mySac,Wav &myWav);
    void ScanFrames(Sac &mySac);
  private:
    void SetOptimizeParam(FrameCoder::coder_ctx &opt);
    void PrintProgress(int samplesprocessed,int totalsamples);
    int framesize;
};

#endif
