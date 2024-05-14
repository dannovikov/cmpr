#!/bin/bash

filter_common_python_functions() {
    grep -v '__len__' | grep -v '__getitem__' | awk '{split($4,a,"/"); $4=a[length(a)]; print $0}' | sort -n -k 3
}

# Function to check and process Python files in directories from PYTHONPATH
process_pythonpath() {
    IFS=':' read -ra DIRS <<< "$PYTHONPATH"
    for dir in "${DIRS[@]}"; do
        # Check if directory contains __init__.py
        if [ -f "$dir/__init__.py" ]; then
            # If yes, find Python files in this directory
            find "$dir" -maxdepth 1 -type f -name '*.py' -exec ctags -x {} + | grep -v namespace | grep -v variable | filter_common_python_functions
        fi
    done
}

cat <<EOF
I'm working on a Python program.

Top level comment:
\`\`\`python
EOF

echo -n "$(cmpr --print-block 1)"

cat <<EOF

\`\`\`

Existing functions:
\`\`\`
EOF

# Find Python files in current directory, excluding dot-prefixed directories
for f in `ls *.py`; do
    ctags -x "$f" | filter_common_python_functions
done

for dir in "$@"; do
    find "$dir" -type d \( -name ".*" -prune \) -o -type f -name '*.py' -exec ctags -x {} + | filter_common_python_functions
done

cat <<EOF
\`\`\`

You can also utilize thse library functions:
\`\`\`
EOF

# Process PYTHONPATH directories
process_pythonpath

cat <<EOF
\`\`\`

I will provide you with descriptions of code. Use existing functions, library code and 3rd party libraries and provide me with code that satisfies the description.

Don't redefine functions that already exist, just use them.

Reply with only with OK.
EOF
