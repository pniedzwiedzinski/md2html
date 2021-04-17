# md2html

> Markdown to html renderer that works on Windows

Fork of [md2html](https://github.com/kukrimate/md2html). The magic is that it works
on Linux, *BSD, macOs, Windows thanks to [cosmopolitan](https://github.com/jart/cosmopolitan).

## Running

```sh
wget https://niedzwiedzinski.cyou/md2html.com
chmod +x md2html.com
sha256sum md2html.com
# 0f1477ae8abfbc9c973d5149a63e5fe33d5121d515de2c9379e29bca42736d1c

# For zsh or fish
bash -c "./md2html.com"

# For everything else
./md2html.com
```

## Compiling

Requirements:

* make
* gcc
* objcopy (from pkg `binutils`)

```
make
```
