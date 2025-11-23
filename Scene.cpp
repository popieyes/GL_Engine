#include "Scene.h"

Scene::Scene() {
	this->lights = vector<Light*>();
	this->cameras = vector<Camera>();
	this->models = vector<Model>();
}

void Scene::Setup()
{

}