#ifndef _MODBUS_AEW100_H
#define _MODBUS_AEW100_H

#include "mt_mb.h"
#include "mt_mbport.h"

#include "mt_proto_aew100.pb-c.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

#define BUF_MAXLEN 64

esp_err_t mt_aew100_get_current_ABC(UCHAR Addr, double *a, double *b,
                                    double *c);
esp_err_t mt_aew100_get_currentA(UCHAR addr, double *data);
esp_err_t mt_aew100_get_currentB(UCHAR addr, double *data);
esp_err_t mt_aew100_get_currentC(UCHAR addr, double *data);

esp_err_t mt_aew100_get_votage_ABC(UCHAR addr, double *a, double *b, double *c);
esp_err_t mt_aew100_get_votageA(UCHAR addr, double *data);
esp_err_t mt_aew100_get_votageB(UCHAR addr, double *data);
esp_err_t mt_aew100_get_votageC(UCHAR addr, double *data);

esp_err_t mt_aew100_get_activePower_ABC(UCHAR addr, double *a, double *b,
                                        double *c);
esp_err_t mt_aew100_get_activePowerA(UCHAR addr, double *data);
esp_err_t mt_aew100_get_activePowerB(UCHAR addr, double *data);
esp_err_t mt_aew100_get_activePowerC(UCHAR addr, double *data);

esp_err_t mt_aew100_get_reactivePower_ABC(UCHAR addr, double *a, double *b,
                                          double *c);
esp_err_t mt_aew100_get_reactivePowerA(UCHAR addr, double *data);
esp_err_t mt_aew100_get_reactivePowerB(UCHAR addr, double *data);
esp_err_t mt_aew100_get_reactivePowerC(UCHAR addr, double *data);

esp_err_t mt_aew100_get_powerFactor_ABC(UCHAR addr, double *a, double *b,
                                        double *c);
esp_err_t mt_aew100_get_powerFactorA(UCHAR addr, double *data);
esp_err_t mt_aew100_get_powerFactorB(UCHAR addr, double *data);
esp_err_t mt_aew100_get_powerFactorC(UCHAR addr, double *data);

esp_err_t mt_aew100_get_quality_ABC(UCHAR addr, double *a, double *b,
                                    double *c);
esp_err_t mt_aew100_get_qualityA(UCHAR addr, double *data);
esp_err_t mt_aew100_get_qualityB(UCHAR addr, double *data);
esp_err_t mt_aew100_get_qualityC(UCHAR addr, double *data);

esp_err_t mt_aew100_get_temp_ABC(UCHAR addr, double *a, double *b, double *c);
esp_err_t mt_aew100_get_tempA(UCHAR addr, double *data);
esp_err_t mt_aew100_get_tempB(UCHAR addr, double *data);
esp_err_t mt_aew100_get_tempC(UCHAR addr, double *data);

// full data
esp_err_t mt_aew100_get_data(UCHAR addr, MtAew100__Data *data);

// simple data: current votage temp (activepower powefactor){compute not real}
esp_err_t mt_aew100_get_data2(UCHAR addr, MtAew100__Data *data);

esp_err_t mt_aew100_init(uint8_t port, int tx_pin, int rx_pin, int en_pin);

#endif
