In this chapter we will be adding quite a bit of code, code which will support us finding errors and identifying problems,
because sooner or later we will run into them and we should figure out how to help ourselves as soon as possible.

The amount of code can be a bit overwhelming, but bear with me. It's also not THAT bad.

First of all, quick recap of our project structure from the previous chapter.

```
OpenGLGettingStarted
├── lib
│   └── CMakeLists.txt             # here we describe all third party dependencies
│                                  # like glfw, glad, spdlog, imgui, ...
├── src
│   ├── 01-01-BasicWindow
│   │   ├── Main.cpp               # guess what is in here
│   │   └── CMakeLists.txt         # or here
└── CMakeLists.txt                 # Thats our solution file
```

We need to extend it so that we will create a new directory in `src` and call it `01-02-BasicWindowAndTriangle`.
We copy paste `Main.cpp` and `CMakeLists.txt` from `src/01-01-BasicWindow` into the newly created directory and make
sure to adjust `CMakeLists.txt` accordingly - it should just be naming things for now - I hope you can figure that out by yourself.

Now is probably a good time to talk about the

## Graphics Pipeline

### Vertex Input

![Vertex Input](graphics-pipeline-01-vertex-input.png)

DirectX is calling it `Input Assembly`, I also kind of like this name better for some reason (RenderDoc which I explain later also calls it `Input Assembly`). Anyway. Our models and meshes we want to render are made out of vertices. We may want to render them as lines, points, or triangle fans, but for now we'll do a triangle list, and that is setup here in the input assembly or vertex input stage.

!!! note "I might use `Input Assembly` or `IA` instead of vertex input when I elaborate on things, out of habit, but I mean `Vertex Input`"

