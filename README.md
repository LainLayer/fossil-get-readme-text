# Overview

This is a small C program for getting the text of the latest version of the `README.md` file
out of a fossil file.

# Purpose

It is meant as a reference example for interfacing with fossil using sqlite3
programmatically from C.

The functionality of this program can entirely be replicated without C, using
the built in `sqlite3` command of the fossil cli tool.

However, the sqlite3 shell provided by fossil is different from normal sqlite3.
Fossil has its own [SQL functions](https://www.fossil-scm.org/home/help/sqlite3), notably
the `content()` function would not only decompress the data, but also resolve
deltas, allowing the user to also view previous versions of the file.

# Build

```bash
./build.sh
```

You may modify the build command inside with `-DSILENT` to disable the debug printing.

The code depends on sqlite3 and zlib. Install those.

# Usage

```bash
./main test.fossil
```

The text content will be sent to stdout.
Debug prints are sent to stderr.

# How it works

From what I could gather, fossil stores only previous versions as deltas from the most
recent version. I am new to fossil, and there is no documentation I could find on the
topic, but from my testing, this seems to be true.

So my current theoretical model of how fossil works is as such:
```
file_v1(file id 2)
commit
delta_to_v1(file id 2) file_v2(file id 4)
commit
delta_to_v1(file id 2) delta_to_v2(file id 4) file_v3(file id 6)
etc...
```

The reason why the file ID skips one in this example is because fossil also adds a
"manifest" blob per commit. So the first file starts from ID 2, as ID 1 is the
initial empty commit manifest.

# Disclaimer

Please do not rely on this code for real work, I have not verified that it
it works in EVERY usecase or every version or every codebase.
This is just me playing around with a cool technology for fun.

If I made incorrect assumptions or there is an issue with the code, please do tell me.

# Useful links

  - The `sqlite3` command of fossil - https://www.fossil-scm.org/home/help/sqlite3
  - Useful SQL snippets             - https://fossil.wanderinghorse.net/r/libfossil/wiki?name=SQLSnippets
  - The fossil delta format         - https://fossil-scm.org/home/doc/tip/www/delta_format.wiki
  - Fossil source code tarball      - https://fossil-scm.org/home/uv/download.html
