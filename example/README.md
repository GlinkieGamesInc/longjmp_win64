# example.cpp

### a simple example demonstrating ...
 * how to jump out of a function
 * the vendor supplied implementation of longjmp unwinds
   the stack and destroys local objects (as exceptions would)
 * the custom implementation of longjmp (longjmp_win64)
   does not unwind the stack or destroy local objects
   (the desired result in this case).

Intended for use with vs2013 or vs2015, x64 build target only.

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
