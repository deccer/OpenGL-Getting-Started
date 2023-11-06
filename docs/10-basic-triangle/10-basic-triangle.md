# Basic Triangle

Like before in the `HelloWindowApplication` lets setup the files we need first.

### HelloTriangleApplication.hpp
```cpp
```

### HelloTriangleApplication.cpp
```cpp
```

### Main.cpp
```cpp
#include "HelloTriangleApplication.hpp"

int main(int argc, char* argv[])
{
    HelloTriangleApplication application;
    application.Run();
    return 0; // comment
}
```

To render a triangle we need a shader program, a few vertices and some information to tell the gpu how it all goes together.

For that let's extend `Application` with a method which allows us to read text files.

### Application.hpp
```cpp
#include <string_view>

...

protected:
    static std::string ReadTextFromFile(std::string_view filePath);
```

### Application.cpp
```cpp
#include <fstream>

...

std::string Application::ReadTextFromFile(std::string_view filePath);
{
    std::ifstream file(filePath.data(), std::ios::ate);
    std::string result(file.tellg(), '\0');
    file.seekg(0);
    file.read((char*)result.data(), result.size());
    return result;
}
```