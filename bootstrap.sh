#!/bin/sh

# This bootstrap.sh is a shell script that produces on stdout our bootstrap block.
# The "bootstrap" config param takes a command to run, which is where we use this.
# Running the ex command ":bootstrap" in cmpr then runs this, and the bootstrap block is sent to the LLM.
# You can adapt this to your own project or create your own script or program that outputs what you need.

cat <<EOF
We are writing cmpr, which is a tool to interact with LLMs and is written in C.

Here is information about spanio, the library we are using:

Note that spanio and all needed system header files are already included elsewhere, so don't include header files in your reply.
EOF

cmpr --print-block $(cmpr --find-block "#libraryintro")

# experimenting with leaving out the ctags, since our library intro and #all_functions should be sufficient

#cat <<EOF
#Here's our ctags:
#
#\`\`\`
#EOF
#
#ctags cmpr/spanio.c cmpr/cmpr.c
#
#cat cmpr/tags
#
#cat <<EOF
#
#\`\`\`

cat <<EOF
That is the end of the spanio library; now we describe our program, cmpr.

We have a few globals; to those already part of spanio (inp, out, cmp, etc.) we add \`state\`, a pointer to the \`ui_state\` struct, automatically available everywhere; the definition of \`ui_state\` is provided below, along with our config fields and the projfile struct.

EOF

cmpr --print-code $(cmpr --find-block "#config_fields")
cmpr --print-code $(cmpr --find-block "#ui_state")
cmpr --print-code $(cmpr --find-block "#projfiles")

cat <<EOF
The projfiles type is created by MAKE_ARENA and has the methods defined for our generic array types (with element type projfile).
In particular, we use projfiles_alloc with a sufficient capacity in main() and then we use projfiles_push() whenever we populate a project file.
EOF

echo
echo "Here are the declarations of all our functions:\n\n\`\`\`c"

#cmpr --print-code $(cmpr --find-block "#all_functions")
make fdecls.h >/dev/null
cat fdecls.h

echo "\`\`\`"
echo "Reply with \"OK\"."

#In the library intro above there are some idioms and advice given at the very end. Extremely briefly, summarize these in bullet form, starting with "- spans uses .s not .a".
