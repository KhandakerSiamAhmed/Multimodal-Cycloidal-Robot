import re

def cleanup():
    with open('digital_twin.html', 'r', encoding='utf-8') as f:
        text = f.read()

    replacements = [
        # 1. Header titles
        ('<span class="header-title">SCADA HMI - J1</span>', '<span class="header-title">Digital Twin Interface</span>'),
        ('<span class="header-sub">AXIS 1 (AZIMUTH) - SYSTEM NOMINAL</span>', '<span class="header-sub">Joint 1 Telemetry</span>'),
        
        # 2. Tooltips
        (' title="USB Serial — works offline without Wi-Fi"', ''),
        (' title="Wi-Fi HTTP polling — ESP32 must be on same network"', ''),
        (' title="Capture current position as 0° reference"', ''),
        (' title="Remove relative offset, show raw AS5600 angle"', ''),
        
        # 3. Model Banner
        (r'<div id="model-banner">.*?</div>', '<div id="model-banner" style="display:none;"></div>'),
        
        # 4. Serial Warning
        (r'<div id="serial-warn">\s*⚠️ Web Serial API not available in this browser or context.\s*Use <strong style="margin:0 4px;">serve.bat</strong> to open via localhost, then use Chrome or Edge.\s*</div>',
         '<div id="serial-warn">⚠️ Web Serial API unavailable.</div>'),
        
        # 5. Mesh Inspector and Shaft Hint
        (r'<span style="font-size:10px;color:var\(--text-lo\);" id="shaft-hint">.*?</span>', '<span style="display:none;" id="shaft-hint"></span>'),
        (r'<div style="margin-top:12px;">\s*<div class="section-title" style="margin-bottom:8px;">Mesh Inspector</div>.*?<div id="mesh-inspector-btns" style="display:flex;flex-wrap:wrap;gap:5px;">\s*<span style="font-size:10px;color:var\(--text-lo\);">Load the GLB to see meshes…</span>\s*</div>\s*</div>',
         '<div id="mesh-inspector-btns" style="display:none;"></div>'),
         
        # 6. Footer details
        (r'<div>ESP32-C6 Super Mini &middot; Wi-Fi 6</div>\s*<div>AS5600 &middot; 12-bit &middot; 0.087&deg;/step</div>\s*<div>Online poll: <span style="font-family:var\(--font-mono\);">50ms</span></div>',
         '<div>System Live</div>'),
         
        # 7. Jog Panel hints
        ('<span style="color:var(--text-lo);">range: 0 → 360°</span>', '')
    ]

    for old, new in replacements:
        if old.startswith('<') or old.startswith(' '):
            text = text.replace(old, new)
        else:
            text = re.sub(old, new, text, flags=re.DOTALL)

    with open('digital_twin.html', 'w', encoding='utf-8') as f:
        f.write(text)
        
    print("Cleanup successful.")

if __name__ == '__main__':
    cleanup()
