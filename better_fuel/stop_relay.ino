  //  Realy on, off management
void realayToggle()
{
  if (currentstart_stop == HIGH && laststart_stop == LOW) {
    if (RelayState == HIGH) {        //Toggle the state of the Relay
      digitalWrite(Relay, LOW);
      RelayState = LOW;
    }
    else {
      digitalWrite(Relay, HIGH);
      RelayState = HIGH;
    }
  }
}
