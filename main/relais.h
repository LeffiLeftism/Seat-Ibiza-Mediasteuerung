namespace  {
  class Relais {
    private:
      uint8_t m_pin;
      bool m_state;
      void update(){
        digitalWrite(m_pin, m_state);
        delay(1250);
      }
    public:
      Relais(uint8_t pin){
        this->m_pin = pin;
        //init();
      }
      void init(){
        pinMode(m_pin, OUTPUT);
        m_state = 0;
      }
      void on(){
        if (!m_state) {
          m_state = 1;
          update();
        }
      }
      void off(){
        if (m_state) {
          m_state = 0;
          update();
        }
      }
      bool get(){
        return m_state;
      }
  };
};