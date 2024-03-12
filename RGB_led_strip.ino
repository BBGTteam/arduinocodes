void rgb(int fadespeed){  
    int maxV = 80;
    int r = 0;
    int g = 0;
    int b = maxV;
  unsigned long curMillis = millis();
  unsigned long prevMillis = 0;
  
 // fade from blue to violet

  while (b > 0) { 
    curMillis = millis();
    if (curMillis - prevMillis >= fadespeed) {
      prevMillis = curMillis;
      analogWrite(GABI_BLUE_PIN, b);
      b--;
      analogWrite(GABI_RED_PIN, r);
      r++; 

  }
  } 
  b = 0;

  while (r >= 0) { 
    curMillis = millis();
    if (curMillis - prevMillis >= fadespeed) {
      prevMillis = curMillis;
      analogWrite(GABI_RED_PIN, r);
      r--;
      analogWrite(GABI_GREEN_PIN, g);
      g++;

    } 
  } 

  // fade from teal to blue
  while (g >= 0) { 
     curMillis = millis();
    if (curMillis - prevMillis >= fadespeed) {
      prevMillis = curMillis;
      analogWrite(GABI_GREEN_PIN, g);
      g--;
      analogWrite(GABI_BLUE_PIN, b);
      b++;
      
    }
  } 

Serial.println("END_RGB");
}
