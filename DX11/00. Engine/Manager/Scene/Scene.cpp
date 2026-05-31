#include "pch.h"
#include "Scene.h"

#include "00. Engine/Component/Rendering/MeshRenderer.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Awake()
{
    for (const shared_ptr<GameObject>& gameObject : _gameObjects)
    {
        gameObject->Awake();
    }
}

void Scene::Start()
{
    for (const shared_ptr<GameObject>& gameObject : _gameObjects)
    {
        gameObject->Start();
    }
}

void Scene::Update()
{
    for (const shared_ptr<GameObject>& gameObject : _gameObjects)
    {
        gameObject->Update();
    }
}

void Scene::LateUpdate()
{
    for (const shared_ptr<GameObject>& gameObject : _gameObjects)
    {
        gameObject->LateUpdate();
    }
}

void Scene::FixedUpdate()
{
    for (const shared_ptr<GameObject>& gameObject : _gameObjects)
    {
        gameObject->FixedUpdate();
    }
}

void Scene::AddGameObject(shared_ptr<GameObject> gameObject)
{
    _gameObjects.push_back(gameObject);
}

void Scene::RemoveGameObject(shared_ptr<GameObject> gameObject)
{
    auto findIt = std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);
    if (findIt != _gameObjects.end())
    {
        _gameObjects.erase(findIt);
    }   
}
