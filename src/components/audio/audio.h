#pragma once
#include "../component.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace Engine {

enum class AudioType { Self, Global };

class AudioSystem {
public:
  static bool init();
  static void shutdown();
  static void updateListener(const glm::vec3 &pos, const glm::vec3 &forward,
                             const glm::vec3 &up);
  static bool isInitialized() { return s_initialized; }

private:
  static ALCdevice *s_device;
  static ALCcontext *s_context;
  static bool s_initialized;
};

class Audio : public Component {
public:
  Audio() = default;
  ~Audio();

  Audio(const Audio &) = delete;
  Audio &operator=(const Audio &) = delete;
  Audio(Audio &&other) noexcept;
  Audio &operator=(Audio &&other) noexcept;

  bool load(const std::string &filepath);
  void play();
  void pause();
  void stop();
  void setLoop(bool loop);
  void setGain(float gain);
  void setPitch(float pitch);
  void setType(AudioType type);
  void updatePosition(const glm::vec3 &pos);
  void update();

  void setReferenceDistance(float dist);
  void setMaxDistance(float dist);
  void setRolloffFactor(float factor);

  bool isPlaying() const;
  bool isLoaded() const { return m_source != 0; }

  void renderUI() override;
  const char *name() const override { return "Audio"; }

  void destroy();

  void bindTransform(const glm::vec3 *pos) { m_position = pos; }

  static std::vector<Audio *> &activeSources() { return s_active; }

private:
  static std::vector<Audio *> s_active;

  static const int STREAM_BUFFERS = 4;
  static const int CHUNK_SAMPLES = 8192;

  enum class DecoderType { None, WAV, MP3, OGG };

  bool fillBuffer(ALuint buffer);
  void decoderSeekStart();
  void destroyDecoder();
  ALenum getPlayFormat() const;

  ALuint m_source = 0;
  ALuint m_buffers[STREAM_BUFFERS] = {};
  AudioType m_type = AudioType::Self;
  bool m_loop = false;
  bool m_playing = false;
  float m_gain = 1.0f;
  float m_pitch = 1.0f;

  const glm::vec3 *m_position = nullptr;

  DecoderType m_decoderType = DecoderType::None;

  struct {
    void *handle = nullptr;
    unsigned int channels = 0;
    unsigned int sampleRate = 0;
  } m_decoder;

  ALsizei m_sampleRate = 0;
};

} // namespace Engine
