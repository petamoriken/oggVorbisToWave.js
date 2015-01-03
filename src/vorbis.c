#include <emscripten.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#pragma comment(lib , "ogg_static")
#pragma comment(lib , "vorbis_static")
#pragma comment(lib , "vorbisfile_static")

typedef unsigned short WORD;
typedef unsigned long  DWORD;

//wavファイルのヘッダ
typedef struct {
    char RIFF[4];	//"RIFF"
    DWORD bytes;	//(ファイルのバイト数)-8
    char WAVE[4];	// "WAVE" 
    char fmt[4];	// "fmt "
    DWORD siz_wf;	//PCMWAVEFORMAT構造体のバイト数=常に16

    //PCMWAVEFORMAT構造
    WORD wFormatTag; 
    WORD nChannels; 
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec; 
    WORD nBlockAlign;
    WORD wBitsPerSample;
    char data[4];		//"data"
    DWORD pcmbytes;		//波形データのバイト数
} WAVHEADER;


OggVorbis_File *sp_ov_start(void) {
    OggVorbis_File *s;
    s = (OggVorbis_File *)malloc(sizeof(OggVorbis_File));
    return s;
}

void sp_ov_end(OggVorbis_File *s) {
    free(s);
}

char *sp_ov_to_wave(OggVorbis_File *s) {
    vorbis_info *vi;
    WAVHEADER wh;
    DWORD bytes;

    char pcmout[4096];
    int current_section = 0;
    int count = 0;

    char *ret;
    int pc = 0;

    ov_fopen("data.ogg", s);

    //必要な情報を取得
    vi = ov_info(s, -1);
    bytes = vi->channels * 2 * ov_pcm_total(s, -1);

    //wavヘッダを作る
    memcpy(wh.RIFF , "RIFF" , 4);
    wh.bytes = bytes + 44 - 8;
    memcpy(wh.WAVE , "WAVE", 4); 
    memcpy(wh.fmt , "fmt ", 4);
    wh.siz_wf = 16;
    wh.wFormatTag = 1;
    wh.nChannels = vi->channels; 
    wh.nSamplesPerSec = vi->rate;
    wh.wBitsPerSample = 16;
    wh.nBlockAlign = wh.nChannels * wh.wBitsPerSample/8;
    wh.nAvgBytesPerSec = wh.nSamplesPerSec * wh.nBlockAlign; 
    memcpy(wh.data , "data", 4);
    wh.pcmbytes = bytes;

    //ret = (char *)malloc(bytes + 44);
    memcpy(ret, &wh, 44);
    pc += 44;

    do {
        count = ov_read(s, pcmout, 4096, 0, 2, 1, &current_section);
        if (count > 0) {
            memcpy(ret + pc, pcmout, count);
            pc += count;
        }
    } while(count > 0);

    ov_clear(s);

    return ret;
}







