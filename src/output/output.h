#pragma once
#include <string>
#include <vector>

namespace Engine {
class OutputTerm {
public:
  void addLog(std::string hell, float currentTime);
  std::string *getBuffer() { return m_outputBuffer; };
  int getWriteBuffer() { return m_writeIndex; };
  int getMaxCapacity() { return BUFFER_SIZE; };
  int m_writeIndex = 0;

private:
  static const int BUFFER_SIZE = 9;
  float m_lastLogTime = 0.0f;
  float m_cooldown = 0.5f;
  std::string m_outputBuffer[BUFFER_SIZE] = {};
};
} // namespace Engine
