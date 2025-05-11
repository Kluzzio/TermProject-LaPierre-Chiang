cmake_minimum_required(VERSION 3.10)
project(LaPierre_PA5)

set(CMAKE_CXX_STANDARD 17)

# Source files
set(SOURCES
    camera.cpp
    engine.cpp
    graphics.cpp
    main.cpp
    mesh.cpp
    object.cpp
    shader.cpp
    sphere.cpp
    Texture.cpp
    window.cpp
)

include_directories(
    .
    /opt/X11/include
    /opt/homebrew/opt/assimp/include
    /opt/homebrew/opt/glew/include 
    /opt/homebrew/opt/glfw/include 
    /opt/homebrew/opt/glm/include 
    /usr/local/include 
)

link_directories(
    /opt/X11/lib
    /opt/homebrew/opt/assimp/lib
    /opt/homebrew/opt/glew/lib 
    /opt/homebrew/opt/glfw/lib 
    /opt/homebrew/opt/glm/lib 
    /usr/local/lib                     
)


# Add executable
add_executable(LaPierre_PA5 ${SOURCES})

# Link required libraries
target_link_libraries(LaPierre_PA5
    -lGL
    -lGLU
    -lglut
    -lGLEW  
    -lGLFW  
    /usr/local/lib/libsoil2.a
    assimp

)
# Link macOS CoreFoundation properly
target_link_libraries(LaPierre_PA5
    "-framework CoreFoundation"
)
add_custom_command(TARGET LaPierre_PA5 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:LaPierre_PA5>/assets)
