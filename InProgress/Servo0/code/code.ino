#include <Servo.h>

Servo esc_right;
Servo esc_left;

int pot_vertical = A0;//Analog pin
int pot_horizontal = A1;//Analog pin

int pot_vec[2] = {0,0};//vector of pot values, where center is 0,0.
int servo_vec[2] = {0,0};//servo values, 0: left, 1: right.

void setup() {
  esc_right.attach(9);
  esc_right.attach(10);
  pinMode(pot_vertical, INPUT);
  pinMode(pot_horizontal, INPUT);
}

void loop() {
  joystick_to_vector(pot_vec);
  vector_to_servo(pot_vec,servo_vec);
  
  esc_left.writeMicroseconds(servo_vec[0]);
  esc_right.writeMicroseconds(servo_vec[1]);
}

void joystick_to_vector(int* vector)
{
  int MIDDLE_X = 512;
  int MIDDLE_Y = 512;
  int TOL = 40;
  int MAX_DELTA = 10;
  
  int x = analogRead(pot_vertical) - MIDDLE_X;
  int y = analogRead(pot_horizontal) - MIDDLE_Y;
  int dx = abs(vector[0] - x);
  int dy = abs(vector[1] - y);
  boolean same_x = dx < TOL;
  boolean same_y = dy < TOL;
  if(dx > MAX_DELTA) dx = MAX_DELTA;
  if(dy > MAX_DELTA) dy = MAX_DELTA;
  if(!same_x) vector[0] += dx;
  if(!same_y) vector[1] += dy;
  if(x < TOL) vector[0] = 0;
  if(y < TOL) vector[1] = 0;
}

void vector_to_servo(int* vector,int* servo)
{
  int MAX_MAG = 512;
  int SERVO_MIN = 1000;
  int SERVO_MAX = 2000;
  int SERVO_RANGE = SERVO_MAX-SERVO_MIN;
  
  int mag = sqrt(vector[0]*vector[0] + vector[1]*vector[1]);
  double scale = mag / MAX_MAG;
  if(scale > 1)
  {
    vector[0] = (int) (vector[0] / scale);
    vector[1] = (int) (vector[1] / scale);
    mag = MAX_MAG;
  }
  //servo[0]: left, servo[1]: right
  servo[0] = SERVO_MIN + mag * SERVO_RANGE / MAX_MAG;
  servo[1] = SERVO_MIN + mag * SERVO_RANGE / MAX_MAG;
  int subtraction = abs (vector[0] * SERVO_RANGE / MAX_MAG);
  if(vector[0] < 0) servo[0] -= subtraction;
  if(vector[0] > 0) servo[1] -= subtraction;
}
