#include "pch.h"
#include "MonoBehavior.h"

MonoBehavior::MonoBehavior()
    : Super(ComponentType::Script)
{
    
}

MonoBehavior::~MonoBehavior()
{
    
}

void MonoBehavior::Awake()
{
    Component::Awake();
}

void MonoBehavior::Update()
{
    Component::Update();
}
