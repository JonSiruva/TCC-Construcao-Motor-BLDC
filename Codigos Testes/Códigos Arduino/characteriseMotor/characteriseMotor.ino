#include <SimpleFOC.h>

BLDCMotor motor = BLDCMotor(21);

BLDCDriver3PWM driver = BLDCDriver3PWM(32, 33, 25, 12);

InlineCurrentSense current_sense = InlineCurrentSense(0.01, 50.0, 39, 36, _NC);


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  SimpleFOCDebug::enable(&Serial);

  driver.voltage_power_supply = 12;
  driver.init();

  motor.linkDriver(&driver);

  current_sense.linkDriver(&driver);
  current_sense.init();

  motor.linkCurrentSense(&current_sense);

  motor.init();

  Serial.println("Iniciando caracterização do motor...");
  Serial.println("Mantenha o eixo do motor PARADO");

  motor.characteriseMotor(2);

  Serial.println("\n--- Resultados da Caracterizacao ---");

  Serial.print("Resistencia de Fase (R): ");
  Serial.print(motor.phase_resistance);
  Serial.println(" Ohms");

  Serial.print("Indutancia Eixo D (Ld): ");
  Serial.print(motor.axis_inductance.d * 1000);
  Serial.println(" mH");

  Serial.print("Indutancia Eixo Q (Lq): ");
  Serial.print(motor.axis_inductance.q * 1000);
  Serial.println(" mH");

  Serial.println("\n Caracterização concluída");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
}
