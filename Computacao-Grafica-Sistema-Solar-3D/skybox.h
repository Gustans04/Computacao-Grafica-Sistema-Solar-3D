#include <memory>
class SkyBox;
using SkyBoxPtr = std::shared_ptr<SkyBox>;

#ifndef SKYBOX_H
#define SKYBOX_H
#include "shape.h"
#include <glm/glm.hpp>

class SkyBox : public Shape {
	unsigned int m_vao;
	unsigned int m_coord_buffer;
protected:
	SkyBox ();
public:
	static SkyBoxPtr Make();
	virtual void Draw(StatePtr st);
};

#endif 
