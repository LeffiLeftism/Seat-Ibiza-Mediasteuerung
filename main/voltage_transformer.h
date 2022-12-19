namespace {
  class VoltageTransformer {
    private:
      byte m_pin;
      uint16_t m_r1;          //Resistor in kOhms, 0...65535
      uint16_t m_r2;          //Resistor in kOhms, 0...65535
      uint16_t m_reading;     //Raw value from ADC 0...1023
      float m_factor;         //Factor to calculate Voltage = factor * reading
      float m_voltage;        //Calculated Voltage
      void update(){ //Reads m_pin and calculates m_voltage
        m_reading = analogRead(m_pin);
        m_voltage = m_reading * m_factor;
      }
    public:
      VoltageTransformer(byte pin, uint16_t r1, uint16_t r2) {
        this->m_pin = pin;
        this->m_r1 = r1;
        this->m_r2 = r2;
        //init();
      }      
      void init(){ //Set pinmode, calculate m_factor and update
        pinMode(m_pin, INPUT);
        m_factor = 5 / 1024; //Max 5V on pin, 10-Bit converter -> 5V/2^10 ~ 0.0049 V/Unit
        update();
      }
      float get() { //Updates and returns m_voltage
        update();
        return m_voltage;
      }
  };
};