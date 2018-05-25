#include <Servo.h>
#include <ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Float32.h>
#include <avr/io.h>
Servo sMotor;
ros::NodeHandle nh;
geometry_msgs::Twist info;
ros::Publisher chatter("InfoArduino", &info);

const int ADELANTE = 30;
const int ATRAS = 32;
const int ENABLE = 2;
const int IZQUIERDA = 6;
const int DERECHA = 7;
const int SERVO = 8;
int vueltas = 0;
unsigned long tiempo1;
unsigned long tiempo2;
unsigned long tiempo3;
unsigned long tiempo4;
int izq;
int der;
int avanzar;
int vDeseada;
int cambio;
void avanza(const std_msgs::Float32& msg)
{
  if(msg.data == 1)
    avanzar = 1;
  if(msg.data == 0)
    avanzar = 0;
}

ros::Subscriber<std_msgs::Float32> sub("Avanza", &avanza); 

void setup() {
  // put your setup code here, to run once:
  cli();
  TCCR5B = B00001111;
  TCNT5 = 240;
  TCCR5B |= (1 << WGM01); //Esto hace que vuelva a su valor de 240
  TIMSK5 |= (1 << OCIE0A); // enable timer compare interrupt
  pinMode(47,INPUT); //PIN T5 de ATMEGA
  Serial.begin(57600);
  nh.initNode();
  nh.advertise(chatter);
  nh.subscribe(sub);
  pinMode(ADELANTE, OUTPUT);
  pinMode(ATRAS, OUTPUT);
  pinMode(ENABLE, OUTPUT);
  pinMode(IZQUIERDA, INPUT);
  pinMode(DERECHA, INPUT);
  sMotor.attach(SERVO);
  delay(2000);
  tiempo1 = millis();
  tiempo3 = millis();
  sMotor.write(90);
  avanzar = 0;
  vDeseada = 200;
  cambio = 0;
  sei();
}
ISR(TIMER5_COMPA_vect){  
   vueltas++;
   TCNT5 = 240;
}
void loop() {
  // put your main code here, to run repeatedly:
  if(avanzar == 1)
  {
    analogWrite(ENABLE, vDeseada);  
    adelante();
    tiempo2 = millis();
    if(tiempo2-tiempo1>= 1000)
    {
        double vel = vueltas + (TCNT5-240.0)/16.0;
        vueltas = 0;
        tiempo1 = tiempo2;
        TCNT5 = 240;
        info.linear.x = vel;
    }
    direccion();
  }
  else
  {
    analogWrite(ENABLE,0);
  }
  tiempo4 = millis();
  if(tiempo4 - tiempo3 >= 1000)
  {
    tiempo3 = tiempo4;
    chatter.publish( &info);
    nh.spinOnce();
  }
}
void adelante()
{
  digitalWrite(ATRAS, LOW);
  digitalWrite(ADELANTE, HIGH);
}
void atras()
{
  digitalWrite(ADELANTE, LOW);
  digitalWrite(ATRAS, HIGH);
}
void direccion()
{
  izq =digitalRead(IZQUIERDA);
  der =digitalRead(DERECHA);
  info.linear.y = izq;
  info.linear.z = der;
  if(izq == 1 && der == 1)
  {
    vDeseada = 140;
    if(cambio <= 3)
    {
      sMotor.write(110);
      delay(150);
      cambio++;
    }
    else
    {
      sMotor.write(70);
      delay(150);
      cambio++;
      if(cambio >= 6)
      {
        cambio = 0;
      }
    }
  }
  else if(izq == 1)
  {
    vDeseada = 170;
    sMotor.write(110);
    delay(150);
  }
  else if(der == 1)
  {
    vDeseada = 170;
    sMotor.write(70);
    delay(150);
  }
  else
  {
    vDeseada = 200;
    sMotor.write(90);
    delay(150);
  }

}

