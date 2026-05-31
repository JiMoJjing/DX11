#pragma once

#include "00. Engine/Manager/Resource/ResourceBase.h"

class Shader;
class Texture;

class Material : public ResourceBase
{
	using Super = ResourceBase;

public:
	Material();
	virtual ~Material();

	auto GetShader() { return _shader; }
	auto GetTexture() { return _texture; }

	void SetShader(shared_ptr<Shader> shader) { _shader = shader; }
	void SetTexture(shared_ptr<Texture> texture) { _texture = texture;}
	
private:
	shared_ptr<Shader> _shader;

	shared_ptr<Texture> _texture;
};

