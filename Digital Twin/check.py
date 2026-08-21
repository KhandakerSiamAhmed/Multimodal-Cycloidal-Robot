import sys

def check_brackets(filepath):
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            html = f.read()
    except Exception as e:
        print(f'Error reading file: {e}')
        return

    if '<script type="module">' not in html:
        print('No module script found!')
        return
        
    js = html.split('<script type="module">')[1].split('</script>')[0]
    
    brackets = {'{': '}', '(': ')', '[': ']'}
    stack = []
    in_string = False
    string_char = ''
    in_comment = False
    
    for i, c in enumerate(js):
        if in_comment:
            if c == '\n':
                in_comment = False
            continue
            
        if in_string:
            if c == string_char and js[i-1] != '\\':
                in_string = False
            continue
            
        if c in '\"\'`':
            in_string = True
            string_char = c
            continue
            
        if c == '/' and i+1 < len(js) and js[i+1] == '/':
            in_comment = True
            continue
            
        if c in brackets:
            stack.append((c, i))
        elif c in brackets.values():
            if not stack:
                print(f'Unmatched {c} at index {i}')
                print(js[max(0, i-20):min(len(js), i+20)])
                return
            if brackets[stack[-1][0]] == c:
                stack.pop()
            else:
                print(f'Mismatched {c} at index {i}, expected {brackets[stack[-1][0]]}')
                print(js[max(0, i-20):min(len(js), i+20)])
                return
                
    if stack:
        c, i = stack[-1]
        print(f'Unclosed {c} at index {i}')
        print(js[max(0, i-20):min(len(js), i+20)])
        return
        
    print('Syntax OK')

check_brackets('digital_twin.html')
