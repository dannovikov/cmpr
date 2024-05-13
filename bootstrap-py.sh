#!/bin/bash

remove_common_python_functions() {
    grep -v '__len__' | grep -v '__getitem__'
}

# Function to check and process Python files in directories from PYTHONPATH
process_pythonpath() {
    IFS=':' read -ra DIRS <<< "$PYTHONPATH"
    for dir in "${DIRS[@]}"; do
        # Check if directory contains __init__.py
        if [ -f "$dir/__init__.py" ]; then
            # If yes, find Python files in this directory
            find "$dir" -maxdepth 1 -type f -name '*.py' -exec ctags -x {} + | grep -v namespace | grep -v variable | remove_common_python_functions
        fi
    done
}

cat <<EOF
Here's a description for a program I'm currently working on:
\`\`\`python
EOF

echo -n "$(cmpr --print-block 1)"

cat <<EOF

\`\`\`

Current imports:
\`\`\`python
EOF

echo -n "$(cmpr --print-block $(cmpr --find-block "Imports"))"

cat <<EOF

\`\`\`

Output from \`ctags -x\` run on the program:
\`\`\`
EOF

# Find Python files in current directory, excluding dot-prefixed directories
for f in `ls *.py`; do
    ctags -x "$f" | remove_common_python_functions
done

for dir in "$@"; do
    find "$dir" -type d \( -name ".*" -prune \) -o -type f -name '*.py' -exec ctags -x {} + | remove_common_python_functions
done

cat <<EOF
\`\`\`

Output from \`ctags -x\` run on library code I've written:
\`\`\`
EOF

# Process PYTHONPATH directories
process_pythonpath

cat <<EOF
\`\`\`

I will provide you with descriptions and you will provide me with code that satisfies the description.

Reply with only with OK.
EOF
