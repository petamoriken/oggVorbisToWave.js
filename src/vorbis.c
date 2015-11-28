//#include <emscripten.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#pragma comment(lib , "ogg_static")
#pragma comment(lib , "vorbis_static")
#pragma comment(lib , "vorbisfile_static")

typedef unsigned short WORD;
typedef unsigned long  DWORD;

typedef struct {
    char RIFF[4];	// "RIFF"
    DWORD bytes;	// file byteLength - 8
    char WAVE[4];	// "WAVE" 
    char fmt[4];	// "fmt "
    DWORD siz_wf;	// sizeof(PCMWAVEFORMAT) == 16

    // PCMWAVEFORMAT
    WORD wFormatTag; 
    WORD nChannels; 
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec; 
    WORD nBlockAlign;
    WORD wBitsPerSample;
    char data[4];		// "data"
    DWORD pcmbytes;
} WAVHEADER;

typedef struct {
    char *buf;
    int siz;
    int pos;
} OVMEM;


// http://torasukenote.blog120.fc2.com/blog-entry-106.html
bool convertEndian(void *input, size_t s) {
    int i;
    char *temp;

    if((temp = (char *) malloc( s * sizeof(char) )) == NULL) {
        return false;
    }

    for(i=0; i<s; i++){
        temp[i] = ((char *)input)[i];
    }

    for(i=1; i<=s; i++){
        ((char *)input)[i-1] = temp[s-i];
    }

    free(temp);

    return true;
}

// http://www.geocities.jp/debu0510/personal/oggvorbis.html#8
size_t mread(void *ptr, size_t size, size_t nmemb, void *mem) {
    OVMEM *pom = (OVMEM *) mem;

    if (pom == NULL) return -1;
    if ((pom->pos >= pom->siz) || (pom->pos == -1)) return 0;
    if (pom->pos + size*nmemb >= pom->siz) {
        int ret;
        memcpy(ptr, &pom->buf[pom->pos], pom->siz - pom->pos);
        ret = (pom->siz - pom->pos)/size;
        pom->pos = pom->siz;
        return ret;
    }
    memcpy(ptr, &pom->buf[pom->pos], nmemb * size);
    pom->pos += (nmemb * size);
    return nmemb;
}

int mseek(void *mem, ogg_int64_t offset, int whence) {
    OVMEM *pom = (OVMEM *) mem;
    int newpos;
    
    if (pom == NULL || pom->pos < 0) return -1;
    if (offset < 0) {
        pom->pos = -1;
        return -1;
    }
    switch (whence) {
        case SEEK_SET:
            newpos = offset;
            break;
        case SEEK_CUR:
            newpos = pom->pos + offset;
            break;
        case SEEK_END:
            newpos = pom->siz + offset;
            break;
        default:
            return -1;
    }
    if (newpos < 0) return -1;
    pom->pos = newpos;
    
    return 0;
}

long mtell(void *mem) {
    OVMEM *pom = (OVMEM *) mem;
    if (pom == NULL) return -1;
    return pom->pos;
}

int mclose(void *mem) {
    OVMEM *pom = (OVMEM *) mem;
    if (pom == NULL) return -1;
    free(pom);
    return 0;
}


unsigned char *sp_ov_to_wave(char *buffer, size_t buffer_size, bool isLittleEndian) {
    OggVorbis_File s;

    ov_callbacks oc;
    OVMEM *pom;

    vorbis_info *vi;
    WAVHEADER wh;
    DWORD bytes;

    char pcmout[4096];
    int current_section = 0;
    int count = 0;

    unsigned char *ret;
    int pc = 0;

    // set callback function
    oc.read_func = mread;
    oc.seek_func = mseek;
    oc.close_func = mclose;
    oc.tell_func = mtell;

    // crete Ogg Vorbis Memory
    pom = (OVMEM *) malloc(sizeof(OVMEM));

    if(pom == NULL) {
        goto fail;
    } 

    pom->pos = 0;
    pom->siz = buffer_size;
    pom->buf = buffer;

    ov_open_callbacks(pom, &s, NULL, 0, oc);
    vi = ov_info(&s, -1);
    bytes = vi->channels * 2 * ov_pcm_total(&s, -1);

    // create wave header
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

    // big endianess -> little endianess
    if( !isLittleEndian ) {
        if(!convertEndian(&(wh.bytes), sizeof(wh.bytes)))                        goto fail;
        if(!convertEndian(&(wh.siz_wf), sizeof(wh.siz_wf)))                      goto fail;
        if(!convertEndian(&(wh.wFormatTag), sizeof(wh.wFormatTag)))              goto fail;
        if(!convertEndian(&(wh.nChannels), sizeof(wh.nChannels)))                goto fail;
        if(!convertEndian(&(wh.nSamplesPerSec), sizeof(wh.nSamplesPerSec)))      goto fail;
        if(!convertEndian(&(wh.wBitsPerSample), sizeof(wh.wBitsPerSample)))      goto fail;
        if(!convertEndian(&(wh.nBlockAlign), sizeof(wh.nBlockAlign)))            goto fail;
        if(!convertEndian(&(wh.nAvgBytesPerSec), sizeof(wh.nAvgBytesPerSec)))    goto fail;
        if(!convertEndian(&(wh.pcmbytes), sizeof(wh.pcmbytes)))                  goto fail;
    }

    ret = (unsigned char *) malloc(bytes + sizeof(WAVHEADER));

    if(ret == NULL) {
        goto fail;
    }

    memcpy(ret, &wh, sizeof(WAVHEADER));
    pc += sizeof(WAVHEADER);

    do {
        count = ov_read(&s, pcmout, 4096, 0, 2, 1, &current_section);
        if (count > 0) {
            memcpy(ret + pc, pcmout, count);
            pc += count;
        }
    } while(count > 0);

    ov_clear(&s);

    return ret;

fail:
    return NULL;

}

void sp_free_wave(unsigned char *s) {
    free(s);
}