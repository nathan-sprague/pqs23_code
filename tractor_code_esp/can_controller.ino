
CAN_device_t CAN_cfg;               // CAN Config
unsigned long previousMillis = 0;   // will store last time a CAN Message was send
const int interval = 1000;          // interval at which send CAN Messages (milliseconds)
const int rx_queue_size = 1;       // Receive Queue size

unsigned long lastSendTime = 0; // millis time when CAN message was last sent
unsigned int sendFrequency = 100; // milliseconds between sending CAN message



void init_CAN() {
  Serial.println("Starting CAN");
  CAN_cfg.speed = CAN_SPEED_250KBPS; // set up can speed
  CAN_cfg.tx_pin_id = GPIO_NUM_5; // define pin numbers for CAN
  CAN_cfg.rx_pin_id = GPIO_NUM_4;
  CAN_cfg.rx_queue = xQueueCreate(rx_queue_size, sizeof(CAN_frame_t));

  // Initialize CAN Module
  ESP32Can.CANInit();

}


void send_CAN() {

  if (millis() - lastSendTime > sendFrequency) {

    // throttle
    // convert target throttle (0-100) to a byte
    int throttleSignal = targetThrottle * 2.55;

    // set up the new CAN message to send the throttle position
    CAN_frame_t throttle_tx_frame;
    throttle_tx_frame.FIR.B.FF = CAN_frame_ext;
    throttle_tx_frame.MsgID = 0xCF01A00;
    throttle_tx_frame.FIR.B.DLC = 8;
    throttle_tx_frame.data.u8[0] = 0; //throttleSignal % 250; // fine
    throttle_tx_frame.data.u8[1] = throttleSignal * 0.6; // coarse
    throttle_tx_frame.data.u8[2] = 0x00;
    throttle_tx_frame.data.u8[3] = 0x00;
    throttle_tx_frame.data.u8[4] = 0x00;
    throttle_tx_frame.data.u8[5] = 0x00;
    throttle_tx_frame.data.u8[6] = 0x00;
    throttle_tx_frame.data.u8[7] = 0x00;
    ESP32Can.CANWriteFrame(&throttle_tx_frame); // give the throttle position



    // steering actuator
    
    // convert units for steering actuator (to fit our tractor's turning amount)
    int steerSignal = (((targetSteer + 100) / 2 * 2.55) + 250) * 1.8;

    CAN_frame_t steer_tx_frame;

    // set up CAN message to send
    steer_tx_frame.FIR.B.FF = CAN_frame_ext;
    steer_tx_frame.MsgID = 216994560;
    steer_tx_frame.FIR.B.DLC = 8;
    steer_tx_frame.data.u8[0] = steerSignal % 250; // position
    steer_tx_frame.data.u8[1] = steerSignal / 250 + 128; // position (128-131)
    steer_tx_frame.data.u8[2] = 62; // speed (62 or 190)
    steer_tx_frame.data.u8[3] = 23; // speed & reset (23 or 24)
    steer_tx_frame.data.u8[4] = 0;
    steer_tx_frame.data.u8[5] = 0;
    steer_tx_frame.data.u8[6] = 0;
    steer_tx_frame.data.u8[7] = 0;
    ESP32Can.CANWriteFrame(&steer_tx_frame);


    lastSendTime = millis();
  }


}



void read_CAN() {
  CAN_frame_t rx_frame;

  // Receive next CAN frame from queue
  if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) { // a CAN message was received


    if (rx_frame.FIR.B.RTR != CAN_RTR) { // it is a typical CAN message

      unsigned long msgId = rx_frame.MsgID; // what type of message it is


      if (msgId == 217962035) { // joystick position ID

        // get the Y joystick position
        int dir = rx_frame.data.u8[2];
        if (dir == 4 || dir == 68 || dir == 132 || dir == 196) { // basically whether it is left/right
          dir = -1;
        } else {
          dir = 1;
        }
        pos[1] = (rx_frame.data.u8[3] * dir) / 2.55; // convert to -100 to +100


        // get the X joystick position
        dir = rx_frame.data.u8[0];
        if (dir == 4 || dir == 68 || dir == 132 || dir == 196) {
          dir = -1;
        } else {
          dir = 1;
        }
        pos[0] = (rx_frame.data.u8[1] * dir) / 2.55; // convert to -100 to +100


        int byte6 = rx_frame.data.u8[6]; // grip pressed
        if (byte6 % 2 == 1) {
          gripPressed = true;
        }
      }

      else if (msgId == 216994560) { // steering actuator position ID
        // don't really need this.
        // May use for confirming that the actuator is working later.
      }
    }
  }
}
