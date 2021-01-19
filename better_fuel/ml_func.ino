int ml_func() {

  if (RelayState == LOW) {   // You only can change settings while system is not running!

    //    //-----Adders Buttons (set point_1)---//
    //
    //    if (currentadd_ten == HIGH && lastadd_ten == LOW) { // Add +10
    //      set_point_1 = set_point_1 + 10;
    //    }
    //    lastadd_ten = currentadd_ten;
    //
    //    if (currentadd_cien == HIGH && lastadd_cien == LOW) { // Add +100
    //      set_point_1 = set_point_1 + 100;
    //    }
    //    lastadd_cien = currentadd_cien;
    //
    //    if (currentadd_mil == HIGH && lastadd_mil == LOW) { // Add +1000
    //      set_point_1 = set_point_1 + 1000;
    //    }
    //    lastadd_mil = currentadd_mil;

    //-------Reset Buttons----//
    if (currentrst_sp == HIGH && lastrst_sp == LOW) { //Reset Set Point
      tft.setCursor(15, 20);
      tft.setTextColor(0xFFFF, 0xF800);
      tft.setTextSize(4);
      tft.println("       ");// Clear SP area
      set_point_1 = 0;
    }
    lastrst_sp = currentrst_sp;
    if (currentrst_cnt == HIGH && lastrst_cnt == LOW) { //Reset Counter
      tft.setCursor(20, 80);
      tft.setTextColor(0xFFFF, 0x07E0);
      tft.setTextSize(4);
      tft.println("       ");// Clear CNT area
      tft.fillRect(10, 250, 220, 50, RED);
      tft.drawRect(10, 250, 220, 50, WHITE);
      tft.setCursor(30, 265);
      tft.setTextColor(WHITE);
      tft.setTextSize(3);
      tft.print("AUTHENTIC");
      //      counter_1 = 0;
      TotalCount_1 = 0;
    }
    lastrst_cnt = currentrst_cnt;
  }//-----End Settings-----//


  tft.setCursor(15, 20);
  tft.setTextColor(0xFFFF, 0xF800);
  tft.setTextSize(4);
  tft.println(set_point_1);//Show set point

  if (card_pre) {
    tft.setCursor(30, 265);
    tft.setTextColor(RED);
    tft.setTextSize(3);
    tft.println("          ");// Clear Authenticate Area
    tft.fillRect(10, 250, 220, 50, GREEN);
    tft.drawRect(10, 250, 220, 50, WHITE);
    tft.setCursor(30, 265);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    veh_no =  read_String(addr_veh);
    tft.print(veh_no);
    //    tft.print("CONTMAC-50087");
    tft.fillRect(10, 130, 220, 50, MAGENTA);
    tft.drawRect(10, 130, 220, 50, WHITE);
    tft.setCursor(20, 140);
    tft.setTextColor(WHITE);
    tft.setTextSize(4);
    quo = read_String(addr_qty);
    quo_int = quo.toInt();
    quo_int = quo_int * 10;
    tft.print(quo_int);


    //----Start Counter------//
    if (RelayState == HIGH) {  // Only counts while relay is HIGH
      //      Serial.println("relay on");
      if (!fuel_start) {
        currentTime = millis();
        fuel_start = true;
      }

      time_lapse = millis() - currentTime;
      TotalCount_1 = int(time_lapse / 250) * 10;
      //      Serial.println(time_lapse);
      //      Serial.println(int(time_lapse / 250));
      //      Serial.println(TotalCount_1);

      tft.setCursor(20, 80);
      tft.setTextColor(0xFFFF, 0x07E0);
      tft.setTextSize(4);
      tft.println(TotalCount_1);// Show count

      //--Stop Counter.You can't start if set point is lower or equal to counter--//
      if (RelayState == HIGH) {
        if (quo_int < set_point_1) {
          RelayState = LOW;
          digitalWrite(Relay, LOW);
          tft.setCursor(15, 20);
          tft.setTextColor(0xFFFF, 0xF800);
          tft.setTextSize(4);
          tft.println("       ");// Clear SP area
          tft.setCursor(20, 80);
          tft.setTextColor(0xFFFF, 0x07E0);
          tft.setTextSize(4);
          tft.println("       ");// Clear CNT area
          tft.fillRect(10, 250, 220, 50, RED);
          tft.drawRect(10, 250, 220, 50, WHITE);
          tft.setCursor(30, 265);
          tft.setTextColor(WHITE);
          tft.setTextSize(3);
          tft.print("AUTHENTIC");

          TotalCount_1 = 0;
          set_point_1 = 0;
          fuel_start = false;
        }
        //        Serial.println("total " + String(TotalCount_1) + " set point " + String(set_point_1));
        if (set_point_1 <= TotalCount_1) {
          RelayState = LOW;
          digitalWrite(Relay, LOW);
          quo_int = quo_int - TotalCount_1;
          tft.fillRect(10, 190, 220, 50, BLUE);
          tft.drawRect(10, 190, 220, 50, WHITE);
          tft.setCursor(15, 200);
          tft.setTextColor(WHITE);
          tft.setTextSize(4);
          tft.print(quo_int);
          writeString(addr_qty, String(quo_int));
          //***********************************************Autoreset
          tft.setCursor(20, 80);
          tft.setTextColor(0xFFFF, 0x07E0);
          tft.setTextSize(4);
          tft.println("       ");// Clear CNT area
          tft.fillRect(10, 250, 220, 50, RED);
          tft.drawRect(10, 250, 220, 50, WHITE);
          tft.setCursor(30, 265);
          tft.setTextColor(WHITE);
          tft.setTextSize(3);
          tft.print("AUTHENTIC");
          TotalCount_1 = 0;
          fuel_start = false;
        }
      }
    }
  }
  else {
    RelayState = LOW;
    digitalWrite(Relay, LOW);
    //***********************************************Autoreset
    tft.setCursor(20, 80);
    tft.setTextColor(0xFFFF, 0x07E0);
    tft.setTextSize(4);
    tft.println("       ");// Clear CNT area
    tft.fillRect(10, 130, 220, 50, MAGENTA);
    tft.drawRect(10, 130, 220, 50, WHITE);
    tft.setCursor(20, 140);
    tft.setTextColor(0xFFFF, 0xF81F);
    tft.setTextSize(4);
    tft.print("       ");
    tft.fillRect(10, 250, 220, 50, RED);
    tft.drawRect(10, 250, 220, 50, WHITE);
    tft.setCursor(30, 265);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.print("AUTHENTIC");
    TotalCount_1 = 0;
    fuel_start = false;
  }
}
