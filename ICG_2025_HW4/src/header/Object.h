#pragma once
#include <glad/glad.h>
#include <vector>
#include <string>
#include "Material.h"

using namespace std;

enum class FACETYPE {
	TRIANGLE,
	QUAD
};

class Object {
public:
	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> texcoords;

	Object(const std::string& filename);

	void draw();
	void loadTexture(const std::string& filepath);

	FACETYPE getFaceType();
	Material getMaterial();

	void setMaterial(const Material& newMaterial);

private:
	unsigned int VAO;
	unsigned int textureID = 0;
	bool hasTexture = false;
	int vertex_cnt;
	Material material;
	FACETYPE faceType = FACETYPE::TRIANGLE;

	void loadOBJ(const std::string& filename);
	void set_VAO();
};
