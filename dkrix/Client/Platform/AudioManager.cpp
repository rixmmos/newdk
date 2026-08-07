//----------------------------------------------------------------------
// AudioManager.cpp
//
// SDL2 Implementation (Cross-platform)
// Windows DirectSound implementation removed - using SDL2_mixer on all platforms
// NOTE: This class is a stub. Use SDL_mixer functions directly instead.
// (CDirectSound_Adapter.cpp, a duplicate implementation that called into
// DXLibBackend's dxlib_sound_* functions, was deleted in Phase 3 item 1: it
// never compiled — its ctor/dtor were named CDirectSound() instead of
// AudioManager(), and its global instance was declared with the undeclared
// type `CDirectSound`. It also duplicated every symbol below whenever
// HAVE_SDL2_MIXER was on, which would have been a link error even after
// fixing the names. This file was always the one actually built.)
//----------------------------------------------------------------------

#include "AudioManager.h"

//-----------------------------------------------------------------------------
// Global instance
//-----------------------------------------------------------------------------
AudioManager g_SDLAudio;

//-----------------------------------------------------------------------------
// Constructor/Destructor
//-----------------------------------------------------------------------------

AudioManager::AudioManager()
{
	m_pDS = NULL;
	m_bInit = false;
	m_bMute = false;
	m_MaxVolume = 0;
}

AudioManager::~AudioManager()
{
	// Stub - SDL mixer is managed elsewhere
}

//-----------------------------------------------------------------------------
// Initialization
//-----------------------------------------------------------------------------

bool AudioManager::Init(HWND hwnd)
{
	// Not implemented - use Mix_OpenAudio() from SDL_mixer instead
	(void)hwnd;
	return false;
}

void AudioManager::Release()
{
	// Not implemented - use Mix_CloseAudio() from SDL_mixer instead
}

bool AudioManager::IsInit() const
{
	return m_bInit;
}

//-----------------------------------------------------------------------------
// Mute Control
//-----------------------------------------------------------------------------

bool AudioManager::IsMute() const
{
	return m_bMute;
}

void AudioManager::SetMute()
{
	m_bMute = true;
}

void AudioManager::UnSetMute()
{
	m_bMute = false;
}

//-----------------------------------------------------------------------------
// Volume Control
//-----------------------------------------------------------------------------

LONG AudioManager::GetVolumeLimit() const
{
	return m_MaxVolume;
}

void AudioManager::SetVolumeLimit(LONG volume)
{
	m_MaxVolume = volume;
}

bool AudioManager::SetMaxVolume(LPDIRECTSOUNDBUFFER buffer)
{
	// Not implemented - use Mix_Volume() from SDL_mixer instead
	(void)buffer;
	return false;
}

bool AudioManager::AddVolume(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// Not implemented - use Mix_Volume() from SDL_mixer instead
	(void)buffer;
	(void)step;
	return false;
}

bool AudioManager::SubVolume(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// Not implemented - use Mix_Volume() from SDL_mixer instead
	(void)buffer;
	(void)step;
	return false;
}

bool AudioManager::SubVolumeFromMax(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// Not implemented - use Mix_Volume() from SDL_mixer instead
	(void)buffer;
	(void)step;
	return false;
}

//-----------------------------------------------------------------------------
// Frequency Control
//-----------------------------------------------------------------------------

bool AudioManager::AddFrequency(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// Not implemented - SDL_mixer doesn't support real-time frequency changes
	(void)buffer;
	(void)step;
	return false;
}

bool AudioManager::SubFrequency(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// Not implemented - SDL_mixer doesn't support real-time frequency changes
	(void)buffer;
	(void)step;
	return false;
}

//-----------------------------------------------------------------------------
// Pan Control
//-----------------------------------------------------------------------------

bool AudioManager::RightPan(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// Not implemented - SDL_mixer doesn't support real-time panning
	(void)buffer;
	(void)step;
	return false;
}

bool AudioManager::LeftPan(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// Not implemented - SDL_mixer doesn't support real-time panning
	(void)buffer;
	(void)step;
	return false;
}

bool AudioManager::CenterToRightPan(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// Not implemented - SDL_mixer doesn't support real-time panning
	(void)buffer;
	(void)step;
	return false;
}

bool AudioManager::CenterToLeftPan(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// Not implemented - SDL_mixer doesn't support real-time panning
	(void)buffer;
	(void)step;
	return false;
}

bool AudioManager::CenterPan(LPDIRECTSOUNDBUFFER buffer)
{
	// Not implemented - SDL_mixer doesn't support real-time panning
	(void)buffer;
	return false;
}

bool AudioManager::ChangePan(LPDIRECTSOUNDBUFFER buffer, int pan)
{
	// Not implemented - SDL_mixer doesn't support real-time panning
	(void)buffer;
	(void)pan;
	return false;
}

//-----------------------------------------------------------------------------
// Sound Buffer Operations
//-----------------------------------------------------------------------------

LPDIRECTSOUNDBUFFER AudioManager::LoadWav(LPSTR filename)
{
	// Not implemented - use Mix_LoadWAV() from SDL_mixer instead
	(void)filename;
	return NULL;
}

LPDIRECTSOUNDBUFFER AudioManager::CreateBuffer(LPVOID sdat, DWORD size, DWORD caps, LPWAVEFORMATEX wfx)
{
	// Not implemented - use SDL_mixer sound functions instead
	(void)sdat;
	(void)size;
	(void)caps;
	(void)wfx;
	return NULL;
}

void AudioManager::Release(LPDIRECTSOUNDBUFFER buffer)
{
	// Not implemented - use Mix_FreeChunk() from SDL_mixer instead
	(void)buffer;
}

LPDIRECTSOUNDBUFFER AudioManager::DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER buffer, bool bAutoRelease)
{
	// Not implemented - SDL_mixer doesn't need buffer duplication
	(void)buffer;
	(void)bAutoRelease;
	return NULL;
}

void AudioManager::ReleaseDuplicateBuffer()
{
	// Not implemented - not needed with SDL_mixer
}

void AudioManager::ReleaseTerminatedDuplicateBuffer()
{
	// Not implemented - not needed with SDL_mixer
}

//-----------------------------------------------------------------------------
// Playback Control
//-----------------------------------------------------------------------------

bool AudioManager::IsPlay(LPDIRECTSOUNDBUFFER buffer) const
{
	// Not implemented - use Mix_Playing() from SDL_mixer instead
	(void)buffer;
	return false;
}

bool AudioManager::NewPlay(LPDIRECTSOUNDBUFFER buffer, bool loop)
{
	// Not implemented - use Mix_PlayChannel() from SDL_mixer instead
	(void)buffer;
	(void)loop;
	return false;
}

bool AudioManager::Play(LPDIRECTSOUNDBUFFER buffer, bool loop, bool duplicate)
{
	// Not implemented - use Mix_PlayChannel() from SDL_mixer instead
	(void)buffer;
	(void)loop;
	(void)duplicate;
	return false;
}

bool AudioManager::Stop(LPDIRECTSOUNDBUFFER buffer)
{
	// Not implemented - use Mix_HaltChannel() from SDL_mixer instead
	(void)buffer;
	return false;
}

//-----------------------------------------------------------------------------
// DirectSound Access
//-----------------------------------------------------------------------------

LPDIRECTSOUND AudioManager::GetDS() const
{
	return m_pDS;
}

//-----------------------------------------------------------------------------
// Error Handling
//-----------------------------------------------------------------------------

bool AudioManager::DirectSoundFailed(const char* str)
{
	// Not implemented - log error to console instead
	(void)str;
	return false;
}
