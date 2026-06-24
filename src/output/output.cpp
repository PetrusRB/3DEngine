#include "output.h"

namespace Engine {
void OutputTerm::addLog(std::string wtf, float currentTime) {
  if (currentTime - m_lastLogTime < m_cooldown) {
    return;
  }
  m_lastLogTime = currentTime;
  m_outputBuffer[m_writeIndex] = wtf;
  m_writeIndex = (m_writeIndex + 1) % BUFFER_SIZE;
}
} // namespace Engine
