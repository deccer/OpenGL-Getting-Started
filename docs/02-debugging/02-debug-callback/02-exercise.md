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

Do you remember what I mentioned before that Vertex Array Object is an unfortunate name? It's here again `Array object`. But thats all you need to know.

Can you make the connection?