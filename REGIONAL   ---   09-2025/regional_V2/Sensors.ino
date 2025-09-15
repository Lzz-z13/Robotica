
// GY sensors
int read_sensors_pure(uint8_t index) {
  if (index >= N_SENSORS) return -1;
  tcaselect(index);
  return sensors[index].readRangeContinuousMillimeters();
}

//Reading sensors less offset
int read_sensors(uint8_t index) {
 return read_sensors_pure(index) - dist_sensors_offsets[index];
}

// int read_sensors(uint8_t index) {
//   const uint8_t samples = 2;
//   long sum = 0;
//   for (uint8_t i = 0; i < samples; ++i) {
//     sum += read_sensors_pure(index);
//     // pequeno delay para dar tempo ao sensor de atualizar a medida
//     delayMicroseconds(10);
//   }
//   int average = sum / samples;
//   return average - dist_sensors_offsets[index];
// }

// Activate GY sensor
void tcaselect(uint8_t i) {
  if (i >= N_SENSORS) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
  delayMicroseconds(300);  
}

void begin_gy() {
  for (uint8_t i = 0; i < N_SENSORS; i++) {
    tcaselect(i);
    vTaskDelay(pdMS_TO_TICKS(5));  // tempo reduzido
    if (!sensors[i].init()) {
      Serial.print("Erro no sensor ");
      Serial.println(i);
    } else {
      sensors[i].setTimeout(500);
      sensors[i].startContinuous();
      Serial.print("Sensor iniciado no canal ");
      Serial.println(i);
    }
  }
}

// ------------------------------ Reflectance plate ---------------------------- // 

void readColorSensors()
{
  const uint8_t numReadings = 5;  // Número de amostras
  long sums[sensor_length] = {0}; // Vetor para acumular as leituras

  for (uint8_t n = 0; n < numReadings; n++)
  {
    for (uint8_t i = 0; i < sensor_length; i++)
    {
      sums[i] += analogRead(sensor_vector[i]);
    }
    vTaskDelay(pdMS_TO_TICKS(2)); // Pequeno delay para estabilizar cada leitura (2 ms)
  }

  for (uint8_t i = 0; i < sensor_length; i++)
  {
    sensor_values[i] = sums[i] / numReadings; // Calcula a média
  }
}

const char *getColor()
{
  readColorSensors();
  if (find_blue())return "blue";
  if (find_red())return "red";
  if (find_grey())return "grey";
  if (find_black())return "black";
  return "unknown";
}

bool find_red() { 
  for (uint8_t i = 0; i < 2; i++) sensor_values[i] = analogRead(sensor_vector[i]);
  return (sensor_values[0] > 170 && sensor_values[0] < 200 && sensor_values[3] > 45 && sensor_values[3] < 70) ? true : false;
}
bool find_blue() {
  for (uint8_t i = 0; i < 2; i++) sensor_values[i] = analogRead(sensor_vector[i]);
  return (sensor_values[0] > 150 && sensor_values[0] < 450 && sensor_values[3] > 50 && sensor_values[3] < 100 && abs(gyro.getInclinationAngle()) < 5) ? true : false; //
}
bool find_black() { // First parameter calibrated w black tape and second was calibrated w black sticker 
  for (uint8_t i = 0; i < 2; i++) sensor_values[i] = analogRead(sensor_vector[i]);
  return (sensor_values[0] > 0 && sensor_values[0] < 80 && sensor_values[3] > 0 && sensor_values[3] < 100 && abs(gyro.getInclinationAngle()) < 5) ? true : false;
}
bool find_grey() { // First parameter calibrated w gray tape and second was calibrated w gray sticker 
  for (uint8_t i = 0; i < 2; i++) sensor_values[i] = analogRead(sensor_vector[i]);
  return (sensor_values[0] > 950 && sensor_values[0] < 1010 && sensor_values[3] > 440 && sensor_values[3] < 500 || sensor_values[0] > 350 && sensor_values[0] < 400 && sensor_values[3] > 215 && sensor_values[3] < 230) ? true : false;
}

// ------------------------- Vector reorder --------------------------- //


