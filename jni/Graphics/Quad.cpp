#include "Quad.h"

Quad::~Quad()
{
	unload();
}

void Quad::init()
{
	initShaders();
	/*

		const GLfloat vertices[] = { -1, -1, 0,  // bottom left corner
		                             -1,  1, 0,  // top left corner
		                              1,  1, 0,  // top right corner
		                              1, -1, 0}; // bottom right corner

	*/
}

void Quad::draw(ShadowMap& shadowMap)
{


	const GLfloat texCoords[] = { 0, 0,
								  0, 1,
								  1, 1,
								  1, 0 };

	const GLfloat vertices[] = { 0, -1, 0,  // bottom left corner
	                             0,  0, 0,  // top left corner
	                             1,  0, 0,  // top right corner
	                             1, -1, 0}; // bottom right corner

	const GLubyte indices[] = { 0, 2, 1,     // first triangle (bottom left - top left - top right)
	                            0, 3, 2 };

	 glDisable( GL_CULL_FACE );

	glUseProgram( mShaderParams.program );
	// ensure no VBOs or IBOs are bound

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glVertexAttribPointer(ATTR_VERTEX, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), vertices);
	glVertexAttribPointer(ATTR_TEXTURE, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), texCoords);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowMap.mDepthMap);

	// ensure the proper arrays are enabled
	glEnableVertexAttribArray(ATTR_VERTEX);
	glEnableVertexAttribArray(ATTR_TEXTURE);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
	glEnable( GL_CULL_FACE );
}

void Quad::bindShaderAttributes(GLuint program)
{
	 glBindAttribLocation( program, ATTR_VERTEX, "a_Position" );
	 glBindAttribLocation( program, ATTR_TEXTURE, "a_TexCoord" );
}

void Quad::initShaders()
{
	mShaderParams.program = loadShaders( "shaders/quad.vsh", "shaders/quad.fsh" );
	mShaderParams.sampler = glGetUniformLocation( mShaderParams.program, "u_DepthMap" );
}

void Quad::unload()
{
    if( mShaderParams.program )
    {
        glDeleteProgram( mShaderParams.program );
        mShaderParams.program = 0;
    }
}
