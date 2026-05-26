#ifndef MASTER_TO_NODE_PAYLOAD_H
#define MASTER_TO_NODE_PAYLOAD_H

#include <stdint.h>

/**
 * @file master_to_node_payload.h
 * @brief Interface Contract v1.0 — Master (ESP32-S3) to Node (ESP32-C6) payload.
 * 
 * This struct defines the ESP-NOW packet sent from the Master to each actuator node.
 * Both Master and Node firmware MUST use this identical struct definition.
 * 
 * VERSIONING: Any change to this struct requires incrementing CONTRACT_VERSION
 * and updating both Master_ESP32S3 and Node_ESP32C6 simultaneously.
 */

#define MASTER_TO_NODE_CONTRACT_VERSION 1

typedef struct __attribute__((packed)) {
    uint8_t  contract_version;   // Must match MASTER_TO_NODE_CONTRACT_VERSION
    uint8_t  joint_id;           // 0=Base(Yaw), 1=Shoulder(Pitch), 2=Elbow(Pitch)
    float    target_angle_deg;   // Target position in degrees
    float    velocity_limit;     // Maximum velocity in deg/s
    float    acceleration;       // Maximum acceleration in deg/s²
    uint32_t sequence_id;        // Monotonically increasing, for ordering & loss detection
    uint8_t  command_flags;      // Bit 0: EMERGENCY_STOP, Bit 1: HOLD_POSITION
} MasterToNodePayload;

// Command flag bit definitions
#define CMD_FLAG_EMERGENCY_STOP  (1 << 0)
#define CMD_FLAG_HOLD_POSITION   (1 << 1)

#endif // MASTER_TO_NODE_PAYLOAD_H
