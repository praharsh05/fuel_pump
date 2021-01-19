boolean debounce(boolean last, int pin)
{
  boolean current = digitalRead(pin);
  if (last != current)
  {
    delay(10);
    current = digitalRead(pin);
  }
  return current;
}
