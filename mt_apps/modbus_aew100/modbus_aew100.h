#ifndef _MODBUS_AEW100_H
#define _MODBUS_AEW100_H

#include "esp_err.h"

#include "mt_mb.h"
#include "mt_mbport.h"

#include "cJSON.h"

// define =====================================================================

#define BUF_MAXLEN 64

// global func ================================================================

esp_err_t modbus_aew100_get_current_ABC(uint8_t Addr, double *a, double *b,
                                        double *c);
esp_err_t modbus_aew100_get_currentA(uint8_t addr, double *data);
esp_err_t modbus_aew100_get_currentB(uint8_t addr, double *data);
esp_err_t modbus_aew100_get_currentC(uint8_t addr, double *data);

esp_err_t modbus_aew100_get_votage_ABC(uint8_t addr, double *a, double *b,
                                       double *c);
esp_err_t modbus_aew100_get_votageA(uint8_t addr, double *data);
esp_err_t modbus_aew100_get_votageB(uint8_t addr, double *data);
esp_err_t modbus_aew100_get_votageC(uint8_t addr, double *data);

esp_err_t modbus_aew100_get_activePower_ABC(uint8_t addr, double *a, double *b,
                                            double *c);
esp_err_t modbus_aew100_get_activePowerA(uint8_t addr, double *data);
esp_err_t modbus_aew100_get_activePowerB(uint8_t addr, double *data);
esp_err_t modbus_aew100_get_activePowerC(uint8_t addr, double *data);

esp_err_t modbus_aew100_get_reactivePower_ABC(uint8_t addr, double *a,
                                              double *b, double *c);
esp_err_t modbus_aew100_get_reactivePowerA(uint8_t addr, double *data);
esp_err_t modbus_aew100_get_reactivePowerB(uint8_t addr, double *data);
esp_err_t modbus_aew100_get_reactivePowerC(uint8_t addr, double *data);

esp_err_t modbus_aew100_get_powerFactor_ABC(uint8_t addr, double *a, double *b,
                                            double *c);
esp_err_t modbus_aew100_get_powerFactorA(uint8_t addr, double *data);
esp_err_t modbus_aew100_get_powerFactorB(uint8_t addr, double *data);
esp_err_t modbus_aew100_get_powerFactorC(uint8_t addr, double *data);

esp_err_t modbus_aew100_get_quality_ABC(uint8_t addr, double *a, double *b,
                                        double *c);
esp_err_t modbus_aew100_get_qualityA(uint8_t addr, double *data);
esp_err_t modbus_aew100_get_qualityB(uint8_t addr, double *data);
esp_err_t modbus_aew100_get_qualityC(uint8_t addr, double *data);

esp_err_t modbus_aew100_get_temp_ABC(uint8_t addr, double *a, double *b,
                                     double *c);
esp_err_t modbus_aew100_get_tempA(uint8_t addr, double *data);
esp_err_t modbus_aew100_get_tempB(uint8_t addr, double *data);
esp_err_t modbus_aew100_get_tempC(uint8_t addr, double *data);

esp_err_t modbus_aew100_init(uint8_t port, int tx_pin, int rx_pin, int en_pin,
                             int device_num);

esp_err_t modbus_aew100_with_config_init(uint8_t port, int tx_pin, int rx_pin,
                                         int en_pin);

// json api func ==============================================================

cJSON *modbus_aew100_get_flow_data();

#endif
