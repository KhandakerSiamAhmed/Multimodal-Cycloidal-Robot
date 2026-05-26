#ifndef NODE_TO_MASTER_PAYLOAD_H
#define NODE_TO_MASTER_PAYLOAD_H

#include <stdint.h>

/**
 * @file node_to_master_payload.h
 * @brief Interface Contract v1.0 — Node (ESP32-C6) to Master (ESP32-S3) payload.
 * 
 * This struct defines the ESP-NOW packet sent from each actuator node back to the Master.
 * Both Master and Node firmware MUST use this identical struct definition.
 * 
 * VERSIONING: Any change to this struct requires incrementing CONTRACT_VERSION
 * and updating both Master_ESP32S3 and Node_ESP32C6 simultaneously.
 */

#define NODE_TO_MASTER_CONTRACT_VERSION 1

// Error state enumeration
typedef enum {
    NODE_OK            = 0,
    NODE_STALL         = 1,  // Motor stall detected (position not advancing)
    NODE_ENCODER_FAULT = 2,  // AS5600 I2C communication failure
    NODE_OVERTEMP      = 3,  // Driver overtemperature (if monitored)
    NODE_LIMIT_HIT     = 4   // Software or hardware limit reached
} NodeErrorState;

typedef struct __attribute__((packed)) {
    uint8_t  contract_version;   // Must match NODE_TO_MASTER_CONTRACT_VERSION
    uint8_t  joint_id;           // 0=Base(Yaw), 1=Shoulder(Pitch), 2=Elbow(Pitch)
    float    current_angle_deg;  // AS5600 absolute angle reading in degrees
    float    current_velocity;   // Estimated velocity in deg/s (from ESO)
    uint8_t  error_state;        // NodeErrorState enum value
    uint32_t sequence_id;        // Echoes last received Master sequence_id
    uint8_t  status_flags;       // Bit 0: ADRC_SETTLED, Bit 1: IN_MOTION
} NodeToMasterPayload;

// Status flag bit definitions
#define STATUS_FLAG_ADRC_SETTLED  (1 << 0)
#define STATUS_FLAG_IN_MOTION     (1 << 1)

#endif // NODE_TO_MASTER_PAYLOAD_H
