# Concise Error Notation

This programn converts normal error notation to concise notation and
vice versa (based on whether the input has parentheses in it or not).

The input is taken from `stdin` and output is written to `stdout`.

Without a pipe, it can be used interactively (but there is no prompt):

```sh
$ concise
1.2 .1                           # input
1.2(1)                           # output
8±1                              # input
0.8(1)E1                         # output
[Ctrl-D]                         # quits
```

Any input that contains parentheses will be parsed as already in
concise notation. Otherwise, the program attempts to find two numbers
(value and uncertainty).

## Examples

With concise notation of standard error, the uncertainty of the number is written in parentheses `(` and `)` after the mantissa/significand of the number:

```
 1.23(54) = 1.23 ± 0.54
1.230(54) = 1.23 ± 0.054
4.32(1)E4 = (4.32 ± 0.01) × 10⁴
```

The error in parentheses affects the last digits of the number before
it:

```sh
$ echo "1.2(1)" | ./concise
1.2	0.1
$ echo "1.2 0.1" | ./concise
1.2(1)
```

The character between the two input numbers can be anything non-numeric:

```sh
$ echo "1.2±0.001" | ./concise
1.200(1)
```

### Normal to Concise

```sh
{
cat<<EOF
1.2 1
1.23 0.09
1.99E3 1
1.99e3 1
EOF
} | ./concise
```

result:
```sh
1(1)
1.23(9)
1.990(1)E3
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
} | ./concise
```

result:
```
1.2	0.1
1.23	0.09
1897	13
```

## OPTIONS

The program takes options that affect the output. Reading is more
restrictive than writing, the reader cannot parse every kind of output
this program produces.

Most of these are _fun options_ and need not exist. All can be replaced by piping the default output through sed in some way.

The default behaviour is the two-state equivalent of _idempotent_, if there is such a thing: it switches back and forth:

```sh
$ echo "1.4e-7 0.1e-8" | concise | concise | concise
1.40(1)E-7
$ echo "1.4e-7 0.1e-8" | concise | concise
1.4e-07	1e-09
```

Turning on any of the below options destroys this behaviour.

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

## Funny things that work somewhat conincidentally

```sh
$ echo "1.2(3" | concise               # ) doesn't matter
1.2	0.3
$ echo "1.2.3" | concise               # .3 is an OK number, same as 1.2 ± 0.3
1.2(3)
$ echo "2.4~ish, but uncertain: this uncertain: 0.1" | concise # long separator
2.4(1)
$ echo "2.4~0.1" | concise             # short separator
2.4(1)
$ echo "1.2((((2))))" | concise        # number of () doesn't matter
1.2	0.2
$ echo "1..1" | concise                # 1. and .1 are both OK numbers
1.0(1)
```

## Funny things that don't work

Now that we know that lots of input is accepted, we can search for things that produce unexpected results:

```sh
$ echo "1.2{1}" | concise              # as 1.2 ± 0.1, but {} don't replace ()
1(1)
$ echo "1.2-0.1" | concise             # using a sign (+-) in the separator
1.2(-1)
$ echo "(100 ± 3) * 10^3" | concise    # parentheses trigger reading concise notation
0	1e+06
```

Additional note: this program doesn't do math, it parses numbers, so none of these work:

```sh
$ echo "1.3 * 10**3 ± 1" | concise
1(10)
$ echo "[1.321 ± 0.001] * 10^2" | concise
0(0)
```
