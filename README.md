# longjmp_win64

#### What?
This is a minimalist replacement for the `setjmp` and `longjmp` library calls, specifically for 64-bit windows applications compiled with vs2013/vs2015 and onward (though not yet tested with vs2017).

#### Why?
In the past it had been possible to exploit the behavior of setjmp/longjmp for the purpose of implementing coroutines, cooperative threads, and things of that nature.  One could even get the behavior consistent across win32, iOS, Android (NDK), and OSX.  In more recent times, changes to the implementation of those functions mean that it is no longer possible to use the vendor supplied versions of them for that purpose.

#### What changed in the setjmp/longjmp functions provided by vs2013/vs2015 for win64?

The newer win64 implementations of `longjmp` appear to do some extra 'scope aware' things, like calling destructors, unwinding the stack, etc. in much the same way that a C++ exception would do.  That's probably a good thing if you're not doing anything creative with `longjmp`, however those new behaviors are detrimental to exploits that rely on a more minimalist implementation, where nothing interferes with the stack or it's data.

#### This sounds like black magic
Yes, if you are already lost, then this is not for you.  It is intended for those who live dangerously and push boundaries.

#### How do I use it in my project?
This implementation consists of 2 source files..
```
longjmp_win64.h      <-- header for use in C and C++
longjmp_win64.asm    <-- 64-bit MASM source (Microsoft Macro Assembler); see instructions below for help compiling.
```

Simply include those files in your project, and **enable .asm compilation in your project** by right clicking on
Project, select Build Dependencies, Build Customizations, and turn the 'masm' option on.
You may also have to right click on the longjmp_win64.asm file itself, and in the 'general' section, change
'Item Type' from 'does not participate in build' to 'Microsoft Macro Assembler'.

If you have one or more 32-bit build targets you must exclude these files from them.  This is a 64-bit-only implementation.

#### What about calling conventions and all that jazz?
Thankfully, the binary ABI for Windows x64 has only one official calling convention, so
there is no need to worry about that stuff.<br/>
https://docs.microsoft.com/en-us/cpp/build/overview-of-x64-calling-conventions

#### Haven't you heard of inline assembly?
Microsoft dropped support for inline assembly in their x64 compilers.<br/>
https://docs.microsoft.com/en-us/cpp/build/intrinsics-and-inline-assembly

#### Handy Links
All the relevant stuff is linked from here ...<br/>
https://docs.microsoft.com/en-us/cpp/build/x64-software-conventions

Of particular interest would be these ...<br/>
https://docs.microsoft.com/en-us/cpp/build/overview-of-x64-calling-conventions <br/>
https://docs.microsoft.com/en-us/cpp/build/calling-convention <br/>
https://docs.microsoft.com/en-us/cpp/build/register-usage

Learn Assembly Language for x86 and x64 windows machines ...<br/>
*Assembly Language for x86 Processors (7th Edition)*<br/>
Amazon: https://www.amazon.ca/dp/0133769402  -- Very expensive, but if you're serious about it, invaluable.


#### Future Plans
* additional custom implementations of `setjmp` and `longjmp` for various platforms and bitness levels
* support for other 64 Windows compilers (eg. mingw-w64)
* a wrapper with unified interface (eg. `setjmp_custom` and `longjmp_custom` that automatically maps to the correct name/implementation for your target platform)
* sample code demonstrating a simple OO implementation of coroutines or cooperative threads


#### License
Written by Paul Glinker - Glinkie Games Inc.

```
Copyright 2017 Glinkie Games Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
