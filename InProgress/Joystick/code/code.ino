int ud_pin = A0;//Analog pin
int lr_pin = A1;//Analog pin

int vector_buffer[2] = {0,0};
int vector[2] = {0,0};

void setup() {
  pinMode(ud_pin, INPUT);
  pinMode(lr_pin, INPUT);
  Serial.begin(9600);
}

void loop() {
  boolean changed;
  changed = joystick_to_vector();
//  print_state();
  changed = noise_filter();
//  changed = changed || zero_zone();
  if(changed)
  {
    Serial.print(vector[0]);
    Serial.print(", ");
    Serial.println(vector[1]);
  }
  if(changed) vector_to_buffer();
//  delay(100);
}
boolean joystick_to_vector()
{
  int MIDDLE_X = 470;
  int MIDDLE_Y = 512;

  vector[0] = analogRead(lr_pin) - MIDDLE_X;
  vector[1] = analogRead(ud_pin) - MIDDLE_Y;
  boolean changed = false;
  if(vector[0] != vector_buffer[0]) changed = true;
  if(vector[1] != vector_buffer[1]) changed = true;
}
boolean noise_filter()
{//Returns true if vector was changed.
  int TOL = 10;
  
  int dx = abs(vector[0] - vector_buffer[0]);
  int dy = abs(vector[1] - vector_buffer[1]);
  boolean changed = false;
  if(dx < TOL) vector[0] = vector_buffer[0];
  else changed = true;
  if(dy < TOL) vector[1] = vector_buffer[1];
  else changed = true;
  return changed;
}
boolean zero_zone()
{
  int TOL = 40;
  
  boolean changed = false;
  if(abs(vector[0]) < TOL && vector_buffer[0] != 0)
  {
    vector[0] = 0;
    changed = true;
  }
  if(abs(vector[1]) < TOL && vector_buffer[1] != 0)
  {
    vector[1] = 0;
    changed = true;
  }
}
void vector_to_buffer()
{
  vector_buffer[0] = vector[0];
  vector_buffer[1] = vector[1];
}
void print_state()
{
  Serial.print("vector: ");
  Serial.print(vector[0]);
  Serial.print(", ");
  Serial.print(vector[1]);
  Serial.print(", ");
  Serial.print("vector_buffer: ");
  Serial.print(vector_buffer[0]);
  Serial.print(", ");
  Serial.println(vector_buffer[1]);
}
