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
  }
  return set_point_1;
}
