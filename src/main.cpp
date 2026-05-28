#include <hyprland/src/plugins/PluginAPI.hpp>

#include <atomic>
#include <thread>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "music_data.h"

static HANDLE PHANDLE = nullptr;
static ma_decoder g_decoder;
static ma_device g_device;
static std::atomic<bool> g_running{false};
static std::thread g_audioThread;

static void dataCallback(ma_device *pDevice, void *pOutput, const void *pInput,
                         ma_uint32 frameCount) {
  auto *decoder = static_cast<ma_decoder *>(pDevice->pUserData);
  if (!decoder)
    return;

  auto *pOut = static_cast<ma_uint8 *>(pOutput);
  ma_uint32 bpf =
      ma_get_bytes_per_frame(decoder->outputFormat, decoder->outputChannels);

  ma_uint64 read = 0;
  while (read < frameCount) {
    ma_uint64 got = 0;
    ma_decoder_read_pcm_frames(decoder, pOut + read * bpf, frameCount - read,
                               &got);
    read += got;
    if (got < (frameCount - read))
      ma_decoder_seek_to_pcm_frame(decoder, 0);
    if (got == 0)
      break;
  }

  (void)pInput;
}

static void audioThreadFunc() {
  ma_decoder_config decoderConfig =
      ma_decoder_config_init(ma_format_f32, 2, 44100);

  if (ma_decoder_init_memory(music_mp3, music_mp3_len, &decoderConfig,
                             &g_decoder) != MA_SUCCESS)
    return;

  ma_device_config deviceConfig =
      ma_device_config_init(ma_device_type_playback);
  deviceConfig.playback.format = g_decoder.outputFormat;
  deviceConfig.playback.channels = g_decoder.outputChannels;
  deviceConfig.sampleRate = g_decoder.outputSampleRate;
  deviceConfig.dataCallback = dataCallback;
  deviceConfig.pUserData = &g_decoder;

  if (ma_device_init(nullptr, &deviceConfig, &g_device) != MA_SUCCESS) {
    ma_decoder_uninit(&g_decoder);
    return;
  }

  if (ma_device_start(&g_device) != MA_SUCCESS) {
    ma_device_uninit(&g_device);
    ma_decoder_uninit(&g_decoder);
    return;
  }

  while (g_running.load())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ma_device_uninit(&g_device);
  ma_decoder_uninit(&g_decoder);
}

APICALL EXPORT std::string PLUGIN_API_VERSION() { return HYPRLAND_API_VERSION; }

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
  PHANDLE = handle;

  g_running.store(true);
  g_audioThread = std::thread(audioThreadFunc);

  return {"hyprtiki", "Plays TIKI TIKI SLOWED in background", "datfooldive",
          "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
  g_running.store(false);
  if (g_audioThread.joinable())
    g_audioThread.join();
}
