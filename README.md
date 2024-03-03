# llvm-Kaleidoscope

A simple implementation of the Kaleidoscope language using LLVM-14.
- This repo is for LLVM-14. Unfortunately there are some differences between LLVM-14 and the latest version of LLVM, but the differences are not significant.
- For Latest version of LLVM: https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html
- For LLVM-14, also my version: https://releases.llvm.org/14.0.0/docs/tutorial/MyFirstLanguageFrontend/

Progress:
- [x] Chapter 1: Kaleidoscope: Implementing a Language with LLVM
- [x] Chapter 2: Implementing a Parser and AST
- [x] Chapter 3: Code generation to LLVM IR
- [ ] Chapter 4: Adding JIT and Optimizer Support

Build:
```bash
mkdir build
make clean
make all
```

Demo:
```llvm
$ ./kaleidoscope
>>> 4+5;
>>> Read top-level expression: 
define double @0() {
entry:
  ret double 9.000000e+00
}

>>> def foo(a b) a*a + 2*a*b + b*b;
>>> Read function definition: 
define double @foo(double %a, double %b) {
entry:
  %multmp = fmul double %a, %a
  %multmp1 = fmul double 2.000000e+00, %a
  %multmp2 = fmul double %multmp1, %b
  %addtmp = fadd double %multmp, %multmp2
  %multmp3 = fmul double %b, %b
  %addtmp4 = fadd double %addtmp, %multmp3
  ret double %addtmp4
}

>>> def bar(a) foo(a, 4.0) + bar(31337);
>>> Read function definition: 
define double @bar(double %a) {
entry:
  %calltmp = call double @foo(double %a, double 4.000000e+00)
  %calltmp1 = call double @bar(double 3.133700e+04)
  %addtmp = fadd double %calltmp, %calltmp1
  ret double %addtmp
}

>>> extern cos(x);
>>> Read extern: 
declare double @cos(double)

>>> cos(1.234);
>>> Read top-level expression: 
define double @1() {
entry:
  %calltmp = call double @cos(double 1.234000e+00)
  ret double %calltmp
}

>>> ^D
>>> ; ModuleID = 'my jit'
source_filename = "my jit"

define double @0() {
entry:
  ret double 9.000000e+00
}

define double @foo(double %a, double %b) {
entry:
  %multmp = fmul double %a, %a
  %multmp1 = fmul double 2.000000e+00, %a
  %multmp2 = fmul double %multmp1, %b
  %addtmp = fadd double %multmp, %multmp2
  %multmp3 = fmul double %b, %b
  %addtmp4 = fadd double %addtmp, %multmp3
  ret double %addtmp4
}

define double @bar(double %a) {
entry:
  %calltmp = call double @foo(double %a, double 4.000000e+00)
  %calltmp1 = call double @bar(double 3.133700e+04)
  %addtmp = fadd double %calltmp, %calltmp1
  ret double %addtmp
}

declare double @cos(double)

define double @1() {
entry:
  %calltmp = call double @cos(double 1.234000e+00)
  ret double %calltmp
}
```