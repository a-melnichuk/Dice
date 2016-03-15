#include "Container.h"


Container::Container(Camera& camera, TextureManager& textureManager) :
mTopRadius(0.0f), mBottomRadius(0.0f), mHeight(0.0f),
mNumStacks(0), mNumSlices(0),
mCamera(camera), mTextureManager(textureManager)
{
}

Container::~Container()
{
	unload();
}

Container::Vertex::Vertex(float p_x,float p_y, float p_z,
						  float n_x,float n_y, float n_z,
						  float tex_u, float tex_v)
{
	pos[0]=p_x;
	pos[1]=p_y;
	pos[2]=p_z;

	normal[0]=n_x;
	normal[1]=n_y;
	normal[2]=n_z;

	tex[0] = tex_u;
	tex[1] = tex_v;
}

Container::Vertex::Vertex(ndk_helper::Vec3& P, ndk_helper::Vec3& N, float tex_u, float tex_v)
{
	P.Value(pos[0], pos[1], pos[2]);
	N.Value(normal[0], normal[1], normal[2]);
	tex[0] = tex_u;
	tex[1] = tex_v;
}

void Container::init(float topRadius, float bottomRadius, float height,
		uint32_t numSlices, uint32_t numStacks)
{
	mTopRadius    = topRadius;
	mBottomRadius = bottomRadius;
	mHeight       = height;
	mNumSlices    = numSlices;
	mNumStacks    = numStacks;

	glFrontFace( GL_CCW );
	initTextures();
	initShaders();

	std::vector<Vertex> vertices;
	std::vector<short> indices;

	buildStacks(vertices, indices);
	buildTopCap(vertices, indices);
	buildBottomCap(vertices, indices);

	mNumVertices =  (uint32_t)vertices.size();
	mNumFaces    = (uint32_t)indices.size()/3;

	Vertex* verticesArr = new Vertex[mNumVertices];
	short* indicesArr = new short[mNumFaces*3];
	std::copy(vertices.begin(), vertices.end(), verticesArr);
	std::copy(indices.begin(), indices.end(), indicesArr);

	initBuffers(verticesArr, indicesArr);

	delete[] verticesArr;
	delete[] indicesArr;

	//mM = ndk_helper::Mat4::Identity();
}

void Container::update(float dt)
{

}

void Container::draw()
{

    ndk_helper::Mat4 MVP = mCamera.mViewProj;// * mM;
    glDisable( GL_CULL_FACE );
    // Bind the VBO
    glBindBuffer( GL_ARRAY_BUFFER, mVbo );

    int32_t iStride = sizeof(Vertex);
    // Pass the vertex data
    glVertexAttribPointer( ATTR_VERTEX, 3, GL_FLOAT, GL_FALSE, iStride, BUFFER_OFFSET( 0 ) );
    glEnableVertexAttribArray( ATTR_VERTEX );

    glVertexAttribPointer( ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, iStride, BUFFER_OFFSET( 3 * sizeof(GLfloat) ) );
    glEnableVertexAttribArray( ATTR_NORMAL );

    glVertexAttribPointer( ATTR_TEXTURE, 2, GL_FLOAT, GL_FALSE, iStride, BUFFER_OFFSET( 6 * sizeof(GLfloat) ) );
    glEnableVertexAttribArray( ATTR_TEXTURE );


    // Bind the IB
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIbo );

    glUseProgram( mShaderParams.program );

    mTexture->apply();

    ndk_helper::Vec3 lightColor(0.1f,0.1f,0.1f);

    glUniform1i(mShaderParams.sampler, 0);
    glUniform3f( mShaderParams.eyePos, mCamera.mPos.x_, mCamera.mPos.y_, mCamera.mPos.z_ );
    glUniform3f( mShaderParams.lightColor, lightColor.x_, lightColor.y_, lightColor.z_ );
    glUniformMatrix4fv( mShaderParams.MVP, 1, GL_FALSE, MVP.Ptr() );

    glDrawElements( GL_TRIANGLES, 3*mNumFaces, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0) );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glEnable( GL_CULL_FACE );
}

void Container::buildStacks(std::vector<Vertex>& vertices, std::vector<short>& indices)
{
	float stackHeight = mHeight / mNumStacks;

	// Amount to increment radius as we move up each stack level from bottom to top.
	float radiusStep = (mTopRadius - mBottomRadius) / mNumStacks;

	uint32_t numRings = mNumStacks+1;

	// Compute vertices for each stack ring.
	for(uint32_t i = 0; i < numRings; ++i)
	{
		float y = -0.5f*mHeight + i*stackHeight;
		float r = mBottomRadius + i*radiusStep;

		// Height and radius of next ring up.
		float y_next = -0.5f*mHeight + (i+1)*stackHeight;
		float r_next = mBottomRadius + (i+1)*radiusStep;

		// vertices of ring
		float dTheta = 2.0f*M_PI/mNumSlices;
		for(uint32_t j = 0; j <= mNumSlices; ++j)
		{
			float c = cosf(j*dTheta);
			float s = sinf(j*dTheta);

			float u = fmod(0.3f*j*dTheta,2.0f*r);// (0.5f*c + 0.5f);
			float v = 1.0f - (float)i/mNumStacks;

			// Partial derivative in theta direction to get tangent vector (this is a unit vector).
			ndk_helper::Vec3 T(-s,0.0f,c);
			ndk_helper::Vec3 P(r*c, y, r*s);
			ndk_helper::Vec3 P_next(r_next*c, y_next, r_next*s);
			ndk_helper::Vec3 B = P - P_next;
			B = B.Normalize();
			ndk_helper::Vec3 N = -T.Cross(B);
			N = N.Normalize();

			vertices.push_back( Vertex(P, N, u, v) );
		}
	}

	uint32_t numRingVertices = mNumSlices +1;

	// Compute indices for each stack.
	for(uint32_t i = 0; i < mNumStacks; ++i)
	{
		for(uint32_t j = 0; j < mNumSlices; ++j)
		{
			indices.push_back(i*numRingVertices + j);
			indices.push_back((i+1)*numRingVertices + j);
			indices.push_back((i+1)*numRingVertices + j+1);

			indices.push_back(i*numRingVertices + j);
			indices.push_back((i+1)*numRingVertices + j+1);
			indices.push_back(i*numRingVertices + j+1);
		}
	}
}

