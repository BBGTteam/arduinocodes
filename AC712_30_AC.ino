
int getAmper(ACS712 room, int correction){
  
  room.autoMidPoint();

  int mA = room.mA_AC();
  
  int mA_corrected = mA+correction;
  
  
  if (mA_corrected < 0){
    mA_corrected = 0;
  }
  
  return mA_corrected;  
}
