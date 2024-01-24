#pragma once
//#NOTE: This class is unimplemented and needs to be reworked. 
//#protype_object_class_redisgn_when_you_have_more_insight_into_what_you_want_to_automate

#include "utils.h"

#include <vector>
#include <map>
#include <string>

class Object
{
private:
	unsigned int VAO;
	std::vector<unsigned int> VBO;
	unsigned int EBO;
	bool usingEBO;
	
	std::map<std::string, unsigned int> attribArray; //Be careful not to create two different names with the same index.

public:
	Object();

	void setUpBasicNoEbo(float* vert_array);
	void setUpBasicWithEbo(float* vert_array, unsigned int* ind_array);

	void bind();
	void unbind();
	void reset();

	void addVBO(float* vert_array);

	void addEBO(float* ind_array);

	void addVertAttribPtr(
		std::string name,
		unsigned int index, 
		unsigned int size, 
		GLenum type, 
		GLboolean normalized, 
		GLsizei stride, 
		const GLvoid*pointer);

	void bindVertexArray(unsigned int index);
	void bindVertexArray(std::string name);
};