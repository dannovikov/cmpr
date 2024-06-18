#v8

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

Markdown is now supported; each heading creates a block.
(If you have two headings, like "# big heading\n## sub-heading" then you'll get two blocks.)
The markdown blocks don't have any "code part", they are all NL and no PL, so most of the LLM features like "r" won't make sense to use with them.
It's just a quick way to move through markdown files and keep your code organized.
