int ltr_func() {
  if (RelayState == LOW) {   // You only can change settings while system is not running!
    //-------Reset Buttons----//
    if (currentrst_sp == HIGH && lastrst_sp == LOW) { //Reset Set Point
      setPointValue(0);
      set_point_1 = 0;
    }
    lastrst_sp = currentrst_sp;
    if (currentrst_cnt == HIGH && lastrst_cnt == LOW) { //Reset Counter
      setCounter(0);
      TotalCount_2 = 0;
    }
    lastrst_cnt = currentrst_cnt;
  }//-----End Settings-----//

  if (card_pre) {
    setPointValue(set_point_1);
    setAuth(veh);
    quo_int = qty.toFloat();
    setMrnQuantity(quo_int);
    setRemainingQuantity(remainingQty);

    //    check if quantity is less than allowed quota
    if (remainingQty < set_point_1) {
      RelayState = LOW;
      digitalWrite(Relay, LOW);
      lcdsetup();
      TotalCount_1 = 0;
      set_point_1 = 0;
      fuel_start = false;
      setAuth("Quota Exceed");
      delay(5000);
      lcdsetup();
      return;
    }
    //     starting to pump fuel
    if (RelayState == HIGH) {  // Only counts while relay is HIGH
      if (!fuel_start) {
        currentTime = millis();
        fuel_start = true;
        temp_quantity = remainingQty;
      }
      time_lapse = millis() - currentTime;
//      Serial.println("time lapase " + String(time_lapse));
      TotalCount_2 = float(time_lapse / 25000.0f) ;
//      Serial.println(" count in ltr " + String(TotalCount_2));
      if (set_point_1 >= TotalCount_2) {
        setCounter(TotalCount_2);
        remainingQty = temp_quantity - TotalCount_2;
        setRemainingQuantity(remainingQty);
      }
      if (set_point_1 <= TotalCount_2) {
        RelayState = LOW;
        digitalWrite(Relay, LOW);
        setCounter(set_point_1);
        remainingQty = temp_quantity - set_point_1;
        setRemainingQuantity(remainingQty);
        TotalCount_2 = 0;
        delay(4000);
        lcdsetup();
      }
    }
  }
  else {
    //    delay(4000);
    RelayState = LOW;
    digitalWrite(Relay, LOW);
    //***********************************************Autoreset
    TotalCount_2 = 0;
    fuel_start = false;
  }
}
