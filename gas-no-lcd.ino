#include <LiquidCrystal_I2C.h>

#define GAS_SENSOR_PIN A0 // Change this to the pin you've connected the gas sensor to

LiquidCrystal_I2C lcd(0x27, 16, 2);
const int MQ_PIN = A0;
float RO_CLEAN_AIR_FACTOR = 9.83;

float Ro = 10;
int RL_VALUE = 10;

int CALIBARAION_SAMPLE_TIMES = 50;
int CALIBRATION_SAMPLE_INTERVAL = 500;
int READ_SAMPLE_INTERVAL = 50;
int READ_SAMPLE_TIMES = 5;

float rs_ro = 0;

float SmokeCurve[3] = {2.3, 0.3979, -0.31};

#define GAS_SMOKE 2

// LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()
{
    Serial.begin(9600);
    pinMode(GAS_SENSOR_PIN, INPUT);
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);

    Serial.println("Calibrating gas sensor in 10 seconds...");

    Serial.println("Calibrating");
    lcd.print("Calibrating...");
    Ro = MQCalibration(MQ_PIN);

    Serial.println("Calibration is done...\n");
    lcd.print("done!");
    // Serial.setCursor(0, 1);
    Serial.println("Ro=");
    lcd.print("Ro= ");
    Serial.print(Ro);
    lcd.print(Ro);
    Serial.print("kohm");
    lcd.print("kohm");
    delay(3000);

    // Print the pin number and mode
    // Serial.print("GAS_SENSOR_PIN: ");
    // Serial.println(GAS_SENSOR_PIN);
    // Serial.println("Pin mode: INPUT");
}

void loop()
{
    long iPPM_Smoke = 0;
    iPPM_Smoke = MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_SMOKE);
    rs_ro = MQRead(MQ_PIN) / Ro;
    // Serial.println(iPPM_Smoke);
    // lcd.clear();
    // lcd.setCursor(0, 0);
    // lcd.print("Concentration of gas ");

    // lcd.setCursor(0,1);
    // lcd.print("Smoke: ");
    Serial.println("Concentration of gas");
    Serial.print("Smoke: ");
    Serial.print(iPPM_Smoke);
    lcd.print(iPPM_Smoke);
    lcd.print(" ppm");

    delay(200);
}

float MQResistanceCalculation(int raw_adc)
{
    return (((float)RL_VALUE * (1023 - raw_adc) / raw_adc));
}

float MQCalibration(int mq_pin)
{
    int i;
    float val = 0;

    for (i = 0; i < CALIBARAION_SAMPLE_TIMES; i++)
    {
        val += MQResistanceCalculation(analogRead(mq_pin));
        delay(CALIBRATION_SAMPLE_INTERVAL);
    }
    val = val / CALIBARAION_SAMPLE_TIMES;
    val = val / RO_CLEAN_AIR_FACTOR;
    return val;
}

float MQRead(int mq_pin)
{
    int i;
    float rs = 0;

    for (i = 0; i < READ_SAMPLE_TIMES; i++)
    {
        rs += MQResistanceCalculation(analogRead(mq_pin));
        delay(READ_SAMPLE_INTERVAL);
    }

    rs = rs / READ_SAMPLE_TIMES;

    return rs;
}

long MQGetGasPercentage(float rs_ro_ratio, int gas_id)
{
    if (gas_id == GAS_SMOKE)
    {
        return MQGetPercentage(rs_ro_ratio, SmokeCurve);
    }

    return 0;
}

long MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
    return (pow(10, (((log(rs_ro_ratio) - pcurve[1]) / pcurve[2]) + pcurve[0])));
}

// Power function
int power(int base, int exponent)
{
    int result = 1;
    for (int i = 0; i < exponent; i++)
    {
        result *= base;
    }
    return result;
}