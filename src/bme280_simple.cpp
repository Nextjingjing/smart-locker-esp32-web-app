#include "bme280_simple.h"

// ตัวแปรเก็บค่า Calibration จากโรงงาน
static uint16_t dig_T1; static int16_t dig_T2, dig_T3;
static uint16_t dig_P1; static int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
static uint8_t  dig_H1; static int16_t dig_H2; static uint8_t dig_H3; static int16_t dig_H4, dig_H5; static int8_t dig_H6;
static int32_t t_fine;

// สูตรคำนวณชดเชยค่า (Compensate) ตาม Datasheet
int32_t bme280_comp_T(int32_t adc_T) {
    int32_t v1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    int32_t v2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    t_fine = v1 + v2;
    return (t_fine * 5 + 128) >> 8;
}

uint32_t bme280_comp_P(int32_t adc_P) {
    int64_t v1, v2, p;
    v1 = ((int64_t)t_fine) - 128000;
    v2 = v1 * v1 * (int64_t)dig_P6 + ((v1 * (int64_t)dig_P5) << 17) + (((int64_t)dig_P4) << 35);
    v1 = ((v1 * v1 * (int64_t)dig_P3) >> 8) + ((v1 * (int64_t)dig_P2) << 12);
    v1 = (((((int64_t)1) << 47) + v1)) * ((int64_t)dig_P1) >> 33;
    if (v1 == 0) return 0;
    p = 1048576 - adc_P;
    p = (((p << 31) - v2) * 3125) / v1;
    v1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    v2 = (((int64_t)dig_P8) * p) >> 19;
    return (uint32_t)(((p + v1 + v2) >> 8) + (((int64_t)dig_P7) << 4));
}

uint32_t bme280_comp_H(int32_t adc_H) {
    int32_t v = (t_fine - ((int32_t)76800));
    v = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v)) + (int32_t)16384) >> 15) * (((((((v * (int32_t)dig_H6) >> 10) * (((v * (int32_t)dig_H3) >> 11) + (int32_t)32768)) >> 10) + (int32_t)2097152) * (int32_t)dig_H2 + 8192) >> 14));
    v = (v - (((((v >> 15) * (v >> 15)) >> 7) * (int32_t)dig_H1) >> 4));
    v = (v < 0 ? 0 : (v > 419430400 ? 419430400 : v));
    return (uint32_t)(v >> 12);
}

bool bme280_begin(int sda, int scl) {
    Wire.begin(sda, scl);
    uint8_t id; bme280_read_regs(0xD0, &id, 1);
    if (id != 0x60) return false;

    // อ่านค่า Calibration
    uint8_t b[24]; bme280_read_regs(0x88, b, 24);
    dig_T1 = b[1]<<8|b[0]; dig_T2 = b[3]<<8|b[2]; dig_T3 = b[5]<<8|b[4];
    dig_P1 = b[7]<<8|b[6]; dig_P2 = b[9]<<8|b[8]; dig_P3 = b[11]<<8|b[10];
    dig_P4 = b[13]<<8|b[12]; dig_P5 = b[15]<<8|b[14]; dig_P6 = b[17]<<8|b[16];
    dig_P7 = b[19]<<8|b[18]; dig_P8 = b[21]<<8|b[20]; dig_P9 = b[23]<<8|b[22];
    bme280_read_regs(0xA1, &dig_H1, 1);
    uint8_t h[7]; bme280_read_regs(0xE1, h, 7);
    dig_H2 = h[1]<<8|h[0]; dig_H3 = h[2];
    dig_H4 = (h[3] << 4) | (h[4] & 0x0F);
    dig_H5 = (h[5] << 4) | (h[4] >> 4);
    dig_H6 = (int8_t)h[6];

    // Config: Normal Mode, Oversampling x1
    bme280_write_reg(0xF2, 0x01);
    bme280_write_reg(0xF4, 0x27);
    return true;
}

void bme280_read(float &temp, float &hum, float &pres) {
    uint8_t d[8]; bme280_read_regs(0xF7, d, 8);
    int32_t p_raw = (int32_t)d[0]<<12 | (int32_t)d[1]<<4 | d[2]>>4;
    int32_t t_raw = (int32_t)d[3]<<12 | (int32_t)d[4]<<4 | d[5]>>4;
    int32_t h_raw = (int32_t)d[6]<<8  | d[7];

    temp = bme280_comp_T(t_raw) / 100.0;
    pres = bme280_comp_P(p_raw) / 256.0 / 100.0; // hPa
    hum  = bme280_comp_H(h_raw) / 1024.0;
}

void bme280_write_reg(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(reg); Wire.write(val);
    Wire.endTransmission();
}

void bme280_read_regs(uint8_t reg, uint8_t *buffer, uint8_t len) {
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(reg); Wire.endTransmission();
    Wire.requestFrom(BME280_ADDR, len);
    for (uint8_t i=0; i<len; i++) buffer[i] = Wire.read();
}