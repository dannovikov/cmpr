# v9

### Undo

There is now a basic undo feature.

The first time you use the feature it will create a rev cache.
(Subsequent times will still have to read the cache, but it will be faster than the first time.)
The cache is only generated, or read, the first time you use the "U" command, so startup remains snappy.

To use the "U" command, hit "U" while on a block.
All previous revs will be searched for blocks that are "similar enough" to this one to count as a version of it.
"Similar enough" means that either 8 lines in the block are the same as the current contents of the block, or that one of the ids of the block is a match.
(At some future point we might give you a way to tune the sensitivity of the similarity search if it seems useful.)
(This is one reason to use multiple block ids, if you want to "connect" current version of code to some specific prior implementation.)
(This is another reason to give your blocks ids, though the similarity search works surprisingly well.)

Once you hit "U" you are in the undo mode.
Use j/k to navigate backwards (down) through the history and back up.
For each older version of the block you can see the timestamp when it was saved.
If you want to revert to an old version of a block, hit Enter on it, and it will replace the current contents of the block.
If you then want to go back again, you can use "U" again.
The previous version will still be in there, and since the similarity metric is symmetric, it will still be matched, even if the block doesn't have an id.

The similarity measure is very basic (and fast) but will probably be tuned over the coming months.
If you find any cases that don't do the most useful thing, please let us know about them!

# v8

### Block references

Block references are now supported.
There are two kinds, "inline" and "topline".
The inline refs are created by writing "@blockid" on a line alone in the NL code, while the topline refs are created by putting the "@blockid" ref on the top line of the block.
Block ids themselves are always created by writing "#blockid" on the top line of the block.
Blocks can have more than one id.
The best way to see these features for now is to check the cmpr source itself for examples.
You can use the ":expand" ex command in cmpr to see how the block you're looking at looks when expanded.
This can be a useful way to check some references quickly yourself.
You can use "@blockid:code" to include the PL section of the block instead of the natural language section, and "@blockid:all" to include both.

### Block ids

Block ids with the "hashtag syntax", like "#block\_id" are now a supported feature.
In addition to the block references feature using them, they are also used by the "#" keybinding, which lets you see and jump quickly to any block that has an id.

### Markdown support

Markdown is now supported.
Each heading creates a block: if you have "# h1\n## h2" you'll get two blocks.
Markdown blocks don't have a "code part", they are all NL and no PL.

### Windows and build fixes

Thanks to @dannovikov for the Windows improvements.

### Ollama support

Run the ollama server locally and load the models, and add the models you want to use to the conf file (e.g. "ollamas: llama3,llama3:70b").
The models will then appear in ":models".

### Llama.cpp support

Just run llama.cpp with the model you want to use, and use ":models" to pick llama.cpp.

### Curl binary instead of curllib

We no longer link against libcurl, but just call out to the curl binary.
You can set the curlbin in the conf file if you don't want the default (which is just "curl" in your PATH).

### bootstrap-py.sh

Thanks to @petterik for the Python bootstrap script.
To use it, copy bootstrap-py.sh into your project, put "bootstrap: ./bootstrap-py.sh" in your .cmpr/conf and modify it to suit your needs.
The script assumes you have a top-level comment in block 1 that is relevant for the LLM and that you have ctags installed.

# v7

### API support

Support for LLM rewriting directly via API is now supported in addition to the clipboard style interaction.
Use ":models" to switch between the modes, and put your OpenAI API key in .cmpr/openai-key.

# v6 ... v1

### bootstrap, --init, and conf file handling

Many minor improvements and fixes.
