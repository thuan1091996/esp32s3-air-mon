
#ifndef __ACP1_DEFINE_H__
#define __ACP1_DEFINE_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define APC1_LOG_ENABLE (1)     // 1: Enable debug logging

typedef enum {
    APC1_I2C_Address = 0x12,    // Device’s I2C address (7 bit)
    APC1_I2C_Frequency = 100000 // I2C standard mode (100kHz)
} apc1_i2c_define_t;

typedef enum {
    Request_Register  = 0x40,
    Response_Register = 0x47
} apc1_register_t;

typedef enum {
    Command_Set_Mode   = 0xE4,
    Command_Read_Infor = 0xE9
} apc1_command_t;

typedef enum {
    Mode_None        = 0x00,
    Mode_Idle        = 0x00,
    Mode_Measurement = 0x01,
    Mode_Reset       = 0x0F
} apc1_mode_t;

typedef enum {
    Frame_Header_H = 0x42,
    Frame_Header_L = 0x4D
} apc1_header_t;

typedef enum {
    Read_Infor_Response_Lenght           = 23,
    Set_Idle_Mode_Response_Lenght        = 8,
    Set_Measurement_Mode_Response_Lenght = 64
} apc1_response_length_t;

typedef enum {
    Frame_Header     = 0x00,
    Frame_Length     = 0x02,
    PM_1_0           = 0x04,
    PM_2_5           = 0x06,
    PM_10            = 0x08,
    PMInAir_1_0      = 0x0A,
    PMInAir_2_5      = 0x0C,
    PMInAir_10       = 0x0E,
    NoParticles_0_3  = 0x10,
    NoParticles_0_5  = 0x12,
    NoParticles_1_0  = 0x14,
    NoParticles_2_5  = 0x16,
    NoParticles_5_0  = 0x18,
    NoParticles_10   = 0x1A,
    TVOC             = 0x1C,
    ECO2             = 0x1E,
    T_comp           = 0x22,
    RH_comp          = 0x24,
    T_raw            = 0x26,
    RH_raw           = 0x28,
    RS0              = 0x2A,
    RS1              = 0x2E,
    RS2              = 0x32,
    RS3              = 0x36,
    AQI              = 0x3A,
    FW_Version       = 0x3C,
    ErrorCode        = 0x3D
} apc1_measure_data_index_t;

typedef enum {
    Index_Name          = 0x04,
    Index_Type          = 0x09,
    Index_Serial_Number = 0x0A,
    Index_FW_Version    = 0x13,
    Index_Checksum      = 0x15
} apc1_sensor_infor_index_t;

typedef enum {
    LEN_Name          = 5,
    LEN_Type          = 1,
    LEN_Serial_Number = 8,
    LEN_FW_Version    = 2,
    LEN_Checksum      = 2
} apc1_sensor_infor_length_t;

typedef enum {
    Temperature_Humidity_Scale = 10
} apc1_measure_data_scale_t;

typedef struct {
    uint16_t PM_1_0;            // PM1.0 mass concentration
    uint16_t PM_2_5;            // PM2.5 mass concentration
    uint16_t PM_10;             // PM10  mass concentration
    uint16_t PMInAir_1_0;       // PM1.0 mass concentration in atmospheric environment
    uint16_t PMInAir_2_5;       // PM2.5 mass concentration in atmospheric environment
    uint16_t PMInAir_10;        // PM10  mass concentration in atmospheric environment
    uint16_t NoParticles_0_3;   // Number of particles with diameter > 0.3μm in 0.1L of air
    uint16_t NoParticles_0_5;   // Number of particles with diameter > 0.5μm in 0.1L of air
    uint16_t NoParticles_1_0;   // Number of particles with diameter > 1.0μm in 0.1L of air
    uint16_t NoParticles_2_5;   // Number of particles with diameter > 2.5μm in 0.1L of air
    uint16_t NoParticles_5_0;   // Number of particles with diameter > 5.0μm in 0.1L of air
    uint16_t NoParticles_10;    // Number of particles with diameter >  10μm in 0.1L of air
    uint16_t TVOC;              // TVOC output
    uint16_t ECO2;              // Output in ppm CO2 equivalents
    uint16_t T_comp;            // Compensated temperature (see datasheet)
    uint16_t RH_comp;           // Compensated humidity (see datasheet)
    uint16_t T_raw;             // Uncompensated temperature
    uint16_t RH_raw;            // Uncompensated humidity
    uint32_t RS0;               // Gas sensor 0 raw resistance value
    uint32_t RS1;               // Gas sensor 1 raw resistance value
    uint32_t RS2;               // Gas sensor 2 raw resistance value
    uint32_t RS3;               // Gas sensor 3 raw resistance value
    uint8_t AQI;                // Air Quality Index according to UBA Classification of TVOC value
    uint8_t FW_Version;         // Firmware version
    uint8_t ErrorCode;          // Error codes (see datasheet)
} apc1_measure_data_t;

typedef struct {
    char Name[6];
    char Type;
    uint8_t Serial_Number[8];
    uint8_t FW_Version[2];
} apc1_sensor_infor_t;

typedef struct {
    uint8_t Frame_Header_H;
    uint8_t Frame_Header_L;
    uint8_t Command;
    uint8_t Mode_H;
    uint8_t Mode_L;
    uint8_t Checksum_H;
    uint8_t Checksum_L;
} apc1_command_request_t;

#ifdef __cplusplus
}
#endif

#endif /**< __ACP1_DEFINE_H__ */