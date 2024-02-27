# llvm-Kaleidoscope

A simple implementation of the Kaleidoscope language using LLVM.
- https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html

Build:
```shell
mkdir build
make clean
make all
```

Demo:
```shell
$ ./kaleidoscope
ready> def foo(x y) x+y );
ready> Parsed a function definition.
ready> LogError: unknown token when Exprcting an expression
ready> extern sin(a);
ready> Parsed an extern
ready> def foo(x y) x+foo(y, 4.0);
ready> Parsed a function definition.
ready> def foo(x y) x+y y;
ready> Parsed a function definition.
ready> Parsed a top-level expr
ready> ^C
```