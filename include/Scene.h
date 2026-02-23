#pragma once
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Model.h"
#include "./lights/Light.h"

class Light;
using namespace std;	

class Scene
{
public: 
	Scene();
	Camera* camera;
	vector<Camera> cameras;
	vector<Model> models;
	vector<Light*> lights;
	// Shader used to render all objects in the scene. Might be overwritten by shaders included in Gameobjects materials
	Shader* mainShader;

	void Setup();
	void AddModel(const Model& model) { models.push_back(model); }
	void AddModel(vector <Model> models) {
		for (Model& model : models) { this->models.push_back(model); } }
	void AddCamera(Camera camera) { cameras.push_back(camera); }
	void AddLight(Light* light) { lights.push_back(light); }
};

