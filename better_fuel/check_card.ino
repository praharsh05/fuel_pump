boolean check_card(String card) {
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return false;
  }
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return false;
  }
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  if (content.substring(1) == card) {
    return true;
  }
  else {
    return false;
  }
}
