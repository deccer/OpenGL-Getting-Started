This will be super short.

You should make yourself familiar with the debugger of your choice.
Some come with or are built into integrated development environments, some are standalone programs.

[Visual Studio](https://learn.microsoft.com/en-us/visualstudio/debugger/debugger-feature-tour?view=vs-2022)

[GDB Documentation](https://www.sourceware.org/gdb/documentation/)
[GDB Cheat Sheet](https://gist.github.com/rkubik/b96c23bd8ed58333de37f2b8cd052c30)
[Valgrind](https://valgrind.org/docs/manual/quick-start.html)

Those tools help you to find out whether your code works as intended or produces the right values.
Get used to using it, before asking for help anywhere else. A lot of places/forums/discord servers also assume that you tried helping yourself first.

This should cover the pure code side, unrelated to `OpenGL` or any other graphics API.

It also helps to take a look at the [OpenGL Specification](https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf). It will show you what is valid and what isnt valid to call. Which formats and parameters etc are possible. Make sure to bookmark this document. Same applies to the [GLSL Specification](https://www.opengl.org/registry/doc/GLSLangSpec.4.60.pdf). It contains everything related to GLSL, parameters, attributes, identifiers and keywords. This should also go into your bookmarks.

In the next chapter I will show you how you can ask `OpenGL` to be nice to you.