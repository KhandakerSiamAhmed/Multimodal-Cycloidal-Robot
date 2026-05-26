#ifndef SERIAL_COMMANDS_H
#define SERIAL_COMMANDS_H

/**
 * @file serial_commands.h
 * @brief Interface Contract v1.0 — PC-to-Master Serial/TCP JSON command schema.
 * 
 * This file documents the JSON command format used by the PC software
 * (Vision Pipeline, Voice Integration, Serial Bridge) to communicate
 * with the ESP32-S3 Master node over Serial or Wi-Fi TCP/IP.
 * 
 * This is a DOCUMENTATION-ONLY header. The actual JSON parsing is
 * implemented in serial_bridge.cpp (firmware) and bridge.py (PC).
 * 
 * ===== COMMAND SCHEMAS =====
 * 
 * 1. MOVE_XYZ — Cartesian position command
 *    {"cmd":"MOVE_XYZ","x":120.5,"y":80.0,"z":45.0,"velocity":30.0,"seq":1001}
 * 
 * 2. MOVE_JOINTS — Direct joint angle command
 *    {"cmd":"MOVE_JOINTS","joints":[45.0,-30.0,60.0],"velocity":25.0,"seq":1002}
 * 
 * 3. GRIPPER — End effector control
 *    {"cmd":"GRIPPER","action":"OPEN","seq":1003}
 *    {"cmd":"GRIPPER","action":"CLOSE","seq":1004}
 * 
 * 4. SET_MODE — State machine transition
 *    {"cmd":"SET_MODE","mode":"VISION_SORT","seq":1005}
 *    Valid modes: IDLE, VISION_SORT, VOICE_SORT, TELEOP, TEACH_MODE, REPEAT_MODE
 * 
 * 5. QUERY_STATE — Request current arm state
 *    {"cmd":"QUERY_STATE","seq":1006}
 * 
 * 6. EMERGENCY_STOP — Immediate halt
 *    {"cmd":"ESTOP","seq":1007}
 * 
 * ===== RESPONSE SCHEMA =====
 * 
 * {"status":"OK","joints":[45.0,-30.0,60.0],"mode":"IDLE","errors":[],"seq":1001}
 * {"status":"ERROR","message":"IK_NO_SOLUTION","seq":1001}
 * 
 * ===== PROTOCOL NOTES =====
 * 
 * - All messages are newline-delimited JSON (\n terminated)
 * - Baud rate: 115200 (Serial) or port 8080 (TCP)
 * - seq field is echoed in response for request-response matching
 * - Maximum message length: 256 bytes
 */

// Command string constants for firmware-side parsing
#define CMD_MOVE_XYZ     "MOVE_XYZ"
#define CMD_MOVE_JOINTS  "MOVE_JOINTS"
#define CMD_GRIPPER      "GRIPPER"
#define CMD_SET_MODE     "SET_MODE"
#define CMD_QUERY_STATE  "QUERY_STATE"
#define CMD_ESTOP        "ESTOP"

// Response status constants
#define RSP_OK           "OK"
#define RSP_ERROR        "ERROR"

// Error message constants
#define ERR_IK_NO_SOLUTION    "IK_NO_SOLUTION"
#define ERR_JOINT_LIMIT       "JOINT_LIMIT"
#define ERR_INVALID_CMD       "INVALID_CMD"
#define ERR_NODE_OFFLINE      "NODE_OFFLINE"

#endif // SERIAL_COMMANDS_H
