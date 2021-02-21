# Input

`[^\n]` is a kind of regular expression.

- `[...]`: it matches a nonempty sequence of characters from the *scanset* (a set of characters given by `...`).
- `^` means that the scanset is "negated": it is given by its *complement*.
- `^\n`: the scanset is all characters except `\n`.

`scanf("%[^\n]", line);` will read all characters until you reach `\n` (or `EOF`) and put them in `line`. It is a common idiom to read a whole line in C.

But `scanf("%[^\n]",line);` is a **problematic** way to read a *line*. It worse than [`gets()`](https://stackoverflow.com/q/1694036/2410359). [link](https://stackoverflow.com/questions/39431924/what-does-n-mean-in-c)

Recommend: use `fgets()` to read a *line* of input.