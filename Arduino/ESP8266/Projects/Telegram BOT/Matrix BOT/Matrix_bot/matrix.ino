// Draw on matrix
void matrix(String& disp) {

  ledMatrix.setNextText(disp);
  ledMatrix.clear();
  ledMatrix.scrollTextLeft();
  ledMatrix.drawText();
  
  scroll = ledMatrix.scrollEnd(); // does the matrix has finished to display?
  
  Serial.println(scroll);
  ledMatrix.commit(); // commit transfers the byte buffer to the displays

  delay(text_speed);
}
