#include "RogueProgram.h"
#include "rogue_interface.h"
#include <SDL/SDL_mixer.h>
#include <iostream>


#ifndef SOUND_CHANNELS
  #define SOUND_CHANNELS 32
#endif

struct Sound;
#define INVALID_CHANNEL -1
#define MUSIC_CHANNEL SOUND_CHANNELS
static Sound * real_channel_to_sound[SOUND_CHANNELS + 1 + 1];
static Sound ** channel_to_sound = real_channel_to_sound + 1;


struct Sound
{
  bool is_music;
  int music_vol;
  int chan;
  union {
    Mix_Music * music;
    Mix_Chunk * chunk;
  };

  Sound (char * filepath, bool is_music)
  : is_music(is_music),
    music_vol(MIX_MAX_VOLUME),
    chan(INVALID_CHANNEL)
  {
    if (is_music)
      music = Mix_LoadMUS(filepath);
    else
      chunk = Mix_LoadWAV(filepath);
  }

  void destroy (void)
  {
    if (is_music)
    {
      if (music)
      {
        Mix_FreeMusic(music);
        channel_to_sound[chan] = 0;
      }
    }
    else
    {
      if (chunk)
      {
        Mix_FreeChunk(chunk);
        channel_to_sound[chan] = 0;
      }
    }
  }

  ~Sound ()
  {
    destroy();
  }

  double get_duration (void)
  {
    if (is_music) return 0.0; // Unsupported

    int frequency;
    Uint16 fmt;
    int channels;

    if (!Mix_QuerySpec(&frequency, &fmt, &channels)) return 0.0;

    int bytes_per_sample = 0;
    switch (fmt)
    {
      case AUDIO_U8:
      case AUDIO_S8:
        bytes_per_sample = 8;
        break;
      case AUDIO_U16LSB:
      case AUDIO_S16LSB:
      case AUDIO_U16MSB:
      case AUDIO_S16MSB:
        bytes_per_sample = 16;
        break;
    }
    bytes_per_sample *= channels;

    return (chunk->alen / bytes_per_sample) / (double)frequency;
  }

  bool is_playing (void)
  {
    if (chan == -1) return false;
    if (channel_to_sound[chan] != this) return false;
    if (is_music) return Mix_PlayingMusic();
    return Mix_Playing(chan) && !Mix_Paused(chan);
  }

  void pause (void)
  {
    if (chan == -1) return;
    if (channel_to_sound[chan] != this) return;
    if (is_music)
      Mix_PauseMusic();
    else
      Mix_Pause(chan);
  }

  void play (bool repeating)
  {
    if (is_music)
    {
      chan = MUSIC_CHANNEL;
      auto old_sound = channel_to_sound[chan];
      if (old_sound) old_sound->chan = INVALID_CHANNEL;
      channel_to_sound[chan] = this;
      Mix_HaltMusic(); // Otherwise, a fadeout might *block*?!
      Mix_VolumeMusic(music_vol);
      Mix_PlayMusic(music, repeating ? -1 : 0);
    }
    else
    {
      chan = Mix_PlayChannel(chan, chunk, repeating ? -1 : 0);
      if (chan != -1)
      {
        auto old_sound = channel_to_sound[chan];
        if (old_sound) old_sound->chan = INVALID_CHANNEL;
        channel_to_sound[chan] = this;
      }
    }
  }

  void set_volume (double volume)
  {
    int vol = MIX_MAX_VOLUME * volume;
    if (is_music)
    {
      music_vol = vol;
      if (chan == MUSIC_CHANNEL)
      {
        // It's the current music.
        Mix_VolumeMusic(music_vol);
      }
    }
    else
    {
      if (chunk) Mix_VolumeChunk(chunk, vol);
    }
  }
};


void* PlasmacoreSound_create( RogueString* filepath, bool is_music )
{
  return new Sound((char*)filepath->utf8, is_music);
}

void PlasmacoreSound_delete( void* sound )
{
  if (!sound) return;
  delete (Sound*)sound;
}

double PlasmacoreSound_duration( void* sound )
{
  if (!sound) return 0.0;
  return ((Sound*)sound)->get_duration();
}

bool PlasmacoreSound_is_playing( void* sound )
{
  if (!sound) return false;
  return ((Sound*)sound)->is_playing();
}

void PlasmacoreSound_pause( void* sound )
{
  if (!sound) return;
  ((Sound*)sound)->pause();
}

double PlasmacoreSound_position( void* sound )
{
  return 0.0; // Can't support (Maybe with a postmix processor?)
}

void PlasmacoreSound_play( void* sound, bool repeating )
{
  if (!sound) return;
  ((Sound*)sound)->play(repeating);
}

void PlasmacoreSound_set_position( void* sound, double to_time )
{
  return; // Can't support
}

void PlasmacoreSound_set_volume( void* sound, double volume )
{
  if (!sound) return;
  ((Sound*)sound)->set_volume(volume);
}

bool PlasmacoreSoundInit( void )
{
  //TODO: Reasonable error handling
  Mix_Init(MIX_INIT_OGG);
  if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024))
  {
    std::cerr << "LOG: Failed to initialize audio - " << Mix_GetError() << std::endl;
  }
  Mix_AllocateChannels(SOUND_CHANNELS);
  return true;
}
