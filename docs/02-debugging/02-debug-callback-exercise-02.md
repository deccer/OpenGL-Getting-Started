### Debug Callback Exercise 2

- Go to the debug callback and set a break point at the end of the function
- Go to the main loop and find the `glBindVertexArray(inputLayout);` call.
- Comment it out
- Compile and run

What do you see?

```
[2023-11-23 23:15:39.879] [error] OpenGL Message: 33356 GL_INVALID_OPERATION error generated. Array object is not active.
Source: API
Type: Error
Severity: high
```

Do you remember what I mentioned before that Vertex Array Object is an unfortunate name? It's here again `Array object`. 

Anyway, can you make the connection?

Its an invalid operation to draw something without having an active input layout (VAO/Vertex Array Object).

Solution is to uncomment it. Keep in mind to have at least 1 vao bound. In fact you technically only need one for everything and change the attributes it stands for dynamically as you go.

However we will be using an input layout per vertex type where necessary, its easier to keep track of conceptually and mentally, I think.