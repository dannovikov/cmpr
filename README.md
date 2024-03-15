# CMPr

## Programming in English

Thesis: the future of programming is driving an LLM to write code.
Why?
Mostly, because it's way more fun.
You can work with any technology, so it's a good match for generalist skillsets.
When code is written by an AI, we treat it as disposable, which changes our relationship with the code in interesting ways.
It doesn't matter as much what language the code is in; what's important is the English that described that code.

## What's this then?

This is a early experimental tool to support the LLM programming workflow.

Today we just support ChatGPT, and interaction is by copy and paste.
This means you can use it with a free account, or if you have a paid account you can use GPT4, which writes better code.

(Coming soon: API usage, local models, competing LLMs, etc.)

This is a framework and represents a particular and very opinionated approach to this workflow.
We will be updating continuously as we learn.
This version was written in a week, using the workflow itself.

All code here is by ChatGPT4, and all comments by me, which is the idea of the workflow:

- You have a "block" which starts with a comment and ends with code.
- You write the comment; the LLM writes the code.
- You iterate on the comment until the code works and meets your standard.

The tool is a C program; compile it and run it locally.
The main editing loop shows you your "blocks".
You can use j/k to move from block to block.
Full text search with "/" is also supported (improvements coming soon!).
Once you find the block you want, use "e" to open it up in "$EDITOR" (or vim by default).
Then you do ":wq" or whatever makes your editor exit successfully, and a new revision of your code is automatically saved.

To get the LLM involved, when you're on a block you hit "r" and this puts a prompt into the clipboard for you.
(You won't see anything happen when you hit "r", but the clipboard has been updated.)
Then you switch over to your ChatGPT window and hit "Ctrl-V".
You could edit the prompt, but usually you'll just hit Enter.
ChatGPT writes the code, you can click "copy code" in the ChatGPT window, and then hit "R" (uppercase) back in cmpr to replace everything after the comment (i.e. the code half of the block) with the clipboard contents.
Mnemonic: "r" gets the LLM to "rewrite" (or "replace") the code to match the comment (and "R" is the opposite of "r").

You can currently also hit "q" to quit, "?" for short help, and "b" to build by running some build command that you specify.

## Quick start:

1. Get the code and build; assuming git repo at ~/cmpr and you have gcc, `gcc -o cmpr/cmpr cmpr/cmpr.c -lm`.
2. Put the executable in your path with e.g. `sudo install cmpr/cmpr /usr/local/bin`.
3. There will be a `cmpr --init` command soon but in the meantime, cd to the directory you want to work in and run `mkdir .cmpr .cmpr/revs .cmpr/tmp; touch .cmpr/conf`. This marks that directory so the cmpr tool can treat it as a project directory.
4. `export EDITOR=emacs` or whatever editor you use, or vi will be run by default.
5. Run `cmpr` in this directory, and it will ask you some configuration questions.
   If you want to change the answers later, you can edit the .cmpr/conf file.

Developed on Linux; volunteers and bug reports on other environments gladly welcomed!
We are using "xclip" to send the prompts to the clipboard.
This really improves quality of life over manual copying and pasting of comments into a ChatGPT window.
The first time you use the 'r' or 'R' commands you will be prompted for the command to use to talk to the clipboard on your system.
For Mac you would use "pbcopy" and "pbpaste".

## More

Development is being [streamed on twitch](https://www.twitch.tv/inimino2).
Join [our discord](https://discord.gg/ekEq6jcEQ2).

*/
