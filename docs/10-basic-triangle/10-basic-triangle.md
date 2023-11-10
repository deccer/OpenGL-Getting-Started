# Basic Triangle

Like before in the `HelloWindowApplication` lets setup the files we need first.

### HelloTriangleBasicApplication.hpp
```cpp
#pragma once

#include "../Shared/Application.hpp"

class HelloTriangleBasicApplication final : public Application
{
protected:
    bool Load() override;
    void Unload() override;
    void Render() override;

private:
};
```

### HelloTriangleBasicApplication.cpp
```cpp
#include "HelloTriangleBasicApplication.hpp"
#include "VertexPositionUv.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <array>
#include <expected>
#include <format>
#include <vector>

bool HelloTriangleBasicApplication::Load()
{
    if (!Application::Load())
    {
        return false;
    }

    return true;
}

void HelloTriangleBasicApplication::Unload()
{
    Application::Unload();
}

void HelloTriangleBasicApplication::Render()
{
}

```

### Main.cpp
```cpp
#include "HelloTriangleBasicApplication.hpp"

int main(int argc, char* argv[])
{
    HelloTriangleBasicApplication application;
    application.Run();
    return 0;
}
```

To render a triangle we need a shader program, a few vertices and some information to tell the gpu how it all goes together.

Shaders are usually stored in some file on disk somewhere, we wont be embedding them in the source code as a string.

For that let's extend `Application` with a method which allows us to read text files, like shaders.

### Application.hpp
```cpp
#include <string_view>
#include <expected>

// inside the class
protected:
    static std::expected<std::string, std::string> ReadTextFromFile(std::string_view filePath);
```

### Application.cpp
```cpp
#include <fstream>
#include <format>

std::expected<std::string, std::string> Application::ReadTextFromFile(std::string_view filePath)
{
    std::ifstream file(filePath.data(), std::ios::ate);
    if (file.bad())
    {
        return std::unexpected(std::format("Io: Unable to read from file {}", filePath));
    }
    auto fileSize = file.tellg();
    if (fileSize == 0)
    {
        return std::unexpected(std::format("Io: File {} is empty", filePath));
    }

    std::string result(fileSize, '\0');
    file.seekg(0);
    file.read((char*)result.data(), result.size());
    return result;
}
```

I suppose now is a good time to mention how `OpenGL` actually works, at least the bits which are important to
get a piece of geometry on screen.

???+ Note
     
     Explain graphics pipeline in more detail here

Vertices are the base information of a mesh/model some geometry. A Vertex usually contains the position and also
normal and texture coordinates. Normals can be used for lighting calculations and texture coordinates describe how
textures should be stretched across a face/surface, we will also add colors in our first triangle.

I will also call texture coordinates from now on Uvs. You will find that convention in the shader files too later. 
Its just shorter and more concise... Others might call them `tc`, `texcoord`, `texcoords`, `tex_c`, `texture_coordinates` - pain in the ass.