You might have seen export options in tools such as [Blender](https://blender.org) or perhaps fiddled with libraries like [Assimp](https://github.com/assimp/assimp) where options like `Triangulate Vertices` exist of some form. That's why. Artists might use "quads" or other topologies, but the graphics card likes to munch on triangles.

That is also a good point to introduce a construct like `Vertex` or to be more specific in our case a `VertexPositionColor` construct.

```cpp
--8<-- "src/01-02-BasicWindowAndTriangle/Main.cpp:14:18"
```

A vertex is each point in a model or mesh describing its position in model space amongst other attributes, like color in this specific one. Vertices can also hold other attributes such as texture coordinates (aka `UVs`), normals, tangents or anything else really.

It usually depends on what kind of thing you are actually trying to implement here but in most cases its one or more of the aforementioned attributes. We will be using textures and normals later on for instance.

Speaking of instance. Models and meshes can also be "instanced" by the `GPU`. Think of copies of the same model or mesh but placed somewhere else, like a Tree in a forest, grass or an asteroid in an asteroid field. Information about those instances can also be encoded into the Vertex, but we will not make use of it in this guide because there are better means to do so IMHO but I wanted to mention it briefly. How instancing works I will also explain later.

!!! note "Model or Mesh"

    ...are terms which can be used interchangably here, both are collections of vertices mainly. I might find a better term for it later and adjust this guide accordingly.


Back to the `Vertex Input`. We have our vertices, which we might have been loaded from a model file  or are arranged in a list to make up some primitive shapes like a box or sphere. We also have our primitive type which can be point, line, line-strip, triangle-strip, triangle-fan and triangle.

Our vertices are usually collections of `Vertex` and those are sent over to the `GPU` that they can be used to render them.

Let's declare that thing right after we initialized `OpenGL` with default values after `glClearDepth(1.0f);`

```cpp
--8<-- "src/01-02-BasicWindowAndTriangle/Main.cpp:187:192"
```

Those collections are stored in a buffer. The `GPU` can access them to retrieve whatever information is stored in them, in this case vertices and the buffer in this specific instance is called `vertex buffer`.

Buffers are a generic `OpenGL` object, referring to a blob of memory with size if you will.

!!! note "Explain buffers here, perhaps in more detail"

To create a buffer we simply call these functions and stuff the vertex collection right into it. 

```cpp
--8<-- "src/01-02-BasicWindowAndTriangle/Main.cpp:193:195"
```

!!! note "Elaborate"

    Explain immutable storage and the difference to the old glBindBuffer stuff?

We need one more thing to complete the Vertex Input stage. We need it to tell the `GPU` how to interpret the data coming from the vertex buffer(s).

That thing is unfortunately named `Vertex Array Object` (or `VAO` in short) in OpenGL. God knows why, but we have to deal with it.

I have a better name for it. I would like to call it `Input Layout` (I might refer to it as `IL` too). It makes much more sense to me because the input layout as the name suggests describes a layout of sorts for some input. And if  you remember we are in the `Vertex Input` stage still. 

The input layout is created as follows:

```cpp
--8<-- "src/01-02-BasicWindowAndTriangle/Main.cpp:197:206"
```

A vertex may have "attributes" that describe its constituent data.

For instance, our vertex's first attribute is "position," consisting of 3 floating point components. We specify that the 0th attribute of `inputlayout` consists of 3 floats (`GL_FLOAT`), which are not normalized (`GL_FALSE`), and then specify the offset of the `Position` data within our `VertexPositionColor` struct:

`glVertexArrayAttribFormat(inputLayout, 0, 3, GL_FLOAT, GL_FALSE, offsetof(VertexPositionColor, Position));`

!!! note "Elaborate"

    Explain AOS/SOA/Interleaved/NonInterleaved VertexFormat?

Last but not least, we associate our vertex buffer with our input layout.

```cpp
--8<-- "src/01-02-BasicWindowAndTriangle/Main.cpp:209:209"
```

!!! danger "Exercise"

    Can you create a vertex type similar to our `VertexPositionColor` which has 2 more attributes. I also want normals and tangents. The former usually is a `glm::vec3` the latter a `glm::vec4`. How would you name it? And how would the corresponding input layout look like?

### Vertex Shader

![Vertex Shader](graphics-pipeline-02-vertex-shader.png)

The Vertex shader takes a single vertex as the input. It's main purpose is to transform coordinates from one coordinate system into another. It usually takes your model vertex positions and transforms them into normalized device coordinates (x direction: [-1.0f, 1.0f], y direction: [-1.0f, 1.0f]). That usually happens via matrix manipulations which we will cover later.

The actual coordinates on the screen are achieved when the normalized device coordinates are transformed to screen coordinates via `glViewport`. While writing this I noticed a bubu I made earlier. Its not really a problem but I  would like it to be somewhat proper. `glViewport` is only called when we resize the window, we never set it initially and `GLFW` is also not calling the framebufferresize callback after it created the window. Lets add the following lines after line 144 where we call `gladLoadGLLoader(...)`.

```cpp
--8<-- "src/01-02-BasicWindowAndTriangle/Main.cpp:170:173"
```

Shaders are programs which run on the `GPU`. We can modify them in order to let them do what we want. In case of the vertex shader we want to it to position our vertices and pass the color of each vertex onto the fragment shader (will talk about it in a second). Therefore the shader code looks rather simple.

```cpp
--8<-- "src/01-02-BasicWindowAndTriangle/Main.cpp:211:228"
```

For this example I put the shader code right into the actual source code, usually those programs are separate files, stored on disk somewhere with a proper filename to indicate which program it is.

!!! note "File naming conventions"

    I use the following naming convention for all my shader files

    MeaningfulName.`xx`.glsl

    Where xx stands for the shader, or its shortcut rather.

    Examples:

        FullScreenTriangle.vs.glsl - a vertex shader

        CullVisibility.fs.glsl - a fragment shader

        CullLights.cs.glsl - a compute shader

    Other people might use .vert, .frag, .comp or .vs, .fs, .cs as file extensions, that will most likely trip your OS into believing those are not shaders but post script files or otherwise, Code highlighting tools may or may not like those files right away or require configuration. Pick your poison.

As you can see we take in input, and we return values in the vertex shader. I wonder if you also notice the 2 input attributes `i_position` and `i_color`. Does this look familiar to you? Our input layout also has those 2 attributes. Even the data type matches... `vec3` ... a thing consisting of 3 floats. Now the name "input layout" should even make more sense, `VertexArrayObject` really does not, right?

We write the current position of the vertex into `gl_Position` and pass the vertex color onto the next stage in form of a so called varying (hence the prefix `v_`).

The shader source along wont do anything, we need to construct a program, compile and link it, check if the compilation actually worked.

For that let's write a little helper function, since we will be using it to create the fragment shader later too, and we can practice a little code reuse already.

```cpp
--8<-- "src/01-02-BasicWindowAndTriangle/Main.cpp:20:38"
```

Let's put it below where we declared our `VertexPositionColor` type.

What does it do? It creates an `OpenGL` object to represent a program, we will pass the shader source along, as well as what shader program exactly, vertex shader, fragment shader or compute shader. It will compile and link the program and check whether there was a problem or not.
If there was problem it will be returned as error message and if everything was alright we will
get back the program itself.

Here is how we use it to create the vertex shader we need

```cpp
--8<-- "src/01-02-BasicWindowAndTriangle/Main.cpp:242:248"
```

### Tesselation Control Stage

![Tesselation Control Stage](graphics-pipeline-03-tesselation-control-shader.png)

We will not cover TCS, since its outdated tech and not recommended to be used anymore

However if you are curious what is is and how it can be used ogldev has videos on this and the following 2 stages.

### Tesselation Evaluation Stage

![Tesselation Evaluation Stage](graphics-pipeline-04-tesselation-evaluation-shader.png)

We will not cover TES, since its outdated tech and not recommended to be used anymore

### Geometry Shader

![Geometry Shader](graphics-pipeline-05-geometry-shader.png)

We will not cover geometry shaders, since its outdated tech and not recommended to be used anymore

### Rasterizer Stage

![Rasterizer Stage](graphics-pipeline-06-rasterizer-stage.png)

This stage receives the output of the vertex shading (remember we ignored TCS, TES, GS) stage. Here is where the primitives (triangle, points, lines) are mapped to screen positions. to produce fragments for the fragment shader to process.

The Rasterizer also clips fragments which are not part of the viewport.

Other things affecting the rasterizer are face culling and face winding.
The former defaults to no face culling on opengl, that means front and backfaces are rasterized, but usually back face culling is enabled. This comes in handy for closed objects like a cube, where you dont need to render the insides of it when you never go into the cube. The other thing, face winding, is there to tell in which order the vertices of primitives are processed. Clockwise (`CW`) or Counterclockwise `CCW`, OpenGL's default is `CCW`.

We did setup those things too here

```cpp
--8<-- "src/01-02-BasicWindowAndTriangle/Main.cpp:180:182"
```

### Fragment Shader

![Fragment Shader](graphics-pipeline-07-fragment-shader.png)

The fragment shader takes the fragments from the rasterizer stage and "colors" them in this stage. That's what most of the time is happening here anyway.

The fragment shader is also an object we create, similar to the vertex shader

```cpp
--8<-- "src/01-02-BasicWindowAndTriangle/Main.cpp:250:256"
```

But with just those 2 programs we cannot do much. We need to hook them up in a program pipeline.

With shader program pipelines shader programs can be mix and matched together like you want, without having to recreate pairs of vertex/fragment shaders for instance, ie. Create 1 vertex shader program and combine it with various fragment shaders, like for post effects.

Let's create that program pipeline

```cpp
--8<-- "src/01-02-BasicWindowAndTriangle/Main.cpp:258:261"
```

### Output Merger ("FB")

![Tests & Blending](graphics-pipeline-08-tests-blending.png)

I would like to call this stage `Output Merger`, just like Microsoft did name this stage in DirectX. It makes more sense since in this stage all relevant output is merged into the final output. This could mean output to multiple framebuffers, while alpha, blending, stencil and depth testing is happening.

# Compute Pipeline

Compute Pipeline

![Compute Shader](compute-pipeline-01-compute-shader.png)

Compute shaders are general purpose programs which can do whatever you like, they dont specialize in rasterization like stuff does in the graphics pipeline. We might take a look at compute shaders later on.

# Final Draw Call

We setup our rendering pipeline, loaded shaders, initialized vertex buffers and inputlayout, what is left to do is
tell `OpenGL` to use those things in order to render the triangle.

The shaders are associated with `programPipeline` and the vertex buffer is associated with the `inputLayout`. We bind them and issue the actual draw call.

```cpp
--8<-- "src/01-02-BasicWindowAndTriangle/Main.cpp:263:276"
```

If you compile and run you should see this:

![Triangle](graphics-pipeline-triangle.png)

# Cleanup

It goes without saying that you should clean up after yourself.

We do it in the opposite direction of when we created all those things.

```cpp
--8<-- "src/01-02-BasicWindowAndTriangle/Main.cpp:278:286"
```
