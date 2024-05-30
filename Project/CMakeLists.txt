# Source files
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${SHADER_SOURCE_DIR}/*.frag"
    "${SHADER_SOURCE_DIR}/*.vert"
    "${SHADER_SOURCE_DIR}/*.tesc"
    "${SHADER_SOURCE_DIR}/*.tese"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
)

set(SOURCES
    "main.cpp"
    "GP2Window.cpp"
    "GP2Base.cpp"
    "GP2Pipeline.cpp"
    "GP2Device.cpp"
    "GP2SwapChain.cpp"
    "GP2Model.cpp"
    "GP2Renderer.cpp"
    "systems/SimpleRenderSystem.cpp"
    "GP2Camera.cpp"
    "KeyboardMovementController.cpp"
    "GP2GameObject.cpp"
    "GP2Buffer.cpp"
    "GP2Descriptors.cpp"
    "GP2Texture.cpp"
    "systems/PointLightSystem.cpp"
    "systems/SimpleRenderSystem2D.cpp"
    "systems/TessellationSystem.cpp")

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${GLSL_SOURCE_FILES})
add_dependencies(${PROJECT_NAME} Shaders)
# Link libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} tinyobjloader)

#Copy models
set(MODELS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/models")
set(MODELS_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/models")

set(TEXTURES_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/textures")
set(TEXTURES_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/textures")

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${MODELS_SOURCE_DIR} ${MODELS_BINARY_DIR}
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${TEXTURES_SOURCE_DIR} ${TEXTURES_BINARY_DIR}
)