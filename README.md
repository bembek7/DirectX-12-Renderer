# Game Engine

## Table of Contents

- [Game Engine](#game-engine)
  - [Table of Contents](#table-of-contents)
  - [How to run](#how-to-run)
  - [Overview](#overview)
  - [Technology used](#technology-used)
  - [Project structure](#project-structure)
    - [App](#app)
    - [Window](#window)
    - [Graphics](#graphics)
    - [Passes](#passes)
      - [Pass](#pass)
      - [RegularDrawingPass](#regulardrawingpass)
      - [ShadowMapPass](#shadowmappass)
    - [Scene](#scene)
    - [Actors](#actors)
      - [Actor](#actor)
      - [MeshActor](#meshactor)
      - [PointLight](#pointlight)
    - [Components](#components)
      - [SceneComponent](#scenecomponent)
      - [MeshComponent](#meshcomponent)
      - [Camera](#camera)
    - [Material](#material)
    - [Model](#model)
    - [Bindables](#bindables)
      - [Bindable](#bindable)
      - [Blender](#blender)
      - [ConstantBuffer](#constantbuffer)
      - [CubeTexture](#cubetexture)
      - [DepthCubeTexture](#depthcubetexture)
      - [DepthStencilState](#depthstencilstate)
      - [DepthStencilView](#depthstencilview)
      - [IndexBuffer](#indexbuffer)
      - [InputLayout](#inputlayout)
      - [PixelShader](#pixelshader)
      - [Rasterizer](#rasterizer)
      - [ShadowRasterizer](#shadowrasterizer)
      - [RenderTargetView](#rendertargetview)
      - [Sampler](#sampler)
      - [Texture](#texture)
      - [VertexBuffer](#vertexbuffer)
      - [VertexShader](#vertexshader)
      - [Viewport](#viewport)
    - [BindablesPool](#bindablespool)
    - [ShaderSettings](#shadersettings)
    - [Skybox](#skybox)
    - [Gui](#gui)

## How to run

Just clone the solution and run it in x64.

## Overview

This project is a simple rendering engine done in DirectX 11. The engine suppports phong lighting and dynamic shadows(currently with only one light source). Below is a brief description of used classes and their more complicated methods or members.

Demo video:

[![Alt text](https://img.youtube.com/vi/nv6an-7eBww/0.jpg)](https://youtu.be/nv6an-7eBww).

## Technology used

- **DirectX 11**
- **DirectXTex** - for loading textures from image files
- **Assimp library** - for loading meshes and materials
- **Dear ImGui** - for gui

## Project structure

### App

The `App` class represents the main application logic, including the game loop, scene initialization, and input handling.

- **Methods**:
  - `int Run()`: Starts the main loop, initializes the scene, processes input, and handles frame rendering. Returns an exit code.
  - `void HandleInput()`: Processes keyboard and mouse inputs for controlling the camera, toggling cursor visibility, and handling raw input.
  - `void InitializeScene()`: Sets up the initial scene by loading 3D objects (actors) and lighting, and positioning them within the scene.

- **Members**:
  - `std::unique_ptr<Scene> scene`: A smart pointer to the `Scene` object which manages all actors, lighting, and rendering within the scene.
  - `Window window = Window(1444, 810)`: Represents the application window, initialized with dimensions 1444x810, and provides access to rendering and input systems.

### Window

Manages the graphical window, input handling, cursor control, and raw input for the application.

- **Methods**:
  - `Window(unsigned int clientAreaWidth = 800, unsigned int clientAreaHeight = 600)`: Initializes the window and sets up graphics.
  - `static std::optional<int> ProcessMessages()`: Processes window events, returns exit code if closed.
  - `std::optional<short> ReadPressedKey() noexcept`: Retrieves the most recent key press.
  - `Graphics& GetGraphics() noexcept`: Accesses the graphics object.
  - `void EnableRawInput() noexcept / DisableRawInput() noexcept`: Toggles raw input mode which is used for FPV camera.
  - `std::optional<std::pair<int, int>> ReadRawDelta() noexcept`: Returns raw mouse input delta if available.

- **Private Methods**:
  - `void FreeCursor() noexcept / LockCursorToClientArea() noexcept`: Manages cursor lock state.
  - `LRESULT CALLBACK WindowProcBeforeCreation(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)`: Pre-creation window message handler, that stores the window objects in lParam and changes message handler to after creation one.
  - `LRESULT CALLBACK WindowProcAfterCreation(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)`: Post-creation window message handler that redirects message to `HandleMassage`.
  - `LRESULT HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)`: Main message handler for window events.

- **Members**:
  - `HWND hWnd`: Window handle.
  - `std::unique_ptr<Graphics> graphics`: Graphics engine tied to the window.
  - `std::queue<short> keyPressedEvents`: Queue for key press events.
  - `std::queue<std::pair<int, int>> rawDeltaEvents`: Queue for raw mouse input events.
  - `std::array<bool, 255> keys`: Tracks the state of each key.
  - `static constexpr char winClassName[] = "RythmWindow"`: Window class name.

### Graphics

Manages DirectX 11 rendering, handles window-related graphics setup, and facilitates drawing operations.

- **Methods**:
  - `Graphics(HWND hWnd, float windowWidth, float windowHeight)`: Initializes DirectX 11 components, including the swap chain and device.
  - `void BeginFrame() noexcept`: Prepares for a new frame calling begin frame on gui.
  - `void EndFrame()`: Finalizes the frame by ending ImGui rendering and presenting the swap chain.
  - `void DrawIndexed(size_t numIndices) noexcept`: Executes indexed drawing with the specified number of indices.
  - `void BindCurrentRenderTarget()`: Binds the current render target and depth stencil view for rendering.
  - `void ClearRenderTargetBinds()`: Clears any render targets and depth stencil bindings.

- **Members**:
  - `ComPtr<IDXGISwapChain> swapChain`: The swap chain used to handle back buffers and presenting.
  - `ComPtr<ID3D11Device> device`: DirectX 11 device for resource creation.
  - `ComPtr<ID3D11DeviceContext> context`: Device context for managing the rendering pipeline.
  - `ComPtr<ID3D11RenderTargetView> currentRenderTargetView`: The current render target view to be bound for rendering.
  - `ComPtr<ID3D11DepthStencilView> currentDepthStencilView`: The current depth stencil view to be bound for depth testing.
  - `std::unique_ptr<Gui> gui`: Manages the graphical user interface using ImGui.
  - `float windowWidth`: Width of the application window client area.
  - `float windowHeight`: Height of the application window client area.
  - `XMFLOAT4X4 camera`: The current camera matrix.
  - `XMFLOAT4X4 projection`: The current projection matrix.

### Passes

Techniques that specify how objects are drawn. They create bindables required for specific technique and execute appropriate behavior for every actor in the scene.

#### Pass

Represents a rendering pass that can execute a sequence of rendering operations using bindables.

- **Methods**:
  - `void Execute(Graphics& graphics)`: Executes the rendering pass, updating and binding all associated bindables, and binding projection.

- **Members**:
  - `std::vector<std::unique_ptr<Bindable>> bindables`: Holds unique bindable objects for rendering.
  - `std::vector<std::shared_ptr<Bindable>> sharedBindables`: Holds shared bindable.
  - `DirectX::XMFLOAT4X4 projection`: Stores the projection matrix for the rendering pass.

#### RegularDrawingPass

Handles the execution of a regular drawing pass.

#### ShadowMapPass

Handles the execution of shadow mapping for point lights, including setup for rendering shadow maps.

- **Methods**:
  - `void Execute(Graphics& graphics, const std::vector<std::shared_ptr<Actor>>& actors, PointLight* const pointLight)`: Executes the shadow mapping process, rendering depth buffers to cube from the perspective of the point light.

- **Members**:
  - `std::shared_ptr<DepthCubeTexture> shadowMapCube`: Holds the depth cube texture used for shadow mapping.

### Scene

The `Scene` class manages actors, lights, and rendering passes within a graphics scene.

- **Methods**:
  - `Scene(Graphics& graphics)`: Initializes the scene with a main camera and sets up shadow and drawing passes.
  - `void AddActor(std::shared_ptr<Actor> actorToAdd)`: Adds an actor to the scene, ensuring it is not a `PointLight`.
  - `void AddLight(std::shared_ptr<PointLight> lightToAdd)`: Adds a `PointLight` to the scene, enforcing a single light restriction.
  - `void AddSkybox(std::unique_ptr<Skybox> skyboxToAdd)`: Adds a skybox to the scene, allowing only one skybox.
  - `void Draw(Graphics& graphics)`: Executes the shadow map and drawing passes, and renders the skybox last.
  - `void RenderControls(Graphics& graphics)`: Renders the GUI controls for each actor in the scene.
  - `Camera* GetMainCamera()`: Returns a pointer to the main camera used in the scene.

- **Members**:
  - `std::unique_ptr<RegularDrawingPass> drawingPass`: Pointer to the regular drawing pass for rendering.
  - `std::unique_ptr<ShadowMapPass> shadowMapPass`: Pointer to the shadow map pass for shadow rendering.
  - `std::vector<std::shared_ptr<Actor>> actors`: Collection of actors in the scene.
  - `std::shared_ptr<PointLight> light`: Pointer to the single point light in the scene.
  - `std::unique_ptr<Skybox> skybox`: Pointer to the skybox in the scene.
  - `std::unique_ptr<Camera> mainCamera`: Pointer to the main camera used for rendering.

### Actors

#### Actor

The `Actor` class represents an entity that contains a root `SceneComponent` and can be transformed or rendered. It handles transformations such as scaling, rotation, and location, and manages its root component.

#### MeshActor

The `MeshActor` class is derived from the `Actor` class and represents an actor that contains a `MeshComponent` as its root. It is used for actors that need to display 3D meshes.

#### PointLight

The `PointLight` class is derived from `MeshActor` and represents a point light source in a scene with shadow mapping capabilities.

- **Methods**:
  - `PointLight(Graphics& graphics, const std::string& fileName, const std::string& actorName = "Point Light")`: Initializes the point light with a mesh and sets up the shadow map camera and constant buffers.

- **Members**:
  - `LightBuffer lightBuffer`: Holds light color, intensity, and position.
  - `ShadowMapBuffer shadowMapBuffer`: Stores the light's perspective matrix for shadow mapping.
  - `Camera* shadowMapCamera`: A camera used for shadow map rendering.
  - `std::unique_ptr<ConstantBuffer<LightBuffer>> constantLightBuffer`: Constant buffer for the light properties.
  - `std::unique_ptr<ConstantBuffer<ShadowMapBuffer>> constantShadowMapBuffer`: Constant buffer for the shadow map data.

### Components

#### SceneComponent

The `SceneComponent` class represents a component in the scene hierarchy, handling transformations (scale, rotation, location) and rendering.

- **Methods**:
  - `static std::unique_ptr<SceneComponent> CreateComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene)`: Creates a new scene component out of loaded assimp structs.
  - `static std::unique_ptr<SceneComponent> LoadComponent(Graphics& graphics, const std::string& fileName)`: Loads a component from a file using assimp.
  - `template <typename T> static T* AttachComponents(std::unique_ptr<SceneComponent> child, SceneComponent* const newParent)`: Attaches a child component to a parent.
  - `void Draw(Graphics& graphics)`: Renders the component and its children.
  - `void RenderShadowMap(Graphics& graphics)`: Renders the shadow map for the component and its children.
  - `std::string GetComponentFullName()`: Returns the full name of the component, which is component name and it's class(for gui use).

- **Members**:
  - `std::vector<std::unique_ptr<SceneComponent>> children`: Holds the child components.
  - `SceneComponent* parent = nullptr`: Pointer to the parent component.
  - `DirectX::XMFLOAT3 relativeLocation = { 0.f, 0.f, 0.f }`: Stores the relative location of the component.
  - `DirectX::XMFLOAT3 relativeRotation = { 0.f, 0.f, 0.f }`: Stores the relative rotation of the component.
  - `DirectX::XMFLOAT3 relativeScale = { 1.f, 1.f, 1.f }`: Stores the relative scale of the component.
  - `std::string componentName`: Name of the component.

#### MeshComponent

The `MeshComponent` class represents a mesh within a scene and handles drawing, shadow mapping, and material management.

- **Methods**:
  - `ShaderSettings ResolveShaderSettings(const aiMesh* const mesh, const aiMaterial* const material)`: Analyzes the given mesh and material to determine the appropriate shader settings. This function checks for texture, normal, and specular maps and applies the relevant flags.

- **Members**:
  - `std::unique_ptr<Model> model`: The main model for rendering.
  - `std::unique_ptr<Material> material`: The material applied to the mesh.
  - `std::unique_ptr<Model> modelForShadowMapping`: Model used for rendering shadows.
  - `struct TransformBuffer`: Holds the transformation matrices for rendering.
  - `std::unique_ptr<ConstantBuffer<TransformBuffer>> transformConstantBuffer`: Constant buffer for transformation matrices.

#### Camera

The `Camera` class represents a camera component with movement and rotation controls.

- **Methods**:
  - `DirectX::XMMATRIX GetMatrix() const noexcept`: Returns the view matrix based on the camera's position and orientation.
  - `void AddMovementInput(const DirectX::XMFLOAT2 moveVector) noexcept`: Adds movement input in the X (left/right) and Y (forward/backward) directions.
  - `void AddPitchInput(const float pitchInput) noexcept`: Adjusts the camera's pitch (up/down rotation).
  - `void AddYawInput(const float yawInput) noexcept`: Adjusts the camera's yaw (left/right rotation).

- **Members**:
  - `const float movementSpeed = 0.6f`: Movement speed of the camera.
  - `const float rotationSpeed = 0.15f`: Rotation speed of the camera.

### Material

The `Material` class represents a material which bindable resource and variables are established on shader settings resolved by mesh component holding the material.

- **Methods**:
  - `Material(Graphics& graphics, const aiMaterial* const assignedMaterial, ShaderSettings shaderSettings)`: Constructs a material with specified shader settings and textures.
  - `void Bind(Graphics& graphics) noexcept`: Binds the material's shaders and resources to the graphics pipeline.

- **Members**:
  - `std::vector<std::unique_ptr<Bindable>> bindables`: List of unique bindables for the material.
  - `std::vector<std::shared_ptr<Bindable>> sharedBindables`: List of shared bindables used by the material.
  - `struct Roughness`: Contains the roughness value used for Phong shading.
  - `struct Color`: Contains the material color in RGBA format.
  - `std::unique_ptr<Roughness> roughnessBuffer`: Holds the roughness value for Phong shading.
  - `std::unique_ptr<Color> colorBuffer`: Holds the color data for solid color materials.
  - `static const std::unordered_map<ShaderSettings, std::wstring, ShaderSettingsHash> psPaths`: Maps shader settings to pixel shader file paths.

### Model

The `Model` class represents a 3D model. It establishes correct input layout from shader settings, and save vertex data in vector of floats using accurate offsets from map.

- **Methods**:
  - `Model(Graphics& graphics, const aiMesh* const assignedMesh, ShaderSettings shaderSettings, std::shared_ptr<IndexBuffer> givenIndexBuffer = nullptr)`: Constructs a model from a mesh with the provided shader settings and optional index buffer.
  - `void Bind(Graphics& graphics) noexcept`: Binds the model's vertices, indices, and shaders to the graphics pipeline.
  - `std::shared_ptr<IndexBuffer> ShareIndexBuffer() noexcept`: Returns the model's shared index buffer.
  - `size_t GetIndicesNumber() const noexcept`: Returns the number of indices in the model.

- **Members**:
  - `std::vector<float> verticesData`: Stores the vertex data of the model.
  - `unsigned int vertexSize`: Size of a single vertex.
  - `std::vector<unsigned int> indices`: Stores the model's index data.
  - `std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs`: Describes the input layout for the vertex buffer.
  - `std::vector<std::unique_ptr<Bindable>> bindables`: List of unique bindables for the model.
  - `std::vector<std::shared_ptr<Bindable>> sharedBindables`: List of shared bindables for the model.
  - `std::shared_ptr<IndexBuffer> indexBuffer`: Shared index buffer for the model.
  - `enum class VertexElement`: Defines the different vertex elements (Position, Normal, TexCoords, Tangent, Bitangent).
  - `std::unordered_map<VertexElement, unsigned int> elementOffset`: Maps vertex elements to their offsets in the vertex data.
  - `static const std::unordered_map<ShaderSettings, std::wstring, ShaderSettingsHash> vsPaths`: Maps shader settings to vertex shader file paths.

### Bindables

Bindables are objects that can be bound to the graphics pipeline, some bindables can be shared to save resources and then they implement static method that resolves their identifier.

#### Bindable

The `Bindable` class serves as a base class that provides virtual methods for binding the object to the GPU, and updating its state.

- **Methods**:
  - `virtual void Bind(Graphics& graphics) noexcept`: Pure virtual method that must be implemented by derived classes to bind the object to the graphics pipeline.
  - `virtual void Update(Graphics& graphics)`: Virtual method that can be overridden to update the object’s state in the graphics pipeline (default implementation does nothing).
  - `static ID3D11DeviceContext* GetContext(Graphics& graphics) noexcept`: Static method to get the Direct3D device context from the `Graphics` object.
  - `static ID3D11Device* GetDevice(Graphics& graphics) noexcept`: Static method to get the Direct3D device from the `Graphics` object.
  - `static IDXGISwapChain* GetSwapChain(Graphics& graphics) noexcept`: Static method to get the Direct3D swap chain from the `Graphics` object.

#### Blender

The `Blender` class manages a Direct3D 11 blend state for controlling how pixel colors are blended in the rendering pipeline, enabling or disabling blending based on specified settings.

#### ConstantBuffer

The `ConstantBuffer` class handles a Direct3D 11 constant buffer for passing data to shaders. It supports both pixel and vertex shader buffers.

#### CubeTexture

The `CubeTexture` class handles loading and managing a cube texture in Direct3D 11, providing functionality to bind the texture to the rendering pipeline for use in shaders.

#### DepthCubeTexture

The `DepthCubeTexture` class encapsulates a cube texture specifically for depth storage in Direct3D 11, providing functionalities for binding, clearing, and accessing depth buffers for each face of the cube.

#### DepthStencilState

The `DepthStencilState` class encapsulates the creation and management of depth stencil states in Direct3D 11, allowing for different configurations based on usage types such as regular rendering and skybox rendering.

#### DepthStencilView

The `DepthStencilView` class encapsulates depth stencil views in Direct3D 11, facilitating depth and stencil operations for rendering, including creation and binding of views based on specified usage types and clearing the view as needed.

#### IndexBuffer

The `IndexBuffer` class encapsulates a Direct3D 11 index buffer, used for indexing vertices in rendering.

#### InputLayout

The `InputLayout` class encapsulates a Direct3D 11 input layout, which describes the vertex data format.

#### PixelShader

The `PixelShader` class encapsulates a Direct3D 11 pixel shader.

#### Rasterizer

The `Rasterizer` class configures rasterization states in Direct3D 11, allowing the specification of culling modes.

#### ShadowRasterizer

The `ShadowRasterizer` class extends the rasterization state configuration for shadow mapping in Direct3D 11, allowing for adjustable culling modes and depth bias settings to improve shadow rendering accuracy.

#### RenderTargetView

The `RenderTargetView` class represents a render target in Direct3D 11, allowing rendering operations to be performed on a back buffer, including binding and clearing functionalities.

#### Sampler

The `Sampler` class encapsulates a Direct3D 11 sampler state.

#### Texture

The `Texture` class manages a Direct3D 11 texture resource for use in shaders. For now it is also responsible for loading the texture from file.

#### VertexBuffer

The `VertexBuffer` class encapsulates a Direct3D 11 vertex buffer for storing vertex data.

#### VertexShader

The `VertexShader` class encapsulates a Direct3D 11 vertex shader.

#### Viewport

The `Viewport` class encapsulates a Direct3D 11 viewport.

### BindablesPool

The `BindablesPool` class manages a pool of bindable resources in Direct3D 11, ensuring efficient reuse of objects by storing them in a map and creating them on demand based on unique identifiers.

### ShaderSettings

The `ShaderSettings` enum class defines various shader configurations using bit flags for features like Phong shading, texture mapping, normal mapping, and alpha testing. The `ShaderSettingsHash` struct provides a hash function for the enum values, enabling their use in maps in `Model` and `Material` class.

### Skybox

The `Skybox` class is responsible for rendering a skybox in a 3D environment.

- **Methods**:
  - `Skybox(Graphics& graphics, const std::string& fileName)`: Initializes the skybox by loading the 3D model and material from the specified file. Sets up the constant buffer and depth stencil state for rendering the skybox.
  - `void Draw(Graphics& graphics)`: Updates the transformation matrix for the skybox, binds the model, material, and other necessary components, and renders the skybox.

- **Members**:
  - `struct TransformBuffer`: Holds the transformation matrix (`viewProjection`) for the skybox.
  - `TransformBuffer transformBuffer`: Instance of `TransformBuffer` that stores the skybox’s transformation data.
  - `std::unique_ptr<ConstantBuffer<TransformBuffer>> transformConstantBuffer`: Constant buffer for the skybox’s transformation matrix.
  - `std::unique_ptr<Model> model`: The skybox’s 3D model.
  - `std::unique_ptr<Material> material`: The skybox’s material.
  - `std::unique_ptr<DepthStencilState> skyDepthStencilState`: Depth stencil state used to render the skybox properly.

### Gui

The `Gui` class provides an interface for rendering graphical user interface elements using ImGui within a DirectX context.

- **Methods**:
  - `Gui(const HWND& hWnd, ID3D11Device* const device, ID3D11DeviceContext* const context)`: Initializes the ImGui context and sets up the necessary bindings for DirectX and Windows.
  - `~Gui()`: Cleans up the ImGui context and releases resources.
  - `void BeginFrame()`: Prepares ImGui for a new frame, calling necessary initialization functions.
  - `void EndFrame()`: Renders the ImGui frame and displays the drawn elements.
  - `void RenderActorTree(Actor* const actor)`: Displays the tree view of the specified actor and its components.
  - `void RenderControlWindow()`: Renders the control window showing details about the selected actor and component.
  - `void RenderComponentDetails(SceneComponent* const component)`: Displays editable fields for the properties of a `SceneComponent`.
  - `void RenderComponentDetails(MeshComponent* const component)`: Displays editable fields for the properties of a `MeshComponent`.
  - `void RenderActorDetails(Actor* const actor)`: Displays basic details about the specified actor.
  - `void RenderActorDetails(PointLight* const actor)`: Displays detailed properties of a `PointLight` actor.
  - `void RenderComponentTree(SceneComponent* const component, Actor* const actor)`: Recursively renders the component tree for the specified `SceneComponent`, allowing selection of components.

- **Members**:
  - `Actor* selectedActor`: Pointer to the currently selected actor in the GUI.
  - `SceneComponent* selectedComponent`: Pointer to the currently selected component in the GUI.
