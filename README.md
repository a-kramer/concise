# Concise Error Notation

This programn converts normal error notation to concise notation and
vice versa.

The input is taken from `stdin` and output is written to `stdout`.

## Examples

```
 1.23(54) == 1.23 ± 0.54
1.230(54) == 1.23 ± 0.054
```

The error in parentheses affects the last digits of the number before
it.

### Normal to Concise

```sh
{
cat<<EOF
1.2 1
1.23 0.09
1.99E3 1
EOF
} | ./concise
```

result:
```sh
1(1)
1.23(9)
1.990(1)E3
```

### Concise to Normal

By default, reading concise notaion will output tabular content, separated by one TAB:

```sh
{
cat<<EOF
1.2(1)
1.23(09)
1.897(13)E3
EOF
} | ./concise -r
```

result:
```
1.2	0.1
1.23	0.09
1897	13
```

## OPTIONS

The program takes options that affect the output. This is the only option that affects function:

`-r` or `--reverse`/`--read` will read concise notation and outut normal.

Reading is more restrictive than writing, the reader cannot parse every kind of output this program produces.


### LaTeX Output

The `--latex` option will print something like this:

```sh
$ echo "1.2E2 0.1E2" | ./concise --latex
1.2(1)\times 10^{2}
```

And also:

```sh
$ echo "1.2(1)E2" | ./concise -r --latex
(1.2\pm 0.1)\times 10^{2}
```

### Plain but Fancy Text

The output can be made more pretty:

```sh
$ echo "1.2E3±0.1E3" | ./concise --cross
1.2(1)×10^{3}
```

And also:
```sh
$ echo "1.2E3±0.1E3" | ./concise --asterisk
1.2(1)*10^(3)

$ echo "1.2E3±0.1E3" | ./concise --dot
1.2(1)·10⁺⁰³
```

### Exponents

If there is a scale exponent \(10^n\), then these options modify how
it is written, say \(n=3\):

- `--parentheses`, `-(` selects `10^(3)`
- `--brackets`, `-[` selects `10^[3]`
- `--braces`, `-{` selects `10^{3}`
- `--mini`, `--superscript`,`-^` selects (utf-8) small superscript numbers: `10⁺⁰³`

Some of the combination of exponents and output formats don't make
much sense but are possible. Picking an output format automatically
selects the most appropriate type of exponent (e.g.: braces for
latex).

