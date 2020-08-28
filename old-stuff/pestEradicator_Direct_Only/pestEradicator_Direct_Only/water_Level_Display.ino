void water_level_display(){
tft.textMode();
  tft.textSetCursor(100, 25);
  tft.textColor(RA8875_BLUE, RA8875_WHITE);
  tft.textEnlarge(1);
  tft.textWrite("Hot Water:", 10);
  tft.textSetCursor(100, 60);
  checkWaterLevel();
  String temp1 = "Level: " + String(waterLevel) + "%";
  int size1 = temp1.length();
  char char_array1[size1 + 1];
  strcpy(char_array1, temp1.c_str());
  tft.textWrite(char_array1, 11);
  }
