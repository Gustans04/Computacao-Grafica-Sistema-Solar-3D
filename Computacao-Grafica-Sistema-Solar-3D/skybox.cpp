#include "skybox.h"

#ifdef _WIN32
//#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/glad.h>
#elif __APPLE__
#include <OpenGL/gl3.h>
#endif

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

SkyBoxPtr SkyBox::Make()
{
	return SkyBoxPtr(new SkyBox());
}


SkyBox::SkyBox()
{
    static const float cubeVerts[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_coord_buffer);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_coord_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(LOC::COORD);
    glVertexAttribPointer(LOC::COORD, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}


void SkyBox::Draw(StatePtr st)
{
	CameraPtr camera = st->GetCamera();
	glm::vec4 origin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec3 peye = glm::vec3(glm::inverse(camera->GetViewMatrix()) * origin);
	glm::mat4 m = glm::translate(glm::mat4(1.0f), peye);

	st->PushMatrix();
	st->MultMatrix(m);
	st->LoadMatrices(); // update loaded matrices
	
	glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_coord_buffer);
	glEnableVertexAttribArray(LOC::COORD);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindBuffer(GL_ARRAY_BUFFER, m_coord_buffer);
	glDisableVertexAttribArray(LOC::COORD);

	glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

	st->PopMatrix();
}
