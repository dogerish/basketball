#include "sound.h"
#include <SDL2/SDL.h>

#define NUMTRACKS 10
SOUND_track tracks[NUMTRACKS] = { 0 };

SDL_AudioDeviceID device = 0;
int has_spec_set = 0;
SDL_AudioSpec devspec = { 0 };

// returns number of bytes mixed
int mixtrack(SOUND_track* track, Uint8* stream, int len)
{
    int mixlen = track->buf + track->len - track->pos;
    if (mixlen == 0) return 0;
    // get minimum length between requested and remaining track length
    mixlen = (len > mixlen ? mixlen : len);
    // mix the track in
    SDL_MixAudioFormat(stream, track->pos, devspec.format, mixlen, track->vol);
    return mixlen;
}

void player_callback(void* userdata, Uint8* stream, int len)
{
    // initialize audio to 0
    SDL_memset(stream, 0, len);

    // mix sound tracks
    for (SOUND_track* track = tracks; track < tracks + NUMTRACKS; track++)
    {
        if (track->buf == NULL || track->len == 0) continue;
        switch (track->rep)
        {
        case SOUND_TRACK_REPEAT_NEVER:
            track->pos += mixtrack(track, stream, len);
            if (track->pos == track->buf + track->len)
                track->pos = track->buf = NULL;
            break;
        case SOUND_TRACK_REPEAT_ALWAYS:
            {
                int mixlen = 0, v = 0;
                // fill up all the requested data because we can
                while (mixlen < len)
                {
                    v = mixtrack(track, stream + mixlen, len - mixlen);
                    mixlen += v;
                    // if it ran out of track, set it back to the start
                    if (mixlen < len) track->pos = track->buf;
                    else track->pos += v;
                }
            }
            break;
        }
    }
}

int SOUND_Play(SOUND_track track)
{
    // find the first open track and fill it with this track
    for (SOUND_track* t = tracks; t < tracks + NUMTRACKS; t++)
    {
        if (t->buf != NULL) continue;
        track.pos = track.buf;
        *t = track;
        return 0;
    }
    return -1;
}

int SOUND_OpenDevice()
{
    if (device || !has_spec_set) return -1;
    devspec.callback = player_callback;
    devspec.userdata = NULL;
    device = SDL_OpenAudioDevice(NULL, 0, &devspec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (!device)
        return -1;
    SDL_PauseAudioDevice(device, 0);
    return 0;
}

void SOUND_CloseDevice()
{
    SDL_CloseAudioDevice(device);
    device = 0;
    has_spec_set = 0;
}

int SOUND_LoadWAV(const char* fname, SOUND_track* track)
{
    // load the sound track
    SDL_AudioSpec tmpspec;
    if (SDL_LoadWAV(fname, &tmpspec, &track->buf, &track->len) == NULL)
        return -1;
    // check if it's the trendsetter
    if (!has_spec_set)
    {
        devspec = tmpspec;
        has_spec_set = 1;
        return 0;
    }

    // convert the sound track to meet the trend
    SDL_AudioCVT cvt;
    int v = SDL_BuildAudioCVT(&cvt,
        tmpspec.format, tmpspec.channels, tmpspec.freq,
        devspec.format, devspec.channels, devspec.freq
    );
    if (v < 0) return -1; // failed
    if (v == 0) return 0; // no conversion needed

    cvt.len = track->len;
    cvt.buf = track->buf;
    // the new data is longer than original; copy to a big enough buffer
    if (cvt.len_mult > 1)
    {
        cvt.buf = (Uint8*) SDL_malloc(cvt.len * cvt.len_mult);
        SDL_memcpy(cvt.buf, track->buf, track->len);
        SDL_free(track->buf);
        track->buf = cvt.buf;
    }
    if (SDL_ConvertAudio(&cvt) < 0) return -1;
    track->len = cvt.len_cvt;
    return 0;
}
