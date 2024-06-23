import re
import sys

def extract_function_declarations(code):
    pattern = re.compile(r'^[a-zA-Z_]\w*\s+\*?\w+\s*\([^;\n{}]*\)\s*;[^\n]*$', re.MULTILINE)
    matches = pattern.findall(code)
    return matches

def main():
    code = sys.stdin.read()

    declarations = extract_function_declarations(code)

    for decl in declarations:
        print(decl)

if __name__ == "__main__":
    main()
