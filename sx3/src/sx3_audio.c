// File: sx3_audio.c
// Author: Paul Brannan
//
// Here we handle all the audio for Sx3.  Audio is played with the
// sx3_play_sound function, and is then mixed in with currently playing
// sounds using SDL.  Note that mixaudio() runs in a separate thread from
// the rest of Sx3.  The functions below come (almost) directly from the
// SDL webpage.

#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#include <string.h>
#include <stdlib.h>

// The maximum number of sounds we allow to play at any given time.
// FIX ME!! This should be a run-time option.
#define NUM_SOUNDS 4

// We set this to 1 if audio has been initialized.
int audio_ok = 0;

// The structure we use to store the audio data and mix it.
struct sample {
     Uint8 *data;
     Uint32 dpos;
     Uint32 dlen;
} sounds[NUM_SOUNDS];

// mixaudio() is the SDL callback function for playing sounds
static void mixaudio(void *unused, Uint8 *stream, int len)
{
    int i, num_sounds = 0;
    Uint32 amount;

    // FIX ME!! This code could be more efficient by mixing in pairs,
    // though this would require recursion.
    for(i = 0; i < NUM_SOUNDS; ++i)
    {
        amount = (sounds[i].dlen-sounds[i].dpos);
        if(amount == 0) continue;
        if(amount > len) amount = len;
        SDL_MixAudio(stream, &sounds[i].data[sounds[i].dpos], amount,
            SDL_MIX_MAXVOLUME);
        sounds[i].dpos += amount;
        num_sounds++;
    }

    if(num_sounds == 0) SDL_PauseAudio(1);
}

void sx3_init_audio()
{
    SDL_AudioSpec fmt;

    // Set 16-bit stereo audio at 22Khz
    fmt.freq = 22050;
    fmt.format = AUDIO_S16;
    fmt.channels = 2;
    fmt.samples = 512;
    fmt.callback = mixaudio;
    fmt.userdata = NULL;

    // Open the audio device and start playing sound!
    if(SDL_OpenAudio(&fmt, NULL) < 0)
    {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
        audio_ok = 0;
    }
    else
    {
        printf("Sx3 audio subsystem initialized\n");
        audio_ok = 1;
    }
}

void sx3_close_audio()
{
    if(!audio_ok) return;
    SDL_CloseAudio();
}

void sx3_play_sound(const char *file)
{
    int index;
    SDL_AudioSpec wave;
    Uint8 *data;
    Uint32 dlen;
    SDL_AudioCVT cvt;

    if(!audio_ok) return;

    // Look for an empty (or finished) sound slot
    for(index=0; index<NUM_SOUNDS; ++index)
       if(sounds[index].dpos == sounds[index].dlen) break;
    if(index == NUM_SOUNDS)
        return;

     // Load the sound file and convert it to 16-bit stereo at 22kHz
    if(SDL_LoadWAV(file, &wave, &data, &dlen) == NULL)
    {
        fprintf(stderr, "Couldn't load %s: %s\n", file, SDL_GetError());
        return;
    }
    SDL_BuildAudioCVT(&cvt, wave.format, wave.channels, wave.freq,
        AUDIO_S16, 2, 22050);
    cvt.buf = malloc(dlen*cvt.len_mult);
    memcpy(cvt.buf, data, dlen);
    cvt.len = dlen;
    SDL_ConvertAudio(&cvt);
    SDL_FreeWAV(data);

    // Put the sound data in the slot (it starts playing immediately)
    if(sounds[index].data)
    {
        free(sounds[index].data);
    }
    SDL_LockAudio();
    sounds[index].data = cvt.buf;
    sounds[index].dlen = cvt.len_cvt;
    sounds[index].dpos = 0;
    SDL_UnlockAudio();
    SDL_PauseAudio(0);
}
