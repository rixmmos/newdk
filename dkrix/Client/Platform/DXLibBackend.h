/*-----------------------------------------------------------------------------

	DXLibBackend.h

	DXLib platform backend abstraction.
	Defines the interface for both Windows DirectInput/DirectSound and SDL backends.

	2025.01.14

-----------------------------------------------------------------------------*/

#ifndef __PLATFORM_BACKEND_H__
#define __PLATFORM_BACKEND_H__

#include "../basic/Platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Backend Selection
 * ============================================================================ */

/* Backend type selection */
#ifdef PLATFORM_WINDOWS
	#ifndef PLATFORM_USE_SDL_BACKEND
		#define PLATFORM_BACKEND_WINDOWS  /* Use native Windows APIs */
	#else
		#define PLATFORM_BACKEND_SDL      /* Use SDL2 */
	#endif
#else
	/* Non-Windows platforms must use SDL backend */
	#define PLATFORM_BACKEND_SDL
#endif

/* ============================================================================
 * Input Backend Interface
 * ============================================================================ */

/**
 * Initialize input backend
 * @param window_handle Native window handle (HWND on Windows, SDL_Window* on SDL)
 * @return 0 on success, non-zero on failure
 */
int platform_input_init(void* window_handle);

/**
 * Release input backend
 */
void platform_input_release(void);

/**
 * Update input state (poll for new events)
 * Call this once per frame
 */
void platform_input_update(void);

/**
 * Check if a key is down
 * @param dik_key DirectInput key code (DIK_*)
 * @return 1 if key is down, 0 otherwise
 */
int platform_input_key_down(int dik_key);

/**
 * Get mouse position
 * @param x Output X coordinate
 * @param y Output Y coordinate
 */
void platform_input_get_mouse_pos(int* x, int* y);

/**
 * Get mouse wheel position
 * @return Wheel position (z-coordinate)
 */
int platform_input_get_mouse_wheel(void);

/**
 * Check mouse button states
 * @param left Output: left button state (1=down, 0=up)
 * @param right Output: right button state
 * @param center Output: center button state
 */
void platform_input_get_mouse_buttons(int* left, int* right, int* center);

/**
 * Set mouse position (for relative movement)
 * @param x X coordinate
 * @param y Y coordinate
 */
void platform_input_set_mouse_pos(int x, int y);

/**
 * Text input event callback
 * @param text UTF-8 encoded text input (one or more characters)
 * @param window_coords Window coordinates array [x, y] for cursor position
 */
typedef void (*platform_textinput_callback)(const char* text, int* window_coords);

/**
 * Set text input event receiver callback
 * @param callback Function to call when text is entered
 */
void platform_input_set_textinput_callback(platform_textinput_callback callback);

/**
 * Text editing event callback (for IME composition)
 * @param text UTF-8 encoded text being composed
 * @param start Start position of composition text
 * @param length Length of composition text
 * @param window_coords Window coordinates array [x, y] for cursor position
 */
typedef void (*platform_textediting_callback)(const char* text, int start, int length, int* window_coords);

/**
 * Set text editing event receiver callback
 * @param callback Function to call when text is being composed (IME)
 */
void platform_input_set_textediting_callback(platform_textediting_callback callback);

/**
 * Start text input (enables SDL_TEXTINPUT events)
 */
void platform_input_start_text(void);

/**
 * Stop text input (disables SDL_TEXTINPUT events)
 */
void platform_input_stop_text(void);

/* ============================================================================
 * Sound Backend Interface
 * ============================================================================ */

/**
 * Sound buffer handle (opaque)
 */
typedef struct platform_sound_buffer* platform_sound_t;

/**
 * Initialize sound backend
 * @param window_handle Native window handle
 * @return 0 on success, non-zero on failure
 */
int platform_sound_init(void* window_handle);

/**
 * Release sound backend
 */
void platform_sound_release(void);

/**
 * Load WAV file into memory
 * @param filename Path to WAV file
 * @return Sound handle or NULL on failure
 */
platform_sound_t platform_sound_load_wav(const char* filename);

/**
 * Create sound buffer from raw data
 * @param data Raw audio data
 * @param size Data size in bytes
 * @param channels Number of channels (1=mono, 2=stereo)
 * @param sample_rate Sample rate in Hz
 * @param bits_per_sample Bits per sample (8 or 16)
 * @return Sound handle or NULL on failure
 */
platform_sound_t platform_sound_create_buffer(const void* data, int size,
                                       int channels, int sample_rate,
                                       int bits_per_sample);

/**
 * Release sound buffer
 * @param sound Sound handle
 */
void platform_sound_free(platform_sound_t sound);

/**
 * Play sound
 * @param sound Sound handle
 * @param loop Loop continuously (1) or play once (0)
 * @return 0 on success, non-zero on failure
 */
int platform_sound_play(platform_sound_t sound, int loop);

/**
 * Stop sound
 * @param sound Sound handle
 * @return 0 on success, non-zero on failure
 */
int platform_sound_stop(platform_sound_t sound);

/**
 * Check if sound is playing
 * @param sound Sound handle
 * @return 1 if playing, 0 otherwise
 */
