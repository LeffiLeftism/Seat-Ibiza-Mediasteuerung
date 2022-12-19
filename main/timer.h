namespace  {
  class Timer {
    private:
      uint32_t m_start;
      uint32_t m_duration;
      bool m_isStarted;
      bool m_isFinished;
    public:
      Timer(uint32_t duration) {
        this->m_duration = duration;
        //init();
      }
      void init() {
        m_isStarted = 0;
        m_isFinished = 0;
      }
      void start() {
        m_isStarted = 1;
        m_isFinished = 0;
        m_start = millis();
      }
      void update() {
        if (m_isStarted) {
          if (millis() > (m_start + m_duration)) {
            m_isFinished = 1;
          }
        }
      }
      void reset() {
        m_isStarted = 0;
        m_isFinished = 0;
      }
      bool isStartet() {
        return m_isStarted;
      }
      bool isFinished() {
        return m_isFinished;
      }
  };
};