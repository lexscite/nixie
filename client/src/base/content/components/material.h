#ifndef MATERIAL_H
#define MATERIAL_H

#pragma once

#include "../component.h"
#include "mesh.h"
#include "camera.h"
#include "../../graphics/shader.h"

class Material : public Component
{
public:
	Material();
	Shader* GetShader();

private:
	virtual void OnInit() override;
	virtual void OnUpdate() override;

private:
	Shader* shader_;
};

#endif