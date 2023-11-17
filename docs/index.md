# Getting Started with OpenGL in 2023

![BasicTriangle](https://raw.githubusercontent.com/deccer/opengl-getting-started/main/screenshots/screenshot-basic-triangle.png)

!!! note "Attention"

    This thing is still work in progress...

So, what is this?

I am very much annoyed by all sorts of OpenGL tutorials out there in the wild. All of them do cover OpenGL and you can
get things done, have everything implemented of what you want. But never in a modern way.

Most, if not all, use OpenGL 3.3 and that version is quite old. Since then OpenGL moved on quite a bit
and got plenty of improvements which should also work on hardware from the past 10 years at least. I'm talking about OpenGL 4.6 and
its facilities for Direct State Access or DSA in short.
This thing will allow the user to modify state directly - ho ho - thats exactly what DSA stands for - instead of binding stuff to modify, 
which is a huge annoyance since you have to somewhat keep mental track of OpenGL's state when doing so, it's an unnecessary pain in the ass.

OpenGL 4.6 means people using macs are out of luck. There is simply no support for that version of OpenGL on Apple products.

It's not just the old version of OpenGL used in these tutorials, its also the lack of how the development environment should be setup
at the beginning to help the user detect problems right away or show how they can help themselves. 

And there is another big flaw is that they all use float arrays to explain vertices. That's weird and unreadable.

Instead of holding hands for each and every step, a tutorial for a somewhat complex topic like OpenGL should explain vertices right from the start,
like vertices are most likely more than just positions, they can describe normals, texture coordinates and other things. And that should be
explained right from the start. It wont overly complicate a beginners brainbut simplify ALOT of things later, especially troubleshooting.

There is also more than just the verticles :) But we will get to it.

If you have questions about this thing, I usually hang out on the Graphics Programming discord, feel free to boop
You can find the link in the most bottom right corner, in the footer on any page.

![](https://cdn-icons-png.flaticon.com/512/1808/1808086.png)

Happy reading.
