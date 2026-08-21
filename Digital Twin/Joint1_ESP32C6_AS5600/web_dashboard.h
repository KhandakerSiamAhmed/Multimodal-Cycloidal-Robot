/**
 * @file web_dashboard.h
 * @brief Embedded Web UI Dashboard for Joint 1 (ESP32-C6 + AS5600)
 */

#ifndef WEB_DASHBOARD_H
#define WEB_DASHBOARD_H

#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Joint 1 Digital Twin - AS5600 + ESP32-C6</title>
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link href="https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;600;700&family=JetBrains+Mono:wght@400;600&display=swap" rel="stylesheet">
  <style>
    :root {
      --bg-gradient: radial-gradient(circle at 50% 0%, #171d32 0%, #0a0d16 100%);
      --card-bg: rgba(22, 28, 48, 0.65);
      --card-border: rgba(255, 255, 255, 0.08);
      --accent-cyan: #00f0ff;
      --accent-blue: #3b82f6;
      --accent-green: #10b981;
      --accent-amber: #f59e0b;
      --accent-rose: #f43f5e;
      --text-main: #f8fafc;
      --text-muted: #94a3b8;
    }

    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
    }

    body {
      font-family: 'Outfit', -apple-system, sans-serif;
      background: var(--bg-gradient);
      color: var(--text-main);
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 24px 16px;
      overflow-x: hidden;
    }

    header {
      text-align: center;
      margin-bottom: 28px;
    }

    .badge {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      padding: 6px 14px;
      border-radius: 9999px;
      background: rgba(0, 240, 255, 0.1);
      border: 1px solid rgba(0, 240, 255, 0.25);
      color: var(--accent-cyan);
      font-size: 0.82rem;
      font-weight: 600;
      letter-spacing: 0.05em;
      text-transform: uppercase;
      margin-bottom: 12px;
    }

    .badge-dot {
      width: 8px;
      height: 8px;
      border-radius: 50%;
      background: var(--accent-cyan);
      box-shadow: 0 0 10px var(--accent-cyan);
      animation: pulse 2s infinite;
    }

    @keyframes pulse {
      0%, 100% { opacity: 1; transform: scale(1); }
      50% { opacity: 0.4; transform: scale(0.85); }
    }

    h1 {
      font-size: 2.2rem;
      font-weight: 700;
      letter-spacing: -0.02em;
      background: linear-gradient(135deg, #ffffff 40%, #94a3b8 100%);
      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
    }

    p.subtitle {
      color: var(--text-muted);
      font-size: 0.95rem;
      margin-top: 4px;
    }

    .container {
      width: 100%;
      max-width: 900px;
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 20px;
    }

    @media (max-width: 768px) {
      .container { grid-template-columns: 1fr; }
    }

    .card {
      background: var(--card-bg);
      backdrop-filter: blur(16px);
      -webkit-backdrop-filter: blur(16px);
      border: 1px solid var(--card-border);
      border-radius: 20px;
      padding: 24px;
      box-shadow: 0 20px 40px -15px rgba(0, 0, 0, 0.5);
      display: flex;
      flex-direction: column;
      align-items: center;
      position: relative;
    }

    /* Dial & Visual Gauge */
    .gauge-wrapper {
      position: relative;
      width: 240px;
      height: 240px;
      margin: 16px 0;
      display: flex;
      align-items: center;
      justify-content: center;
    }

    .gauge-svg {
      width: 100%;
      height: 100%;
      transform: rotate(-90deg);
    }

    .gauge-bg {
      fill: none;
      stroke: rgba(255, 255, 255, 0.05);
      stroke-width: 14;
    }

    .gauge-progress {
      fill: none;
      stroke: url(#cyanGrad);
      stroke-width: 14;
      stroke-linecap: round;
      stroke-dasharray: 691;
      stroke-dashoffset: 691;
      transition: stroke-dashoffset 0.08s linear;
    }

    .gauge-needle {
      position: absolute;
      width: 100%;
      height: 100%;
      top: 0;
      left: 0;
      pointer-events: none;
      transition: transform 0.08s linear;
    }

    .needle-point {
      position: absolute;
      top: 14px;
      left: 50%;
      transform: translateX(-50%);
      width: 4px;
      height: 24px;
      border-radius: 2px;
      background: var(--accent-cyan);
      box-shadow: 0 0 12px var(--accent-cyan);
    }

    .gauge-center-val {
      position: absolute;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
    }

    .angle-large {
      font-family: 'JetBrains Mono', monospace;
      font-size: 2.8rem;
      font-weight: 700;
      color: #ffffff;
      line-height: 1;
    }

    .unit {
      color: var(--accent-cyan);
      font-size: 1.1rem;
      margin-left: 2px;
    }

    .angle-label {
      color: var(--text-muted);
      font-size: 0.8rem;
      text-transform: uppercase;
      letter-spacing: 0.1em;
      margin-top: 6px;
    }

    /* Diagnostics & Stats Grid */
    .stats-grid {
      width: 100%;
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 12px;
      margin-top: 16px;
    }

    .stat-item {
      background: rgba(0, 0, 0, 0.25);
      border: 1px solid rgba(255, 255, 255, 0.04);
      border-radius: 12px;
      padding: 12px 14px;
    }

    .stat-title {
      font-size: 0.75rem;
      color: var(--text-muted);
      text-transform: uppercase;
      letter-spacing: 0.05em;
    }

    .stat-val {
      font-family: 'JetBrains Mono', monospace;
      font-size: 1.1rem;
      font-weight: 600;
      color: var(--text-main);
      margin-top: 4px;
    }

    /* Status Pill */
    .status-pill {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      padding: 4px 10px;
      border-radius: 6px;
      font-size: 0.8rem;
      font-weight: 600;
      margin-top: 6px;
    }

    .status-ok { background: rgba(16, 185, 129, 0.15); color: var(--accent-green); border: 1px solid rgba(16, 185, 129, 0.3); }
    .status-warn { background: rgba(245, 158, 11, 0.15); color: var(--accent-amber); border: 1px solid rgba(245, 158, 11, 0.3); }
    .status-err { background: rgba(244, 63, 94, 0.15); color: var(--accent-rose); border: 1px solid rgba(244, 63, 94, 0.3); }

    /* API Endpoint Box */
    .api-card {
      width: 100%;
      max-width: 900px;
      margin-top: 20px;
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      border-radius: 16px;
      padding: 18px 24px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 16px;
      flex-wrap: wrap;
    }

    .code-snippet {
      font-family: 'JetBrains Mono', monospace;
      background: rgba(0, 0, 0, 0.4);
      padding: 8px 14px;
      border-radius: 8px;
      border: 1px solid rgba(255, 255, 255, 0.08);
      color: var(--accent-cyan);
      font-size: 0.9rem;
    }

    .btn {
      background: linear-gradient(135deg, #00f0ff, #3b82f6);
      border: none;
      color: #050811;
      font-family: 'Outfit', sans-serif;
      font-weight: 700;
      font-size: 0.9rem;
      padding: 10px 20px;
      border-radius: 10px;
      cursor: pointer;
      transition: all 0.2s ease;
      box-shadow: 0 4px 15px rgba(0, 240, 255, 0.3);
    }

    .btn:hover {
      transform: translateY(-2px);
      box-shadow: 0 6px 20px rgba(0, 240, 255, 0.45);
    }

    .btn:active {
      transform: translateY(0);
    }

    .btn-secondary {
      background: rgba(255, 255, 255, 0.08);
      color: var(--text-main);
      border: 1px solid rgba(255, 255, 255, 0.12);
      box-shadow: none;
    }

    .btn-secondary:hover {
      background: rgba(255, 255, 255, 0.15);
      box-shadow: none;
    }
  </style>
</head>
<body>

  <header>
    <div class="badge">
      <div class="badge-dot"></div>
      Digital Twin Node &bull; Joint 1
    </div>
    <h1>NEMA 17 Angular Telemetry</h1>
    <p class="subtitle">ESP32-C6 Super Mini &bull; AS5600 12-Bit Magnetic Encoder</p>
  </header>

  <div class="container">
    <!-- Card 1: Angle Gauge -->
    <div class="card">
      <h2 style="font-size: 1.1rem; color: var(--text-muted); font-weight: 600;">CURRENT JOINT 1 POSITION</h2>
      <div class="gauge-wrapper">
        <svg class="gauge-svg" viewBox="0 0 240 240">
          <defs>
            <linearGradient id="cyanGrad" x1="0%" y1="0%" x2="100%" y2="100%">
              <stop offset="0%" stop-color="#00f0ff" />
              <stop offset="100%" stop-color="#3b82f6" />
            </linearGradient>
          </defs>
          <circle class="gauge-bg" cx="120" cy="120" r="110" />
          <circle id="gaugeArc" class="gauge-progress" cx="120" cy="120" r="110" />
        </svg>
        <div id="needle" class="gauge-needle">
          <div class="needle-point"></div>
        </div>
        <div class="gauge-center-val">
          <span class="angle-large"><span id="angleVal">0.0</span><span class="unit">&deg;</span></span>
          <span class="angle-label">Instantaneous</span>
        </div>
      </div>
      <div style="display:flex; gap: 10px; margin-top: 8px;">
        <button class="btn btn-secondary" onclick="setZeroPosition()">Zero Offset</button>
      </div>
    </div>

    <!-- Card 2: Diagnostics & Raw Telemetry -->
    <div class="card" style="align-items: stretch;">
      <h2 style="font-size: 1.1rem; color: var(--text-muted); font-weight: 600; text-align: center;">SENSOR DIAGNOSTICS</h2>
      
      <div class="stats-grid">
        <div class="stat-item">
          <div class="stat-title">JSON Output</div>
          <div class="stat-val" id="jsonDisplay" style="color: var(--accent-cyan); font-size: 0.95rem;">{"joint1": 0.0}</div>
        </div>
        <div class="stat-item">
          <div class="stat-title">Multi-Turn Angle</div>
          <div class="stat-val" id="cumAngleVal">0.0&deg;</div>
        </div>
        <div class="stat-item">
          <div class="stat-title">Raw 12-Bit Ticks</div>
          <div class="stat-val" id="rawVal">0 / 4095</div>
        </div>
        <div class="stat-item">
          <div class="stat-title">Auto Gain (AGC)</div>
          <div class="stat-val" id="agcVal">--</div>
        </div>
      </div>

      <div style="margin-top: 16px; background: rgba(0,0,0,0.25); padding: 14px; border-radius: 12px; border: 1px solid rgba(255,255,255,0.04);">
        <div class="stat-title">Magnet Alignment Health</div>
        <div id="magnetStatusBadge" class="status-pill status-ok">Checking...</div>
        <div style="font-size: 0.78rem; color: var(--text-muted); margin-top: 6px;" id="magnetStatusDesc">
          Ensure magnet is placed 0.5mm - 1.5mm above AS5600 IC.
        </div>
      </div>

      <div class="stats-grid" style="margin-top: 12px;">
        <div class="stat-item">
          <div class="stat-title">WiFi RSSI</div>
          <div class="stat-val" id="rssiVal">-- dBm</div>
        </div>
        <div class="stat-item">
          <div class="stat-title">Refresh Rate</div>
          <div class="stat-val" style="color: var(--accent-green);">~20 Hz (50ms)</div>
        </div>
      </div>
    </div>
  </div>

  <!-- API Quick Access Bar -->
  <div class="api-card">
    <div>
      <div style="font-weight: 600; font-size: 0.95rem;">Direct JSON Endpoint:</div>
      <div style="color: var(--text-muted); font-size: 0.82rem;">Fetch real-time data from Python, ROS, or Web twin</div>
    </div>
    <div class="code-snippet">GET /angle &rarr; {"joint1": <span id="apiAnglePreview">0.00</span>}</div>
    <a href="/angle" target="_blank" style="text-decoration: none;">
      <button class="btn">Open JSON Feed</button>
    </a>
  </div>

  <script>
    const CIRCUMFERENCE = 2 * Math.PI * 110; // ~691.15
    const gaugeArc = document.getElementById('gaugeArc');
    const needle = document.getElementById('needle');
    const angleVal = document.getElementById('angleVal');
    const cumAngleVal = document.getElementById('cumAngleVal');
    const rawVal = document.getElementById('rawVal');
    const agcVal = document.getElementById('agcVal');
    const jsonDisplay = document.getElementById('jsonDisplay');
    const apiAnglePreview = document.getElementById('apiAnglePreview');
    const magnetBadge = document.getElementById('magnetStatusBadge');
    const magnetDesc = document.getElementById('magnetStatusDesc');
    const rssiVal = document.getElementById('rssiVal');

    async function fetchTelemetry() {
      try {
        const res = await fetch('/status');
        if (!res.ok) return;
        const data = await res.json();

        const angle = data.joint1;
        angleVal.textContent = angle.toFixed(2);
        apiAnglePreview.textContent = angle.toFixed(2);
        cumAngleVal.textContent = data.cumulative.toFixed(1) + '°';
        rawVal.textContent = data.raw + ' / 4095';
        agcVal.textContent = data.agc;
        rssiVal.textContent = data.rssi + ' dBm';
        jsonDisplay.textContent = JSON.stringify({ joint1: Number(angle.toFixed(2)) });

        // Update circular gauge
        const offset = CIRCUMFERENCE - (angle / 360) * CIRCUMFERENCE;
        gaugeArc.style.strokeDashoffset = offset;
        needle.style.transform = `rotate(${angle}deg)`;

        // Magnet status
        if (!data.magnet_detected) {
          magnetBadge.className = 'status-pill status-err';
          magnetBadge.textContent = '❌ No Magnet Detected';
          magnetDesc.textContent = 'Magnet is missing or too far from the sensor chip.';
        } else if (data.magnet_weak) {
          magnetBadge.className = 'status-pill status-warn';
          magnetBadge.textContent = '⚠️ Magnet Too Weak';
          magnetDesc.textContent = 'Move magnet slightly closer to the AS5600 IC.';
        } else if (data.magnet_strong) {
          magnetBadge.className = 'status-pill status-warn';
          magnetBadge.textContent = '⚠️ Magnet Too Strong';
          magnetDesc.textContent = 'Move magnet slightly farther from the AS5600 IC.';
        } else {
          magnetBadge.className = 'status-pill status-ok';
          magnetBadge.textContent = '✅ Optimal Alignment';
          magnetDesc.textContent = 'Magnetic field is within ideal operating window.';
        }
      } catch (err) {
        console.error('Polling error:', err);
      }
    }

    async function setZeroPosition() {
      try {
        await fetch('/api/zero', { method: 'POST' });
        fetchTelemetry();
      } catch(e) {
        console.error(e);
      }
    }

    // High frequency 50ms polling loop
    setInterval(fetchTelemetry, 50);
    fetchTelemetry();
  </script>
</body>
</html>
)rawliteral";

#endif // WEB_DASHBOARD_H