Various tutorials also use plain float arrays to describe vertices, which is another weird thing todo, its like writing [IBAN](https://en.wikipedia.org/wiki/International_Bank_Account_Number) as one thing without spaces.

Lets fix that by introducing an actual construct called `Vertex`. Or specifically a `VertexPositionColor` construct.
Since we will be using position AND color data in our example here to describe a vertex.

Lets create a `VertexPositionColor.hpp` file and fill it with

### VertexPositionColor.hpp
```cpp
#pragma once

#include <glm/vec3.hpp>

struct VertexPositionColor
{
    glm::vec3 Position;
    glm::vec3 Color;
};
```

??? GLM

    I believe I mentioned 'glm' before somewhere. It's the vectormaths library we are going to use. It has types for various constructs
    which come in handy in graphics programming such as vectors, quaternions and matrices.

Lets compare that to something from [LearnOpenGL](https://learnopengl.com)

```cpp
float vertices[] = {
    // positions          // colors           // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
};
```

Awful. Just awful.
In their example they also added uvs into the mix, which is fine. But just formatting this thing to figure out what is what
makes no sense in my opinion. Yes the underlying datatype for all those things is `float`. 

Luckily `GLM`'s vec2/3/4 types are also using floats and code should be readable.

As an exercise, try to "convert" LearnOpenGL's vertex from above into a `VertexPositionColorUv` construct.

Besides a proper vertex format, what else do we need to get a tringle on screen?

Perhaps I should show a picture of a typical/simple graphics pipeline, which consists of a few boxes.

![](https://duriansoftware.com/joe/media/gl1-pipeline-01.png)
(stolen from [here](https://duriansoftware.com/joe/an-intro-to-modern-opengl.-chapter-1:-the-graphics-pipeline) - kind of the only picture which doesnt include obsolete/irrelevant stages and I was too lazy to draw my own version)

As you can see its a few steps, involved to cook up pixels on a screen.

### Input Layout
The first stage is where we send the vertices and indices into the input assembly. That's the place which needs to know how to interpret the vertices and indices. Or in other words
how they are layed out in the buffer. That is the reason why I would like to go with the name of `Input Layout` which makes more sense in my head than what OpenGL calls the construct
to describe the layout. OpenGL calls that thing `Vertex Array Object`, it's also quite often just called `VAO`. VAO Makes no sense in my smal frog brain. Therefore we will go with `Input Layout` or perhaps `IL` from here.

Let's fiddle it into our example project.

We need to declare 3 things in `HelloTriangleBasicApplication.hpp`. 

```cpp
private:
    uint32_t _inputLayout = 0;
    uint32_t _vertexBuffer = 0;
    uint32_t _indexBuffer = 0;
```

All the following code snippets besides the vertex shader code, go right after in `HelloTriangleBasicApplication.cpp`

```cpp
bool HelloTriangleBasicApplication::Load()
{
    if (!Application::Load())
    {
        return false;
    }
```

```cpp
glCreateVertexArrays(1, &_inputLayout);
auto label = "InputLayout_Simple";
glObjectLabel(GL_VERTEX_ARRAY, _inputLayout, label.size(), label.data());

glEnableVertexArrayAttrib(_inputLayout, 0);
glVertexArrayAttribFormat(_inputLayout, 0, 3, GL_FLOAT, GL_FALSE, offsetof(VertexPositionColor, Position));
glVertexArrayAttribBinding(_inputLayout, 0, 0);

glEnableVertexArrayAttrib(_inputLayout, 1);
glVertexArrayAttribFormat(_inputLayout, 1, 3, GL_FLOAT, GL_FALSE, offsetof(VertexPositionColor, Color));
glVertexArrayAttribBinding(_inputLayout, 1, 0);
```

As you can see "VertexArray"this "VertexArray"that. The only "Vertex Array" which makes sense is the buffer containing the actual vertices... which we will come to in a sec.
But try to make sense of this.

We create the input layout first and then we describe its format with the 2 sets of 3 function calls.

I also smuggled in a little helper, `glObjectLabel`, which I will also explain later, I can tell you that much already its a useful tool.

When you squint your eyes a little you can probably see a pattern emerging when looking at the glEnable..glVertexArrayAttrib... calls.
They refer to attribute indices or locations. Like "Attribute Location 0 should be a type with 3 float components and its offset is 0
and "Attribute location 1 should be a type with 3 float components and its offset should be whatever size Position is within the VertexPositionColor construct - probably 3xfloat = 3xsizeof(float = 3x4 = 12)

Thats what the ...AttribFormat call is there for.
The ..AttribBinding defines to what bound vertex buffer that format is related to (we use only 1 vertex buffer here, therefore its 0)
Enable...Attrib as the name suggests enables that attribute for that specific input layout. Attributes can be enabled/disabled dynamically and that is useful when you want to live on the ~~just one Ping Vasily~~ just one VAO for everything path, where you dynamically enable/disable attributes on the fly where needed. I prefer individual input layout objects for my graphics pipelines though.

Do you remember the exercise where I asked you to cook up `VertexPositionColorUv`? Perhaps you can figure out how the input layout should be defined for that format.

Another important thing here is the vertex stage which is the next one after the input assembly. That's where the vertex shader comes into play.
Let me toss the first part of the vertex shader in here to see...

```glsl
#version 460 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_color;

layout (location = 0) out gl_PerVertex
{
...
```

Funny how the vertex shader has VERY similar looking input attributes, neh? Coincidence? No.

You can see here, 2 input attributes. One of type vec3 which reads as "type of float which consist of 3 components" and another one also of "type of float with consists of 3 components".
Literally sounds the same as we just did with the gl...AttribFormat.

The important bit here is, the input layout should match the vertex shader input (the attributes which start with `in`)

The vertex shader also transforms our vertices so that they can appear properly on screen.

!!! note "Coordinate Systems/Vertex Transformation"

    Not sure if I want to explain or steal/link it from/to somewhere else, probably should do the latter

Lets create the vertex and index buffers for our triangle

### VertexBuffer
```cpp
std::array<VertexPositionColor, 3> vertices =
{
    VertexPositionColor{ .Position = glm::vec3(-0.5f, +0.5f, 0.0f), .Color = glm::vec3(1.0f, 0.2f, 1.0f) },
    VertexPositionColor{ .Position = glm::vec3(+0.0f, -0.5f, 0.0f), .Color = glm::vec3(0.2f, 1.0f, 1.0f) },
    VertexPositionColor{ .Position = glm::vec3(+0.5f, +0.5f, 0.0f), .Color = glm::vec3(1.0f, 1.0f, 0.2f) }        
};

glCreateBuffers(1, &_vertexBuffer);
label = "VertexBuffer_Triangle";
glObjectLabel(GL_BUFFER, _vertexBuffer, label.size(), label.data());
glNamedBufferData(_vertexBuffer, vertices.size() * sizeof(VertexPositionColor), vertices.data(), GL_STATIC_DRAW);
```

### IndexBuffer
```cpp
std::array<uint32_t, 3> indices = { 0, 1, 2 };

glCreateBuffers(1, &_indexBuffer);
label = "IndexBuffer_Triangle";
glObjectLabel(GL_BUFFER, _vertexBuffer, label.size(), label.data());
glNamedBufferData(_indexBuffer, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
```

Simple, Triangle has 3 vertices, you create a buffer object and add 3 of them to the vertex buffer, with telling them where to place and how to color them.

Similar to indices. Since we want to draw triangles we need 3 of them, also in their dedicated buffer.

!!! Note "Indices???"

    Indices make vertex reuse possible, by indexing into the vertices instead of providing vertices for each and every vertex. Nice I explained Bratwurst with Bratwurst.
    In this specific case we would not need indices at all, because not a single vertex is or was reused, Indices make sense when you want/can reuse vertices in models
    which is quite often the case.

To make the connection between the buffers and the input layout, since that has to be a thing, we call 2 more functions

```cpp
glVertexArrayVertexBuffer(_inputLayout, 0, _vertexBuffer, 0, sizeof(VertexPositionColor));
glVertexArrayElementBuffer(_inputLayout, _indexBuffer);
```

First call tells the input layout "hey use this buffer for binding 0" (that binding we passed in those `glVertex...AttribBinding` calls, if you remember) and the stride
of the vertices is the size of the vertex we made up, so that the offset part makes sense in those `glVertex..AttribFormat` calls

Second call hooks up the indexbuffer to the input layout.

Within the current folder, create a `Data/Shaders` folder and copy paste the vertex and fragment shader in there

### Simple.vs.glsl
```glsl
#version 460 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_color;

layout (location = 0) out gl_PerVertex
{
    vec4 gl_Position;
};
layout(location = 0) out vec3 v_color;

void main()
{
    gl_Position = vec4(i_position, 1.0);
    v_color = i_color;
}
```

### Simple.fs.glsl
```glsl
#version 460 core

layout(location = 0) in vec3 v_color;

layout(location = 0) out vec4 o_color;

void main()
{
    o_color = vec4(v_color, 1.0f);
}
```

tbd