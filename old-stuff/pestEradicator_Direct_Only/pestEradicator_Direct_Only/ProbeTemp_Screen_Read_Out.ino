void ProbeTemp_screen(String Probe_number ,float tempF,int x,int y){
//  tft.graphicsMode();
//  tft.fillRect(20,20,750,950, RA8875_BLUE);

  tft.textMode();
  tft.textSetCursor(x,y);
  tft.textColor(RA8875_BLUE, RA8875_WHITE);
  tft.textEnlarge(1);
  String cTemp = "Probe "+ Probe_number +": "+ String(tempF) + " F";
  int sizet = cTemp.length();
  char char_arrayt[sizet + 1];
  strcpy(char_arrayt, cTemp.c_str());
  tft.textWrite(char_arrayt, 17);
}
