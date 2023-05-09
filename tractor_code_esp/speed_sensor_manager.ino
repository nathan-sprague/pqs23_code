

void init_geartooth(){

  pinMode(GT_PIN,INPUT);
  attachInterrupt(GT_PIN, tick, RISING);
}


void tick(){
  if (millis() - lastTickTime > 5){
     tickCount++;
     lastTickTime = millis();
  }
}


void calcSpeeds(){
  unsigned int timePassed = millis() - lastSpeedCalcTime;
  if (timePassed > 100){
    
    byte numTeeth = 12;
  
    shaftRPM = tickCount * 60 * 1000.0 / timePassed / numTeeth;
    tickCount = 0;
    lastSpeedCalcTime = millis();
  }
}
