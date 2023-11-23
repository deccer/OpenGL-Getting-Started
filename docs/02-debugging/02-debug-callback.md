The debug callback is a piece of code called by OpenGL in case it disagrees with the values you passed to its functions.

Let's hook it up.

A free floating function above `main`.

```cpp
--8<-- "src/01-02-BasicWindowAndTriangle/Main.cpp:40:99"
```

That alone doesn't do anything. We need to tell `OpenGL` about it too. After `gladLoadGLLoader` we add those lines.

```cpp
--8<-- "src/01-02-BasicWindowAndTriangle/Main.cpp:175:177"
```

`glDebugMessageCallback` hands over our callback to OpenGL, and the next `glEnable` call enables the debug callback in general.

The next line `glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS)` is the neat part.

If we place a break point in our debug callback, let's say at this line `spdlog::error("OpenGL Message: ...` then we will naturally get a call stack, but we will also get the right callstack and can follow it to the offending glXXX function which OpenGL was not happy about.

The output also contains somewhat meaningful output which can tell us what exactly we did wrong, well, its not always perfect in telling us whats the problem, but its 10000% better than what was used before... the dreaded `GLCHECK(...)` (or `GLCALL` or whatever else people used to call it) macro, which in most cases called `glGetError` which didnt provide any meaningful hint about what the problem is.

Let's try it out.

I have prepared 2 exercises.


