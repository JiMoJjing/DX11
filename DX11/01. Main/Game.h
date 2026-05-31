#pragma once

#include <string>


class RenderManager;
class ResourceManager;
class TimeManager;
class InputManager;
class Pipeline;
class Graphics;
class SceneManager;

class Game
{
public:
    Game();
    ~Game();
    
public:
    void Init(HWND hwnd);
    void Update();
    void Render();
    
    shared_ptr<SceneManager> GetSceneManager() { return _sceneManager; }
    shared_ptr<InputManager> GetInputManager() { return _inputManager; }
    shared_ptr<TimeManager> GetTimeManager() { return _timeManager; }   
    shared_ptr<ResourceManager> GetResourceManager() { return _resourceManager; }
    shared_ptr<RenderManager> GetRenderManager() { return _renderManager; }
    
    shared_ptr<Pipeline> GetPipeline() { return _pipeline; }
    
private:
    HWND _hwnd;
    shared_ptr<Graphics> _graphics;
    shared_ptr<Pipeline> _pipeline;
    
    shared_ptr<SceneManager> _sceneManager;
    shared_ptr<InputManager> _inputManager;
    shared_ptr<TimeManager> _timeManager;
    shared_ptr<ResourceManager> _resourceManager;
    shared_ptr<RenderManager> _renderManager;
};

extern unique_ptr<Game> GGame;