// MKS ESP32 FOC Mega Close Loop Position - Step Response Test
// Library: SimpleFOC 2.2.1 
// Hardware: MKS ESP32 FOC Mega & MKS AS5600

#include <SimpleFOC.h>

MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);
TwoWire I2Cone = TwoWire(0);

// Parâmetros do motor
BLDCMotor motor = BLDCMotor(21);
BLDCDriver3PWM driver = BLDCDriver3PWM(32, 33, 25, 12);

// Variáveis de controle e teste
float target_angle_rad = 0;
uint32_t prev_millis;

// Variáveis do Step Response
bool test_running = false;
uint32_t test_start_time = 0;
float target_angle_deg = 0;

// Tensão de alarme
#define UNDERVOLTAGE_THRES 11.1

Commander command = Commander(Serial);
void doTarget(char *cmd) {
  command.scalar(&target_angle_rad, cmd);
}

// Inicia o teste de Step Response de 10s
void startTest(char *cmd) {
  test_running = true;
  test_start_time = millis();
  Serial.println("Tempo(ms),Alvo(deg),Real(deg),Erro(deg)");
}

void board_check();
float get_vin_Volt();
void board_init();
bool flag_under_voltage = false;

void setup() {
  Serial.begin(115200);
  board_init();

  I2Cone.begin(19, 18, 400000UL);  // AS5600_M0
  sensor.init(&I2Cone);
  motor.linkSensor(&sensor);

  driver.voltage_power_supply = get_vin_Volt();
  driver.init();
  motor.linkDriver(&driver);

  motor.foc_modulation = FOCModulationType::SpaceVectorPWM;
  motor.controller = MotionControlType::angle;

  // Ajuste o PID conforme a necessidade do seu motor para melhorar o tempo de acomodação
  motor.PID_velocity.P = 0.021;
  motor.PID_velocity.I = 0.12;
  motor.P_angle.P = 30;
  
  motor.voltage_limit = 2; // [V] Cuidado ao modificar

  motor.LPF_velocity.Tf = 0.01;
  motor.velocity_limit = 30;

  motor.useMonitoring(Serial);
  motor.init();
  motor.initFOC();
  
  command.add('T', doTarget, "target angle (rad)");
  command.add('S', startTest, "Start 10s Step Response"); // Comando 'S' para iniciar o teste

  Serial.println(F("Motor ready."));
  Serial.println(F("Envie 'S' no terminal serial para iniciar o teste de Step Response de 10s."));
}

void loop() {
  motor.loopFOC();

  // Lógica do Step Response (10 segundos)
  if (test_running) {
    uint32_t elapsed_time = millis() - test_start_time;

    if (elapsed_time < 5000) {
      // Primeiros 5 segundos: vai para 360 graus
      target_angle_deg = 360.0;
    } else if (elapsed_time < 10000) {
      // Últimos 5 segundos: volta para 0 graus
      target_angle_deg = 0.0;
    } else {
      // Fim do teste
      test_running = false;
      target_angle_deg = 0.0;
      Serial.println("Fim do Teste.");
    }

    // Converte o alvo em graus para radianos para o SimpleFOC
    target_angle_rad = target_angle_deg * _PI / 180.0;
  }

  // Move o motor para o alvo em radianos
  motor.move(target_angle_rad);

  board_check();

  if (!flag_under_voltage) {
    command.run();
  }

  // Captura e impressão dos dados em tempo real durante o teste
  if (test_running) {
    float actual_angle_rad = sensor.getAngle();
    float actual_angle_deg = actual_angle_rad * 180.0 / _PI;
    float error_deg = target_angle_deg - actual_angle_deg;

    // Imprime: Tempo (ms), Ângulo Alvo, Ângulo Real, Erro
    Serial.print(millis() - test_start_time);
    Serial.print(",");
    Serial.print(target_angle_deg);
    Serial.print(",");
    Serial.print(actual_angle_deg);
    Serial.print(",");
    Serial.println(error_deg);
  }
}

// --- Funções Auxiliares de Hardware ---
void board_init() {
  pinMode(32, INPUT_PULLUP);
  pinMode(33, INPUT_PULLUP);
  pinMode(25, INPUT_PULLUP);

  analogReadResolution(12);

  float VIN_Volt = get_vin_Volt();
  while (VIN_Volt <= UNDERVOLTAGE_THRES) {
    VIN_Volt = get_vin_Volt();
    delay(100);
    Serial.printf("Waiting for power-up... Current voltage:%.2f\n", VIN_Volt);
  }
  Serial.printf("Calibrating motor... Current voltage:%.2f\n", VIN_Volt);
}

float get_vin_Volt() {
  return analogReadMilliVolts(13) * 8.5 / 1000;
}

void board_check() {
  uint32_t curr_millis = millis();
  static uint8_t enableState = 0;

  if (curr_millis - prev_millis >= 1000) {
    float vin_Volt = get_vin_Volt();

    if (vin_Volt < UNDERVOLTAGE_THRES) {
      flag_under_voltage = true;
      enableState = 0;
      uint8_t count = 5;
      while (count--) {
        vin_Volt = get_vin_Volt();
        if (vin_Volt > UNDERVOLTAGE_THRES) {
          flag_under_voltage = false;
          break;
        }
      }
    } else {
      flag_under_voltage = false;
    }
    
    if (flag_under_voltage) {
      motor.disable();
    } else if (0 == enableState && flag_under_voltage == false) {
      enableState = 1;
      motor.enable();
    }
    prev_millis = curr_millis;
  }
}