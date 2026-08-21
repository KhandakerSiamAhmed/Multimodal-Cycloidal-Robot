import re

def refactor():
    with open('digital_twin.html', 'r', encoding='utf-8') as f:
        text = f.read()

    # 1. Colors & Variables
    replacements = {
        r'--bg-deep:\s*#[^;]+;': '--bg-deep: #111111;',
        r'--bg-card:\s*#[^;]+;': '--bg-card: #111111;',
        r'--bg-panel:\s*#[^;]+;': '--bg-panel: #181818;',
        r'--border:\s*[^;]+;': '--border: #333333;',
        r'--border-glow:\s*[^;]+;': '--border-glow: #444444;',
        r'--accent:\s*#[^;]+;': '--accent: #16A34A;',
        r'--accent-dim:\s*[^;]+;': '--accent-dim: rgba(22, 163, 74, 0.15);',
        r'--accent2:\s*#[^;]+;': '--accent2: #16A34A;',
        r'--green:\s*#[^;]+;': '--green: #16A34A;',
        r'--red:\s*#[^;]+;': '--red: #DC2626;',
        r'--amber:\s*#[^;]+;': '--amber: #FACC15;',
        r'--orange:\s*#[^;]+;': '--orange: #FACC15;',
        r'--text-hi:\s*#[^;]+;': '--text-hi: #FFFFFF;',
        r'--text-mid:\s*#[^;]+;': '--text-mid: #A3A3A3;',
        r'--text-lo:\s*#[^;]+;': '--text-lo: #737373;',
        r"--font-mono:\s*[^;]+;": "--font-mono: 'IBM Plex Mono', 'Consolas', 'Fira Code', monospace;",
        r"--font-ui:\s*[^;]+;": "--font-ui: 'Helvetica Neue', Arial, sans-serif;",
        r'--radius:\s*[^;]+;': '--radius: 0px;',
        r'--radius-sm:\s*[^;]+;': '--radius-sm: 2px;'
    }

    for k, v in replacements.items():
        text = re.sub(k, v, text)

    # 2. Eradicate web trends
    text = re.sub(r'box-shadow:\s*[^;]+;', '', text)
    # Target all explicitly set border-radius
    text = re.sub(r'border-radius:\s*(?:50%|\d+px)[^;]*;', 'border-radius: 0px;', text)
    
    text = re.sub(r'background(?:-image)?:\s*(?:linear|radial)-gradient\([^;]+\);', 'background: var(--bg-panel);', text)
    text = re.sub(r'-webkit-background-clip:\s*text;', '', text)
    text = re.sub(r'-webkit-text-fill-color:\s*transparent;', '', text)
    text = re.sub(r'backdrop-filter:\s*[^;]+;', '', text)
    text = re.sub(r'filter:\s*drop-shadow\([^;]+\);', '', text)
    text = re.sub(r'filter:\s*url\([^;]+\);', '', text) 

    # 3. Terminology & Classes
    term_replacements = {
        'Single-Turn Angle': 'Base Azimuth Angle',
        'Live Rotation': 'Kinematics (J1)',
        'Telemetry <span': 'Telemetry Data <span',
        'Magnet Health': 'Encoder Diagnostics',
        'Model Configuration': 'Digital Twin Settings',
        'Controls</div>': 'Jog Panel</div>',
        'NEMA 17 Digital Twin': 'SCADA HMI - J1',
        'Joint 1 · Base Azimuth · AS5600 Encoder': 'AXIS 1 (AZIMUTH) - SYSTEM NOMINAL',
        'id="viewport"': 'id="viewport" class="pane-viewport"',
        'id="panel"': 'id="panel" class="pane-controls"',
        'class="panel-section"': 'class="telemetry-grid"',
        '#viewport {': '.pane-viewport {',
        '#viewport::after {': '.pane-viewport::after {',
        '#panel {': '.pane-controls {',
        '#panel::-webkit-scrollbar': '.pane-controls::-webkit-scrollbar',
        '.panel-section {': '.telemetry-grid {',
        'url(#gaugeGrad)': 'var(--accent)',
        'url(#sparkGrad)': 'var(--accent-dim)'
    }

    for k, v in term_replacements.items():
        text = text.replace(k, v)

    # Ensure live data uses Mono font
    text = re.sub(r'(#angle-big \.val {)', r'\1 font-family: var(--font-mono);', text)

    # 4. Remove HTML comments
    text = re.sub(r'<!--.*?-->', '', text, flags=re.DOTALL)
    
    # 5. Remove Google fonts link
    text = re.sub(r'<link href="https://fonts.googleapis.com/css2\?family=[^"]+" rel="stylesheet" />', '', text)

    with open('digital_twin.html', 'w', encoding='utf-8') as f:
        f.write(text)
    
    print("Refactoring complete.")

if __name__ == '__main__':
    refactor()
