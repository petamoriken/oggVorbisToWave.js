//#include <emscripten.h>

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

// プロトタイプ宣言
int convertEndian(void *input, size_t s);

// wav ファイルのヘッダ
typedef struct {
    char RIFF[4];	// "RIFF"
    DWORD bytes;	// (ファイルのバイト数) - 8
    char WAVE[4];	// "WAVE" 
    char fmt[4];	// "fmt "
    DWORD siz_wf;	// PCMWAVEFORMAT 構造体のバイト数 = 常に16

    // PCMWAVEFORMAT 構造
    WORD wFormatTag; 
    WORD nChannels; 
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec; 
    WORD nBlockAlign;
    WORD wBitsPerSample;
    char data[4];		// "data"
    DWORD pcmbytes;		// 波形データのバイト数
} WAVHEADER;


OggVorbis_File *sp_ov_start(void) {
    OggVorbis_File *s;
    s = (OggVorbis_File *) malloc(sizeof(OggVorbis_File));
    return s;
}

void sp_ov_end(OggVorbis_File *s) {
    free(s);
}

unsigned char *sp_ov_to_wave(OggVorbis_File *s, _Bool isLittleEndian) {
    vorbis_info *vi;
    WAVHEADER wh;
    DWORD bytes;

    char pcmout[4096];
    int current_section = 0;
    int count = 0;

    unsigned char *ret;
    int pc = 0;

    ov_fopen("data.ogg", s);

    // 必要な情報を取得
    vi = ov_info(s, -1);
    bytes = vi->channels * 2 * ov_pcm_total(s, -1);

    // wav ヘッダを作る
    memcpy(wh.RIFF , "RIFF" , 4);
    wh.bytes = bytes + sizeof(WAVHEADER) - 8;
    memcpy(wh.WAVE , "WAVE", 4); 
    memcpy(wh.fmt , "fmt ", 4);
    wh.siz_wf = 16;
    wh.wFormatTag = 1;
    wh.nChannels = vi->channels; 
    wh.nSamplesPerSec = vi->rate;
    wh.wBitsPerSample = 16;
    wh.nBlockAlign = wh.nChannels * wh.wBitsPerSample / 8;
    wh.nAvgBytesPerSec = wh.nSamplesPerSec * wh.nBlockAlign; 
    memcpy(wh.data , "data", 4);
    wh.pcmbytes = bytes;

    // ビッグエンディアンならリトルエンディアンに変換する
    if( !isLittleEndian ) {
        convertEndian(&(wh.bytes), sizeof(wh.bytes));
        convertEndian(&(wh.siz_wf), sizeof(wh.siz_wf));
        convertEndian(&(wh.wFormatTag), sizeof(wh.wFormatTag));
        convertEndian(&(wh.nChannels), sizeof(wh.nChannels));
        convertEndian(&(wh.nSamplesPerSec), sizeof(wh.nSamplesPerSec));
        convertEndian(&(wh.wBitsPerSample), sizeof(wh.wBitsPerSample));
        convertEndian(&(wh.nBlockAlign), sizeof(wh.nBlockAlign));
        convertEndian(&(wh.nAvgBytesPerSec), sizeof(wh.nAvgBytesPerSec));
        convertEndian(&(wh.pcmbytes), sizeof(wh.pcmbytes));
    }

    ret = (unsigned char *) malloc(bytes + sizeof(WAVHEADER));

    if(ret == NULL) {
        return NULL;
    }

    memcpy(ret, &wh, sizeof(WAVHEADER));
    pc += sizeof(WAVHEADER);

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

// http://torasukenote.blog120.fc2.com/blog-entry-106.html
int convertEndian(void *input, size_t s){
    // エンディアン変換をおこなう関数
    // stdlib.hをインクルードしてください。
    // 【引数】: void *input...エンディアン変換対象へのポインタ
    // 【引数】: size_t    s...変換対象のバイト数

    int i;   // カウンタ
    char *temp;   // 変換時に用いる一時的配列

    if((temp = (char *) malloc( s * sizeof(char) )) == NULL){
        return 0;   // 領域確保できず（失敗）
    }

    for(i=0; i<s; i++){   // inputデータをtempに一時保管
        temp[i] = ((char *)input)[i];
    }

    for(i=1; i<=s; i++){   // tempデータを逆方向にしてinputへ代入
        ((char *)input)[i-1] = temp[s-i];
    }

    free(temp);   // 確保した領域を解放

    return 1;   // 正常終了
}