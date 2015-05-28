#include <Servo.h>

Servo esc_right;
Servo esc_left;

typedef double dtype; 

int esc_pin_right = 9;//50 Hz PWM output pin
int esc_pin_left = 10;//50 Hz PWM output pin

int ud_pin = A0;//Analog input pin
int lr_pin = A1;//Analog input pin

int vector_buffer[2] = {0,0};
int vector[2] = {0,0};
int servo[2] = {0,0};

void setup() {
  esc_right.attach(esc_pin_right);
  esc_left.attach(esc_pin_left);
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
    vector_to_servo();
//    Serial.print(vector[0]);
//    Serial.print(", ");
//    Serial.println(vector[1]);
    Serial.print(servo[0]);
    Serial.print(", ");
    Serial.println(servo[1]);
    vector_to_buffer();
  }
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

void vector_to_servo()
{
  int MAX_MAG = 460;
  int SERVO_MAX = 2000;
  int SERVO_MIN = 1000;
  int SERVO_RANGE = SERVO_MAX-SERVO_MIN;
  int SAME_THRUST_ZONE = 200;
  
  int POT_MAX_Y = 500;
  int POT_MIN_Y = 480;
  int POT_MAX_X = 500;
  int POT_MIN_X = 470;
  int POT_ZERO_ZONE = 40;
  
  int ud = vector[1];
  int lr = vector[0];
  if(ud < 0) ud = 0;
  dtype sumsq = sq((dtype)lr) + sq((dtype)ud);
  dtype mag = sqrt(sumsq);
//  Serial.println(mag);
  mag -= POT_ZERO_ZONE;
  dtype scale = mag / MAX_MAG;
  if(scale > 1)
  {
    lr = (int) ((dtype)lr / scale);
    ud = (int) ((dtype)ud / scale);
    mag = MAX_MAG;
  }
  //servo[0]: left, servo[1]: right
  servo[0] = SERVO_MIN + ((dtype)mag * (dtype)SERVO_RANGE) / (dtype)MAX_MAG;
  servo[1] = SERVO_MIN + ((dtype)mag * (dtype)SERVO_RANGE) / (dtype)MAX_MAG;
//  Serial.println(mag);
  int subtraction = abs (((dtype)lr * (dtype)SERVO_RANGE) / (dtype)MAX_MAG);
  subtraction -= SAME_THRUST_ZONE;
  if(subtraction < 0) subtraction = 0;
  if(lr < 0) servo[0] -= subtraction;
  if(lr > 0) servo[1] -= subtraction;
}

