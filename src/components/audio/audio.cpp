#include "audio.h"
#include "dr_libs/dr_mp3.h"
#include "dr_libs/dr_wav.h"
#define STB_VORBIS_HEADER_ONLY
#include "stb/stb_vorbis.c"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <vector>

namespace Engine {

ALCdevice *AudioSystem::s_device = nullptr;
ALCcontext *AudioSystem::s_context = nullptr;
bool AudioSystem::s_initialized = false;

bool AudioSystem::init() {
  s_device = alcOpenDevice(nullptr);
  if (!s_device)
    return false;
  s_context = alcCreateContext(s_device, nullptr);
  if (!s_context) {
    alcCloseDevice(s_device);
    s_device = nullptr;
    return false;
  }
  alcMakeContextCurrent(s_context);
  alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
  s_initialized = true;
  return true;
}

void AudioSystem::shutdown() {
  if (!s_initialized)
    return;
  alcMakeContextCurrent(nullptr);
  if (s_context) {
    alcDestroyContext(s_context);
    s_context = nullptr;
  }
  if (s_device) {
    alcCloseDevice(s_device);
    s_device = nullptr;
  }
  s_initialized = false;
}

void AudioSystem::updateListener(const glm::vec3 &pos, const glm::vec3 &forward,
                                 const glm::vec3 &up) {
  if (!s_initialized)
    return;
  alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
  ALfloat orientation[6] = {forward.x, forward.y, forward.z, up.x, up.y, up.z};
  alListenerfv(AL_ORIENTATION, orientation);
}

Audio::~Audio() { destroy(); }

Audio::Audio(Audio &&o) noexcept
    : m_source(o.m_source), m_type(o.m_type), m_loop(o.m_loop),
      m_playing(o.m_playing), m_decoderType(o.m_decoderType),
      m_decoder(o.m_decoder), m_sampleRate(o.m_sampleRate) {
  for (int i = 0; i < STREAM_BUFFERS; i++)
    m_buffers[i] = o.m_buffers[i];
  o.m_source = 0;
  o.m_decoderType = DecoderType::None;
  o.m_decoder.handle = nullptr;
  for (int i = 0; i < STREAM_BUFFERS; i++)
    o.m_buffers[i] = 0;
}

Audio &Audio::operator=(Audio &&o) noexcept {
  if (this != &o) {
    destroy();
    m_source = o.m_source;
    m_type = o.m_type;
    m_loop = o.m_loop;
    m_playing = o.m_playing;
    m_decoderType = o.m_decoderType;
    m_decoder = o.m_decoder;
    m_sampleRate = o.m_sampleRate;
    for (int i = 0; i < STREAM_BUFFERS; i++)
      m_buffers[i] = o.m_buffers[i];
    o.m_source = 0;
    o.m_decoderType = DecoderType::None;
    o.m_decoder.handle = nullptr;
    for (int i = 0; i < STREAM_BUFFERS; i++)
      o.m_buffers[i] = 0;
  }
  return *this;
}

static std::string toLowerStr(const std::string &s) {
  std::string r = s;
  std::transform(r.begin(), r.end(), r.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return r;
}

static std::string getExtension(const std::string &path) {
  size_t dot = path.rfind('.');
  return dot != std::string::npos ? toLowerStr(path.substr(dot)) : "";
}

ALenum Audio::getPlayFormat() const {
  if (m_type == AudioType::Self)
    return AL_FORMAT_MONO16;
  return (m_decoder.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
}

void Audio::destroyDecoder() {
  if (!m_decoder.handle)
    return;
  switch (m_decoderType) {
  case DecoderType::WAV:
    drwav_uninit(static_cast<drwav *>(m_decoder.handle));
    break;
  case DecoderType::MP3:
    drmp3_uninit(static_cast<drmp3 *>(m_decoder.handle));
    break;
  case DecoderType::OGG:
    stb_vorbis_close(static_cast<stb_vorbis *>(m_decoder.handle));
    break;
  default:
    break;
  }
  delete static_cast<char *>(m_decoder.handle);
  m_decoder.handle = nullptr;
  m_decoderType = DecoderType::None;
}

void Audio::decoderSeekStart() {
  if (!m_decoder.handle)
    return;
  switch (m_decoderType) {
  case DecoderType::WAV:
    drwav_seek_to_pcm_frame(static_cast<drwav *>(m_decoder.handle), 0);
    break;
  case DecoderType::MP3:
    drmp3_seek_to_pcm_frame(static_cast<drmp3 *>(m_decoder.handle), 0);
    break;
  case DecoderType::OGG:
    stb_vorbis_seek_start(static_cast<stb_vorbis *>(m_decoder.handle));
    break;
  default:
    break;
  }
}

bool Audio::fillBuffer(ALuint buffer) {
  unsigned int ch = m_decoder.channels;
  std::vector<int16_t> raw(CHUNK_SAMPLES * ch);

  int framesRead = 0;

  switch (m_decoderType) {
  case DecoderType::WAV: {
    drwav *w = static_cast<drwav *>(m_decoder.handle);
    framesRead = static_cast<int>(
        drwav_read_pcm_frames_s16(w, CHUNK_SAMPLES, raw.data()));
    break;
  }
  case DecoderType::MP3: {
    drmp3 *m = static_cast<drmp3 *>(m_decoder.handle);
    framesRead = static_cast<int>(
        drmp3_read_pcm_frames_s16(m, CHUNK_SAMPLES, raw.data()));
    break;
  }
  case DecoderType::OGG: {
    stb_vorbis *v = static_cast<stb_vorbis *>(m_decoder.handle);
    framesRead = stb_vorbis_get_samples_short_interleaved(v, ch, raw.data(),
                                                          CHUNK_SAMPLES * ch);
    break;
  }
  default:
    return false;
  }

  if (framesRead <= 0) {
    if (m_loop) {
      decoderSeekStart();
      return fillBuffer(buffer);
    }
    return false;
  }

  ALenum fmt = getPlayFormat();
  const void *data = raw.data();
  ALsizei size = static_cast<ALsizei>(framesRead * ch * sizeof(int16_t));

  if (m_type == AudioType::Self && ch > 1) {
    std::vector<int16_t> mono(framesRead);
    for (int i = 0; i < framesRead; i++) {
      int32_t sum = 0;
      for (unsigned int c = 0; c < ch; c++)
        sum += raw[i * ch + c];
      mono[i] = static_cast<int16_t>(sum / static_cast<int32_t>(ch));
    }
    data = mono.data();
    size = static_cast<ALsizei>(framesRead * sizeof(int16_t));
  }

  alBufferData(buffer, fmt, data, size, m_sampleRate);
  return alGetError() == AL_NO_ERROR;
}

bool Audio::load(const std::string &filepath) {
  if (!AudioSystem::isInitialized())
    return false;
  destroy();

  std::string ext = getExtension(filepath);

  if (ext == ".wav") {
    auto *w = new drwav;
    if (!drwav_init_file(w, filepath.c_str(), nullptr)) {
      delete w;
      return false;
    }
    m_decoder.handle = w;
    m_decoderType = DecoderType::WAV;
    m_decoder.channels = w->channels;
    m_decoder.sampleRate = w->sampleRate;
  } else if (ext == ".mp3") {
    auto *m = new drmp3;
    if (!drmp3_init_file(m, filepath.c_str(), nullptr)) {
      delete m;
      return false;
    }
    m_decoder.handle = m;
    m_decoderType = DecoderType::MP3;
    m_decoder.channels = m->channels;
    m_decoder.sampleRate = m->sampleRate;
  } else if (ext == ".ogg") {
    int err = 0;
    auto *v = stb_vorbis_open_filename(filepath.c_str(), &err, nullptr);
    if (!v)
      return false;
    stb_vorbis_info info = stb_vorbis_get_info(v);
    m_decoder.handle = v;
    m_decoderType = DecoderType::OGG;
    m_decoder.channels = info.channels;
    m_decoder.sampleRate = info.sample_rate;
  } else {
    return false;
  }

  m_sampleRate = static_cast<ALsizei>(m_decoder.sampleRate);

  alGenSources(1, &m_source);
  alGenBuffers(STREAM_BUFFERS, m_buffers);

  return alGetError() == AL_NO_ERROR;
}

void Audio::play() {
  if (!m_source)
    return;

  for (int i = 0; i < STREAM_BUFFERS; i++) {
    if (!fillBuffer(m_buffers[i]))
      break;
    alSourceQueueBuffers(m_source, 1, &m_buffers[i]);
  }

  alSourcePlay(m_source);
  m_playing = true;
}

void Audio::pause() {
  if (m_source) {
    alSourcePause(m_source);
    m_playing = false;
  }
}

void Audio::stop() {
  if (m_source) {
    alSourceStop(m_source);
    alSourcei(m_source, AL_BUFFERS_QUEUED, 0);
    m_playing = false;
  }
}

void Audio::setLoop(bool loop) { m_loop = loop; }

void Audio::setGain(float gain) {
  if (m_source)
    alSourcef(m_source, AL_GAIN, gain);
}

void Audio::setPitch(float pitch) {
  if (m_source)
    alSourcef(m_source, AL_PITCH, pitch);
}

void Audio::setReferenceDistance(float dist) {
  if (m_source)
    alSourcef(m_source, AL_REFERENCE_DISTANCE, dist);
}

void Audio::setMaxDistance(float dist) {
  if (m_source)
    alSourcef(m_source, AL_MAX_DISTANCE, dist);
}

void Audio::setRolloffFactor(float factor) {
  if (m_source)
    alSourcef(m_source, AL_ROLLOFF_FACTOR, factor);
}

void Audio::setType(AudioType type) {
  m_type = type;
  if (!m_source)
    return;
  if (type == AudioType::Global) {
    alSourcei(m_source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSource3f(m_source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSourcef(m_source, AL_ROLLOFF_FACTOR, 0.0f);
  } else {
    alSourcei(m_source, AL_SOURCE_RELATIVE, AL_FALSE);
    alSourcef(m_source, AL_ROLLOFF_FACTOR, 1.0f);
    alSourcef(m_source, AL_REFERENCE_DISTANCE, 1.0f);
    alSourcef(m_source, AL_MAX_DISTANCE, 50.0f);
  }
}

void Audio::updatePosition(const glm::vec3 &pos) {
  if (m_source && m_type == AudioType::Self)
    alSource3f(m_source, AL_POSITION, pos.x, pos.y, pos.z);
}

void Audio::update() {
  if (!m_source || !m_playing)
    return;

  ALint processed = 0;
  alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);

  while (processed-- > 0) {
    ALuint buf = 0;
    alSourceUnqueueBuffers(m_source, 1, &buf);

    if (fillBuffer(buf)) {
      alSourceQueueBuffers(m_source, 1, &buf);
    } else if (m_loop) {
      alSourceQueueBuffers(m_source, 1, &buf);
    }
  }

  ALint state = 0;
  alGetSourcei(m_source, AL_SOURCE_STATE, &state);
  if (state == AL_STOPPED && !m_loop) {
    m_playing = false;
  }
}

bool Audio::isPlaying() const {
  if (!m_source)
    return false;
  ALint state;
  alGetSourcei(m_source, AL_SOURCE_STATE, &state);
  return state == AL_PLAYING;
}

void Audio::destroy() {
  stop();
  if (m_source) {
    alDeleteSources(1, &m_source);
    m_source = 0;
  }
  if (m_buffers[0]) {
    alDeleteBuffers(STREAM_BUFFERS, m_buffers);
    for (int i = 0; i < STREAM_BUFFERS; i++)
      m_buffers[i] = 0;
  }
  destroyDecoder();
}

} // namespace Engine
