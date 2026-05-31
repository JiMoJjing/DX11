#include "pch.h"
#include "Component.h"
#include "00. Engine/GameObject/GameObject.h"

Component::Component(ComponentType type)
    : _type(type)
{
    
}

Component::~Component()
{
    
}

shared_ptr<GameObject> Component::GetGameObject()
{
    return _gameObject.lock();
}

shared_ptr<Transform> Component::GetTransform()
{
    return _gameObject.lock()->GetTransform();
}