void Container::buildBottomCap(std::vector<Vertex>& vertices, std::vector<short>& indices)
{
	uint32_t baseIndex = (uint32_t)vertices.size();

	// Duplicate cap vertices because the texture coordinates and normals differ.
	float y = -0.5f*mHeight;

	// vertices of ring
	float dTheta = 2.0f*M_PI/mNumSlices;
	for(uint32_t i = 0; i <= mNumSlices; ++i)
	{
		float x = mBottomRadius*cosf(i*dTheta);
		float z = mBottomRadius*sinf(i*dTheta);

		// Map [-1,1]-->[0,1] for planar texture coordinates.
		float u = 0.5f*x/mBottomRadius + 0.5f;
		float v = -0.5f*z/mBottomRadius + 0.5f;

		vertices.push_back( Vertex(x, y, z, 0.0f, 1.0f, 0.0f, v, u));

	}

	vertices.push_back( Vertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f) );

	// index of center vertex
	uint32_t centerIndex = (uint32_t)vertices.size()-1;

	for(uint32_t i = 0; i < mNumSlices; ++i)
	{
		indices.push_back(centerIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i+1);
	}
}

void Container::buildTopCap(std::vector<Vertex>& vertices, std::vector<short>& indices)
{
	uint32_t baseIndex = (uint32_t)vertices.size();

	float y = 0.5f*mHeight;

	float dTheta = 2.0f*M_PI/mNumSlices;
	for(uint32_t i = 0; i <= mNumSlices; ++i)
	{
		float x = mTopRadius*cosf(i*dTheta);
		float z = mTopRadius*sinf(i*dTheta);

		// Map [-1,1]->[0,1] for planar texture coordinates.
		float u = 0.5f*x/mTopRadius + 0.5f;
		float v = -0.5f*z/mTopRadius + 0.5f;

		vertices.push_back( Vertex(x, y, z, 0.0f, -1.0f, 0.0f, v, u));
	}
	// cap center vertex
	vertices.push_back( Vertex(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 0.5f, 0.5f) );

	// index of center vertex
	uint32_t centerIndex = (uint32_t)vertices.size()-1;

	for(uint32_t i = 0; i < mNumSlices; ++i)
	{
		indices.push_back(centerIndex);
		indices.push_back(baseIndex + i+1);
		indices.push_back(baseIndex + i);
	}
}
void Container::bindShaderAttributes(GLuint program)
{
	 glBindAttribLocation( program, ATTR_VERTEX, "a_Position" );
	 glBindAttribLocation( program, ATTR_NORMAL, "a_Normal" );
	 glBindAttribLocation( program, ATTR_TEXTURE, "a_TexCoord" );
}

void Container::initShaders()
{
	mShaderParams.program = loadShaders( "shaders/container.vsh", "shaders/container.fsh" );
	mShaderParams.MVP = glGetUniformLocation( mShaderParams.program, "u_MVPMatrix" );
	mShaderParams.lightColor = glGetUniformLocation( mShaderParams.program, "u_LightColor" );
	mShaderParams.eyePos = glGetUniformLocation( mShaderParams.program, "u_EyePos" );
	mShaderParams.sampler = glGetUniformLocation( mShaderParams.program, "u_TextureUnit" );
}

void Container::initTextures()
{
	mTexture = mTextureManager.registerTexture("dice/Plank.png");
	mTexture->load();
}

void Container::initBuffers(Vertex* verticesArr, short* indicesArr)
{
	glGenBuffers( 1, &mIbo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIbo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * mNumFaces * 3  , indicesArr, GL_STATIC_DRAW );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	glGenBuffers( 1, &mVbo );
	glBindBuffer( GL_ARRAY_BUFFER, mVbo );
	glBufferData( GL_ARRAY_BUFFER,  sizeof(Vertex) * mNumVertices, verticesArr, GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void Container::unload()
{
    if( mVbo )
    {
        glDeleteBuffers( 1, &mVbo );
        mVbo = 0;
    }

    if( mIbo )
    {
        glDeleteBuffers( 1, &mIbo );
        mIbo = 0;
    }

    if( mShaderParams.program )
    {
        glDeleteProgram( mShaderParams.program );
        mShaderParams.program = 0;
    }
}
