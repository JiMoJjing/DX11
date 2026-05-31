#include "pch.h"
#include "Game.h"

#include "00. Engine/Component/Camera/Camera.h"
#include "00. Engine/Component/Rendering/MeshRenderer.h"
#include "00. Engine/Manager/Input/InputManager.h"
#include "00. Engine/Manager/Render/RenderManager.h"
#include "00. Engine/Manager/Resource/ResourceManager.h"
#include "00. Engine/Manager/Scene/SceneManager.h"
#include "00. Engine/Manager/Time/TimeManager.h"

unique_ptr<Game> GGame = make_unique<Game>();

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(HWND hwnd)
{
    _hwnd = hwnd;
    
    _graphics = make_shared<Graphics>(hwnd);
    _pipeline = make_shared<Pipeline>(_graphics->GetDeviceContext());
    
    _sceneManager = make_shared<SceneManager>(_graphics);
    _sceneManager->Init();
    
    _inputManager = make_shared<InputManager>();
    _inputManager->Init(hwnd);
    
    _timeManager = make_shared<TimeManager>();
    _timeManager->Init();
    
    _resourceManager = make_shared<ResourceManager>(_graphics->GetDevice());
    _resourceManager->Init();
    
    _renderManager = make_shared<RenderManager>(_graphics->GetDevice(), _graphics->GetDeviceContext());
    _renderManager->Init();
    
    SCENE->LoadScene(L"Test");
}

void Game::Update()
{
    SCENE->Update();
    TIME->Update();
    INPUT->Update();
}

void Game::Render()
{
    RENDER->Update(_graphics);
}
