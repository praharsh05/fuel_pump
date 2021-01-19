int Set() {
  if (RelayState == LOW) {   // You only can change settings while system is not running!

    //-----Adders Buttons (set point_1)---//
    if (currentadd_one == HIGH && lastadd_one == LOW) { // Add +1
      set_point_1 = set_point_1 + 1;
    }
    lastadd_one = currentadd_one;

    if (currentadd_ten == HIGH && lastadd_ten == LOW) { // Add +10
      set_point_1 = set_point_1 + 10;
    }
    lastadd_ten = currentadd_ten;

    if (currentadd_cien == HIGH && lastadd_cien == LOW) { // Add +100
      set_point_1 = set_point_1 + 100;
    }
    lastadd_cien = currentadd_cien;

    if (currentadd_mil == HIGH && lastadd_mil == LOW) { // Add +1000
      set_point_1 = set_point_1 + 1000;
    }
    lastadd_mil = currentadd_mil;

  }
  return set_point_1;
}
