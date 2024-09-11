# Game Engine

## Table of Contents

- [Game Engine](#game-engine)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
  - [Actors](#actors)
    - [Actor](#actor)
    - [MeshActor](#meshactor)
    - [PointLight](#pointlight)
  - [Bindables](#bindables)
    - [Bindable](#bindable)
    - [ConstantBuffer](#constantbuffer)
    - [IndexBuffer](#indexbuffer)
    - [InputLayout](#inputlayout)
    - [PixelShader](#pixelshader)
    - [Sampler](#sampler)
    - [Texture](#texture)
    - [VertexBuffer](#vertexbuffer)
    - [VertexShader](#vertexshader)
  - [Components](#components)
    - [Camera](#camera)
    - [MeshComponent](#meshcomponent)
    - [SceneComponent](#scenecomponent)
  - [App](#app)
  - [Utils](#utils)
  - [WICTextureLoader](#wictextureloader)
  - [Graphics](#graphics)
    - [DepthStencilView](#depthstencilview)
  - [Window](#window)
  - [Model](#model)
  - [Material](#material)

## Overview

This project is a rendering engine that incorporates a modular structure with actors, bindables, components, and various shaders. Below is a description of each module and its role in the project.

## Actors

### Actor

The `Actor` class represents an entity that contains a root `SceneComponent` and can be transformed or rendered. It handles transformations such as scaling, rotation, and location, and manages its root component.

### MeshActor

The `MeshActor` class is derived from the `Actor` class and represents an actor that contains a `MeshComponent` as its root. It is used for actors that need to display 3D meshes.

### PointLight

The `PointLight` class is derived from `MeshActor` and represents a point light source in a scene with shadow mapping capabilities.

- **Constructor**:
  - `PointLight(Graphics& graphics, const std::string& fileName, const std::string& actorName = "Point Light")`: Initializes the point light with a mesh and sets up the shadow map camera and constant buffers.

- **Public Methods**:
  - `void SetDiffuseColor(Graphics& graphics, const DirectX::XMFLOAT3 newColor)`: Updates the diffuse color of the light.
  - `void Bind(Graphics& graphics)`: Binds the light and shadow map data to the graphics pipeline.
  - `DirectX::XMMATRIX GetLightPerspective() const noexcept`: Returns the light's perspective matrix for shadow mapping.

- **Private Members**:
  - `LightBuffer lightBuffer`: Holds light color, intensity, and position.
  - `ShadowMapBuffer shadowMapBuffer`: Stores the light's perspective matrix for shadow mapping.
  - `Camera* shadowMapCamera`: A camera used for shadow map rendering.
  - `std::unique_ptr<ConstantBuffer<LightBuffer>> constantLightBuffer`: Constant buffer for the light properties.
  - `std::unique_ptr<ConstantBuffer<ShadowMapBuffer>> constantShadowMapBuffer`: Constant buffer for the shadow map data.

## Bindables

### Bindable

The `Bindable` class serves as a base class for objects that can be bound to the graphics pipeline. It provides methods for binding the object to the GPU, updating its state, and retrieving its identifier.

- **`Bind(Graphics& graphics) noexcept`**: Pure virtual method that must be implemented by derived classes to bind the object to the graphics pipeline.
- **`Update(Graphics& graphics)`**: Virtual method that can be overridden to update the object’s state in the graphics pipeline (default implementation does nothing).
- **`GetID() const`**: Returns the identifier of the object. Throws a runtime error if the ID is not set and the object is used with a shared pool.
- **`GetContext(Graphics& graphics) noexcept`**: Static method to get the Direct3D device context from the `Graphics` object.
- **`GetDevice(Graphics& graphics) noexcept`**: Static method to get the Direct3D device from the `Graphics` object.
- **Destructor**: Virtual and default, allowing for proper cleanup of derived classes.

### ConstantBuffer

The `ConstantBuffer` class handles a Direct3D 11 constant buffer for passing data to shaders. It supports both pixel and vertex shader buffers.

- **Constructor**: Initializes the buffer with data, type, and slot.
- **`Update(Graphics& graphics)`**: Updates the buffer's data.
- **`Bind(Graphics& graphics) noexcept`**: Binds the buffer to the specified shader stage.

**Private Members**:

- **`constantBuffer`**: Direct3D buffer resource.
- **`bufferData`**: Pointer to the data.
- **`slot`**: Shader slot.
- **`bufferType`**: Buffer type (pixel or vertex).

### IndexBuffer

The `IndexBuffer` class manages a Direct3D 11 index buffer, used for indexing vertices in rendering.

- **Constructor**: Initializes the index buffer with a list of indices.
- **`Bind(Graphics& graphics) noexcept`**: Binds the index buffer to the input assembler stage.
- **`GetIndicesNumber() const noexcept`**: Returns the number of indices in the buffer.

**Private Members**:

- **`indicesNum`**: Number of indices.
- **`indexBuffer`**: Direct3D buffer resource.

### InputLayout

The `InputLayout` class manages a Direct3D 11 input layout, which describes the vertex data format.

- **Constructor**: Initializes the input layout with element descriptions and shader data.
- **`Bind(Graphics& graphics) noexcept`**: Binds the input layout to the input assembler stage.

**Private Members**:

- **`inputLayout`**: Direct3D input layout resource.

### PixelShader

The `PixelShader` class handles a Direct3D 11 pixel shader, used for shading pixels in rendering.

- **Constructor**: Initializes the pixel shader from a file path.
- **`Bind(Graphics& graphics) noexcept`**: Binds the pixel shader to the pixel shader stage.

### Sampler

The `Sampler` class manages a Direct3D 11 sampler state, which controls how textures are sampled.

- **Constructor**: Initializes the sampler state with a given description and slot.
- **`Bind(Graphics& graphics) noexcept`**: Binds the sampler state to the pixel shader stage at the specified slot.

**Private Members**:

- **`slot`**: The slot in the shader pipeline where the sampler will be bound.
- **`sampler`**: Direct3D sampler state resource.

### Texture

The `Texture` class manages a Direct3D 11 texture resource for use in shaders.

- **Constructor**: Loads a texture from a file and initializes it with a specified slot.
- **`Bind(Graphics& graphics) noexcept`**: Binds the texture to the pixel shader stage at the specified slot.

**Private Members**:

- **`slot`**: The slot in the shader pipeline where the texture will be bound.
- **`textureView`**: Direct3D shader resource view for the texture.

### VertexBuffer

The `VertexBuffer` class handles a Direct3D 11 vertex buffer for storing vertex data.

- **Constructor**: Initializes the vertex buffer with data and vertex size.
- **`Bind(Graphics& graphics) noexcept`**: Binds the vertex buffer to the input assembler stage.

**Private Members**:

- **`vertexBuffer`**: Direct3D vertex buffer resource.
- **`vertexSize`**: Size of each vertex in bytes.

### VertexShader

The `VertexShader` class manages a Direct3D 11 vertex shader, used for vertex processing.

- **Constructor**: Loads and initializes the vertex shader from a file.
- **`Bind(Graphics& graphics) noexcept`**: Binds the vertex shader to the vertex shader stage.
- **`GetBufferPointer() const noexcept`**: Returns a pointer to the shader bytecode.
- **`GetBufferSize() const noexcept`**: Returns the size of the shader bytecode.

**Private Members**:

- **`vertexShader`**: Direct3D vertex shader resource.
- **`blob`**: Shader bytecode.

## Components

### Camera

The `Camera` class represents a camera component in a 3D scene, providing functionality for view matrix calculation and camera control.

- **`CreateComponent(const std::string& componentName = "Camera")`**: Creates a new `Camera` instance with an optional name.
- **`GetMatrix() const noexcept`**: Returns the view matrix of the camera.
- **`AddMovementInput(const DirectX::XMFLOAT2 moveVector) noexcept`**: Moves the camera based on input vectors. Positive X moves right, negative X moves left, negative Y moves backward, and positive Y moves forward.
- **`AddPitchInput(const float pitchInput) noexcept`**: Adjusts the camera's pitch (vertical rotation).
- **`AddYawInput(const float yawInput) noexcept`**: Adjusts the camera's yaw (horizontal rotation).

**Private Members**:

- **`movementSpeed`**: Speed of the camera's movement.
- **`rotationSpeed`**: Speed of the camera's rotation.

### MeshComponent

`MeshComponent` represents a 3D model component with mesh data and material properties in a scene.

- **`CreateComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene)`**: Factory method to create a `MeshComponent` instance.
- **`Draw(Graphics& graphics)`**: Renders the mesh with its material.
- **`RenderShadowMap(Graphics& graphics)`**: Renders the mesh for shadow mapping if using Phong shading.
- **`GetMaterial() noexcept`**: Returns a pointer to the material associated with the mesh.
- **`RenderComponentDetails(Gui& gui) override`**: Renders details of the component in the GUI.
- **`UpdateTransformBuffer(Graphics& graphics)`**: Updates the transform buffer with the current transform, view, and projection matrices.

**Private Members**:

- **`model`**: The 3D model.
- **`material`**: The material for the model.
- **`nullPixelShader`**: Shader used for shadow mapping.
- **`modelForShadowMapping`**: Model used for shadow mapping.
- **`transformConstantBuffer`**: Buffer for transform matrices.
- **`transformBuffer`**: Struct holding transform matrices.
- **`usesPhong`**: Boolean indicating if Phong shading is used.

### SceneComponent

`SceneComponent` represents a component in a 3D scene, with transformations and hierarchy management.

- **`CreateComponent(const std::string& componentName = "Scene Component")`**: Factory method to create a `SceneComponent` instance.
- **`CreateComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene)`**: Creates a `SceneComponent` from Assimp node and scene data.
- **`LoadComponent(Graphics& graphics, const std::string& fileName)`**: Loads and creates a `SceneComponent` from a file.
- **`Draw(Graphics& graphics)`**: Draws the component and its children.
- **`RenderShadowMap(Graphics& graphics)`**: Renders shadow maps for the component and its children.
- **`AttachComponents<T>(std::unique_ptr<SceneComponent> child, SceneComponent* const newParent)`**: Attaches a child component to a new parent and returns the child cast to `T`.
- **`RenderComponentDetails(Gui& gui)`**: Renders details of the component in the GUI.

- **`DeattachFromParent()`**: Detaches the component from its parent.

**Members**:

- **`children`**: List of child components.
- **`parent`**: Pointer to the parent component.
- **`relativeLocation`**: Location relative to the parent.
- **`relativeRotation`**: Rotation relative to the parent.
- **`relativeScale`**: Scale relative to the parent.
- **`componentName`**: Name of the component.

## App

The `App` class manages the main application loop and scene rendering. It initializes and maintains the window, handles user input, and manages various actors including 3D models and lights. It also controls camera movement and orientation, and integrates shadow mapping and regular scene rendering.

- **`Run()`**: Main loop of the application. Handles window messages, processes input for camera movement, updates actor positions, and renders both shadow maps and the regular scene. It also manages GUI rendering for actor trees and control windows.

## Utils

The `Utils` namespace provides utility functions for converting between `std::wstring` (wide string) and `std::string`. These conversions are essential for handling string operations across different encoding formats (e.g., when working with Windows APIs or Unicode).

## WICTextureLoader

## Graphics

The `Graphics` class encapsulates DirectX 11 rendering functionality, handling the swap chain, device, rendering targets, and viewports. It supports shadow mapping, indexed drawing, and frame rendering, as well as managing GUI integration.

- **Constructor**: Initializes the DirectX 11 swap chain, device, context, and related resources such as depth stencils and shadow maps.
- **`BeginFrame()`**: Clears render targets and prepares the frame for drawing.
- **`SetRenderTargetForShadowMap()`**: Sets the render target for shadow map rendering.
- **`SetNormalRenderTarget()`**: Reverts to the normal render target for scene rendering.
- **`DrawIndexed(size_t numIndices)`**: Draws indexed geometry using the number of specified indices.
- **`EndFrame()`**: Finalizes the frame and presents it on the screen.
- **`SetProjection()` / `GetProjection()`**: Sets or retrieves the current projection matrix.
- **`SetCamera()` / `GetCamera()`**: Sets or retrieves the current camera matrix.
- **`GetGui()`**: Provides access to the GUI system.

### DepthStencilView

The `DepthStencilView` class encapsulates the creation and management of a depth stencil view and its associated texture in DirectX 11.

- **Constructor**: Creates a 2D texture and its corresponding depth stencil view using provided DirectX descriptors.
- **`Get()`**: Returns the raw pointer to the `ID3D11DepthStencilView`.
- **`GetTexture()`**: Returns the raw pointer to the underlying `ID3D11Texture2D`.

## Window

The `Window` class manages the creation and lifecycle of a window, handling input events like key presses, cursor control, and raw input. It interacts with the `Graphics` class to manage Direct3D rendering within the window. Additionally, it offers utility functions to read key events, manage the cursor, and process Windows messages efficiently. The class is not copyable and manages its own resources.

## Model

The `Model` class represents a 3D model loaded from an `aiMesh` and contains vertex and index buffers, along with shader bindings for rendering.

- **Constructor**:
  - `Model(Graphics& graphics, const aiMesh* const assignedMesh, const bool hasTexture, const bool usesPhong, std::shared_ptr<IndexBuffer> givenIndexBuffer = nullptr)`: Initializes the model with vertex data, index buffer, and shader bindings.

- **Public Methods**:
  - `void Bind(Graphics& graphics) noexcept`: Binds the model's vertex, index buffers, and shaders to the graphics pipeline.
  - `std::shared_ptr<IndexBuffer> ShareIndexBuffer() noexcept`: Returns the shared index buffer.
  - `size_t GetIndicesNumber() const noexcept`: Returns the number of indices in the model.

- **Private Members**:
  - `std::vector<float> verticesData`: Holds the model's vertex data.
  - `std::vector<unsigned int> indices`: Contains the model's index data.
  - `std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs`: Describes the input layout for the vertex data.
  - `std::vector<std::unique_ptr<Bindable>> bindables`: Stores bindable resources like vertex buffers.
  - `std::vector<std::shared_ptr<Bindable>> sharedBindables`: Stores shared bindable resources like shaders and input layouts.
  - `std::shared_ptr<IndexBuffer> indexBuffer`: The model's index buffer for rendering.
  - `enum VertexElement`: Defines the types of vertex elements (Position, Normal, TexCoords).
  - `std::unordered_map<VertexElement, unsigned int> elementOffset`: Maps vertex elements to their offsets in the vertex buffer.

## Material

The `Material` class manages shaders and textures for rendering an object's material properties.

- **Constructor**:
  - `Material(Graphics& graphics, const aiMaterial* const assignedMaterial, const bool usesPhong)`: Initializes the material with textures or colors and assigns the appropriate pixel shader.

- **Public Methods**:
  - `void Bind(Graphics& graphics) noexcept`: Binds the material's textures, shaders, and constants to the graphics pipeline.

- **Private Members**:
  - `std::vector<std::unique_ptr<Bindable>> bindables`: Holds the material's unique bindable resources (e.g., constant buffers, samplers).
  - `std::vector<std::shared_ptr<Bindable>> sharedBindables`: Holds shared resources like shaders and textures.
  - `struct Roughness { float roughness = 0.8f; } roughnessBuffer`: Stores the roughness value for the material.
  - `struct Color { DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f }; }`: Stores the material's color.
  - `std::unique_ptr<Color> colorBuffer`: Holds the color constant buffer for materials without textures.