void vectorReorder(int16_t* vector, int8_t index) {
  int vector2[4];
  for (uint8_t i = 4; i < 8; i++) {
    vector2[i - 4] = vector[(i - index) % 4];
  }
  for (uint8_t i = 0; i < 4; i++) vector[i] = vector2[i];
}

void beginLed() {
  long color = 0xFFFFFF; // Branco
  led.setPixelColor(6, color); // LED 7
  led.setPixelColor(7, color); // LED 7
  led.setPixelColor(8, color); // LED 8
  led.setPixelColor(9, color); // LED 8
  led.setPixelColor(10, color); // LED 8
  led.setPixelColor(11, color); // LED 8
  led.setPixelColor(12, color); // LED 8
  led.show();
}


// ------------------------- LED blink --------------------------- // 
void blink_led(uint8_t repeticoes, uint8_t typeColor, bool side) {
  long color;
  int8_t ledToBlink[2];

  // Define os LEDs a piscar com base no lado
  if (side) { // lado direito
    ledToBlink[0] = 4;
    ledToBlink[1] = 5;
  } else {     // lado esquerdo
    ledToBlink[0] = 0;
    ledToBlink[1] = 1;
  }
  // Define a cor correspondente
  if (typeColor == 0) color = 0x00FF00; // Verde 
  else if (typeColor == 1) color = 0xFFFF00; // Amarelo
  else if (typeColor == 2) color = 0xFF0000; // Vermelho
  else if (typeColor == 3) color = 0x0000FF; // Azul
  else if (typeColor == 4) color = 0xFFFFFF; // Branco
  else color = 0x000000; // Cor padrão (apagado) se tipo inválido

  // Piscar os LEDs
  for (uint8_t i = 0; i < repeticoes; i++) {
    if (digitalRead(BUTTON)) break;
    if(color == 0xFFFFFF || color == 0x0000FF){
    led.setPixelColor(0, color);
    led.setPixelColor(1, color);
    led.setPixelColor(2, color);
    led.setPixelColor(3, color);
    led.setPixelColor(4, color);
    led.setPixelColor(5, color);
    led.show();
    vTaskDelay(pdMS_TO_TICKS(500));
    led_clear();
    vTaskDelay(pdMS_TO_TICKS(500));
    }else{
    led.setPixelColor(ledToBlink[0], color);
    led.setPixelColor(ledToBlink[1], color);
    led.show();
    vTaskDelay(pdMS_TO_TICKS(500));
    led_clear();
    vTaskDelay(pdMS_TO_TICKS(500));
    }
  }
}

void led_clear() {
  for (uint8_t i = 0; i < 6; i++) led.setPixelColor(i, 0);
  led.show();
}

// ---------------------- Conversão dos Códigos ------------------------

int8_t convert_victim_code(uint8_t code) {
  // Green = 1 ; Yellow = 2 ; Red = 3 ; U = 4 ; S = 5 ; H = 6
  if (code == 1 || code == 4) return 0; // Verde ou U
  else if (code == 2 || code == 5) return 1; // Amarelo ou S
  else if (code == 3 || code == 6) return 2; // Vermelho ou H
  return NULL; // Valor inválido (ignorar)
}

// -------------------------- Release Kits -----------------------------

bool release_kits(uint8_t reps, bool sideFlag) {
  Point ponto_atual = robot.getActualPoint();
  uint16_t currentTile = robot.pointToIndex(ponto_atual);

  // Se já lançou kit nesse tile ou valor inválido, sai
  if (reps >= 3 || robot.victimNodes.contains(currentTile)) return false;

  uint8_t side = sideFlag ? KIT_RIGHT : KIT_LEFT; 

  // Pisca LED com a cor certa no lado certo
  blink_led(5, reps, sideFlag);

  for (uint8_t i = 0; i < reps; i++) {
    servo.write(side);
    vTaskDelay(pdMS_TO_TICKS(700));
    servo.write(KIT_CENTER);
    vTaskDelay(pdMS_TO_TICKS(700));
  }

  victimCounter++;
  robot.victimNodes.append(currentTile);
  return true;
}

