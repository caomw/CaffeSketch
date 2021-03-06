﻿#include "Cuboid.h"
#include "Rectangle.h"
#include "Polygon.h"
#include "CGA.h"
#include "GLUtils.h"

namespace cga {

Cuboid::Cuboid(const std::string& name, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float depth, float height, const glm::vec3& color) {
	this->_name = name;
	this->_removed = false;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_scope.x = width;
	this->_scope.y = depth;
	this->_scope.z = height;
	this->_color = color;
}

boost::shared_ptr<Shape> Cuboid::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new Cuboid(*this));
	copy->_name = name;
	return copy;
}

void Cuboid::comp(const std::map<std::string, std::string>& name_map, std::vector<boost::shared_ptr<Shape> >& shapes) {
	// top face
	if (name_map.find("top") != name_map.end() && name_map.at("top") != "NIL") {
		glm::mat4 mat = glm::translate(_modelMat, glm::vec3(0, 0, _scope.z));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("top"), _pivot, mat, _scope.x, _scope.y, _color)));
	}

	// bottom face
	if (name_map.find("bottom") != name_map.end() && name_map.at("bottom") != "NIL" && _scope.z >= 0) {
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("bottom"), _pivot, _modelMat, _scope.x, _scope.y, _color)));
	}

	// front face
	if (name_map.find("front") != name_map.end() && name_map.at("front") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("front"), _pivot, glm::rotate(_modelMat, rot_angle, glm::vec3(1, 0, 0)), _scope.x, fabs(_scope.z), _color)));
	}

	// right face
	if (name_map.find("right") != name_map.end() && name_map.at("right") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, 0, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("right"), _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.y, fabs(_scope.z), _color)));
	}

	// left face
	if (name_map.find("left") != name_map.end() && name_map.at("left") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = glm::translate(glm::rotate(_modelMat, -M_PI * 0.5f, glm::vec3(0, 0, 1)), glm::vec3(-_scope.y, 0, 0));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("left"), _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.y, fabs(_scope.z), _color)));
	}

	// back face
	if (name_map.find("back") != name_map.end() && name_map.at("back") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = glm::translate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, 0, 0)), M_PI, glm::vec3(0, 0, 1)), glm::vec3(0, -_scope.y, 0));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("back"), _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.x, fabs(_scope.z), _color)));
	}

	// side faces
	if (name_map.find("side") != name_map.end() && name_map.at("side") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}

		// front face
		if (name_map.find("front") == name_map.end()) {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _pivot, glm::rotate(_modelMat, rot_angle, glm::vec3(1, 0, 0)), _scope.x, fabs(_scope.z), _color)));
		}

		// right face
		if (name_map.find("right") == name_map.end()) {
			glm::mat4 mat = glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, 0, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.y, fabs(_scope.z), _color)));
		}

		// left face
		if (name_map.find("left") == name_map.end()) {
			glm::mat4 mat = glm::translate(glm::rotate(_modelMat, -M_PI * 0.5f, glm::vec3(0, 0, 1)), glm::vec3(-_scope.y, 0, 0));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.y, fabs(_scope.z), _color)));
		}

		// back face
		if (name_map.find("back") == name_map.end()) {
			glm::mat4 mat = glm::translate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, 0, 0)), M_PI, glm::vec3(0, 0, 1)), glm::vec3(0, -_scope.y, 0));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.x, fabs(_scope.z), _color)));
		}
	}
}

void Cuboid::setupProjection(float texWidth, float texHeight) {
}

void Cuboid::size(float xSize, float ySize, float zSize) {
	_prev_scope = _scope;

	_scope.x = xSize;
	_scope.y = ySize;
	_scope.z = zSize;
}

/**
 */
void Cuboid::split(int splitAxis, const std::vector<float>& sizes, const std::vector<std::string>& names, std::vector<boost::shared_ptr<Shape> >& objects) {
	if (splitAxis == DIRECTION_X) {
		glm::mat4 mat = this->_modelMat;
		for (int i = 0; i < sizes.size(); ++i) {
			if (names[i] != "NIL") {
				objects.push_back(boost::shared_ptr<Shape>(new Cuboid(names[i], _pivot, mat, sizes[i], _scope.y, _scope.z, _color)));
			}
			mat = glm::translate(mat, glm::vec3(sizes[i], 0, 0));
		}
	} else if (splitAxis == DIRECTION_Y) {
		glm::mat4 mat = this->_modelMat;
		for (int i = 0; i < sizes.size(); ++i) {
			if (names[i] != "NIL") {
				objects.push_back(boost::shared_ptr<Shape>(new Cuboid(names[i], _pivot, mat, _scope.x, sizes[i], _scope.z, _color)));
			}
			mat = glm::translate(mat, glm::vec3(0, sizes[i], 0));
		}
	} else {
		glm::mat4 mat = this->_modelMat;
		for (int i = 0; i < sizes.size(); ++i) {
			if (names[i] != "NIL") {
				objects.push_back(boost::shared_ptr<Shape>(new Cuboid(names[i], _pivot, mat, _scope.x, _scope.y, sizes[i], _color)));
			}
			mat = glm::translate(mat, glm::vec3(0, 0, sizes[i]));
		}
	}
}

void Cuboid::generateGeometry(RenderManager* renderManager, float opacity) const {
	if (_removed) return;

	int num = 0;

	std::vector<Vertex> vertices;

	// top
	{
		glm::mat4 mat = _pivot * glm::translate(_modelMat, glm::vec3(_scope.x * 0.5, _scope.y * 0.5, _scope.z));
		glutils::drawQuad(_scope.x, _scope.y, glm::vec4(_color, opacity), mat, vertices);
	}

	// base
	if (_scope.z >= 0) {
		glm::mat4 mat = _pivot * glm::translate(_modelMat, glm::vec3(_scope.x * 0.5, _scope.y * 0.5, 0));
		glutils::drawQuad(_scope.x, _scope.y, glm::vec4(_color, opacity), mat, vertices);
	}

	// front
	{
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = _pivot * glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x * 0.5, 0, _scope.z * 0.5)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_scope.x, _scope.z, glm::vec4(_color, opacity), mat, vertices);
	}

	// back
	{
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = _pivot * glm::rotate(glm::translate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x * 0.5, 0, _scope.z * 0.5)), M_PI, glm::vec3(0, 0, 1)), glm::vec3(0, -_scope.y, 0)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_scope.x, _scope.z, glm::vec4(_color, opacity), mat, vertices);
	}

	// right
	{
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = _pivot * glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _scope.y * 0.5, _scope.z * 0.5)), M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_scope.y, _scope.z, glm::vec4(_color, opacity), mat, vertices);
	}

	// left
	{
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = _pivot * glm::rotate(glm::translate(glm::rotate(_modelMat, -M_PI * 0.5f, glm::vec3(0, 0, 1)), glm::vec3(-_scope.y * 0.5, 0, _scope.z * 0.5)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_scope.y, _scope.z, glm::vec4(_color, opacity), mat, vertices);
	}

	renderManager->addObject(_name.c_str(), "", vertices);
}

}