int platform_sound_is_playing(platform_sound_t sound);

/**
 * Set sound volume
 * @param sound Sound handle
 * @param volume Volume level (0-100, where 100 is max)
 * @return 0 on success, non-zero on failure
 */
int platform_sound_set_volume(platform_sound_t sound, int volume);

/**
 * Set sound pan (stereo positioning)
 * @param sound Sound handle
 * @param pan Pan value (-100 to 100, where -100=left, 0=center, 100=right)
 * @return 0 on success, non-zero on failure
 */
int platform_sound_set_pan(platform_sound_t sound, int pan);

/**
 * Set sound frequency (playback speed)
 * @param sound Sound handle
 * @param frequency Frequency in Hz (0 = original frequency)
 * @return 0 on success, non-zero on failure
 */
int platform_sound_set_frequency(platform_sound_t sound, int frequency);

/**
 * Duplicate sound buffer (for simultaneous playback)
 * @param sound Source sound handle
 * @return Duplicate sound handle or NULL on failure
 */
platform_sound_t platform_sound_duplicate(platform_sound_t sound);

/* ============================================================================
 * Music Backend Interface
 * ============================================================================ */

/**
 * Initialize music backend
 * @param window_handle Native window handle
 * @return 0 on success, non-zero on failure
 */
int platform_music_init(void* window_handle);

/**
 * Release music backend
 */
void platform_music_release(void);

/**
 * Load music file
 * @param filename Path to music file (MIDI, MP3, OGG, etc.)
 * @return 0 on success, non-zero on failure
 */
int platform_music_load(const char* filename);

/**
 * Free current music
 */
void platform_music_free(void);

/**
 * Play music
 * @param loop Loop continuously (1) or play once (0)
 * @return 0 on success, non-zero on failure
 */
int platform_music_play(int loop);

/**
 * Stop music
 */
void platform_music_stop(void);

/**
 * Pause music
 */
void platform_music_pause(void);

/**
 * Resume music
 */
void platform_music_resume(void);

/**
 * Check if music is playing
 * @return 1 if playing, 0 otherwise
 */
int platform_music_is_playing(void);

/**
 * Check if music is paused
 * @return 1 if paused, 0 otherwise
 */
int platform_music_is_paused(void);

/**
 * Set music volume
 * @param volume Volume level (0-100)
 * @return 0 on success, non-zero on failure
 */
int platform_music_set_volume(int volume);

/**
 * Set music tempo (playback speed)
 * @param tempo Tempo multiplier (1.0 = normal, 0.5 = half speed, 2.0 = double speed)
 * @return 0 on success, non-zero on failure
 */
int platform_music_set_tempo(float tempo);

/* ============================================================================
 * Stream Backend Interface (for long audio files)
 * ============================================================================ */

/**
 * Stream handle (opaque)
 */
typedef struct platform_stream* platform_stream_t;

/**
 * Initialize stream backend
 * @param window_handle Native window handle
 * @return 0 on success, non-zero on failure
 */
int platform_stream_init(void* window_handle);

/**
 * Release stream backend
 */
void platform_stream_release(void);

/**
 * Load audio file for streaming
 * @param filename Path to audio file
 * @return Stream handle or NULL on failure
 */
platform_stream_t platform_stream_load(const char* filename);

/**
 * Free stream
 * @param stream Stream handle
 */
void platform_stream_free(platform_stream_t stream);

/**
 * Play stream
 * @param stream Stream handle
 * @param loop Loop continuously
 * @return 0 on success, non-zero on failure
 */
int platform_stream_play(platform_stream_t stream, int loop);

/**
 * Stop stream
 * @param stream Stream handle
 */
void platform_stream_stop(platform_stream_t stream);

/**
 * Update stream (call regularly to refill buffers)
 * @param stream Stream handle
 * @return 0 on success, non-zero on failure
 */
int platform_stream_update(platform_stream_t stream);

/**
 * Set stream volume
 * @param stream Stream handle
 * @param volume Volume level (0-100)
 * @return 0 on success, non-zero on failure
 */
int platform_stream_set_volume(platform_stream_t stream, int volume);

/**
 * Check if stream is playing
 * @param stream Stream handle
 * @return 1 if playing, 0 otherwise
 */
int platform_stream_is_playing(platform_stream_t stream);

/* ============================================================================
 * Backend Information
 * ============================================================================ */

/**
 * Get backend name
 * @return Backend name string ("DirectInput/DirectSound" or "SDL2")
 */
const char* platform_get_backend_name(void);

/**
 * Get backend capabilities flags
 */
#define PLATFORM_CAP_INPUT           0x01
#define PLATFORM_CAP_SOUND           0x02
#define PLATFORM_CAP_MUSIC           0x04
#define PLATFORM_CAP_STREAM          0x08
#define PLATFORM_CAP_MIDI            0x10
#define PLATFORM_CAP_MP3             0x20
#define PLATFORM_CAP_OGG             0x40

int platform_get_capabilities(void);

#ifdef __cplusplus
}
#endif

#endif /* __PLATFORM_BACKEND_H__ */
