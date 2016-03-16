#include "Dice.h"
#include "dice.inl"

Dice::Dice(Camera& camera, TextureManager& textureManager):
mCamera(camera), mTextureManager(textureManager)
{

}
Dice::~Dice()
{
    unload();
}


void Dice::init(RigidBody *rigidBody, ndk_helper::Vec3& halfSize)
{
    glFrontFace( GL_CCW );

    initTextures();
    initShaders();
    initBuffers();

    mHalfSize = halfSize;
    mRigidBody = rigidBody;
    mInitial = ndk_helper::Mat4::Identity();
    mInitial.SetDiagonal(mHalfSize);

    mM = mInitial;
}


void Dice::update( float dt )
{
	mM = mRigidBody->mModel * mInitial;
	mMVP = mCamera.mViewProj * mM;
}

void Dice::drawToShadowMap(ShadowMap& shadowMap)
{
	glUseProgram( shadowMap.mShaderParams.program );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIbo );
	glBindBuffer( GL_ARRAY_BUFFER, mVbo );

	glVertexAttribPointer( shadowMap.ATTR_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET( 0 ) );
	glEnableVertexAttribArray( shadowMap.ATTR_VERTEX );


	glUniformMatrix4fv( shadowMap.mShaderParams.VP, 1, GL_FALSE, shadowMap.mLightViewProj.Ptr() );
	glUniformMatrix4fv( shadowMap.mShaderParams.M, 1, GL_FALSE, mM.Ptr() );
	glDrawElements( GL_TRIANGLES, mNumIdices, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0) );
	glDisableVertexAttribArray( shadowMap.ATTR_VERTEX );
}

void Dice::draw(ShadowMap& shadowMap)
{
	glUseProgram( mShaderParams.program );

    mTexture->apply();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadowMap.mDepthMap);


    ndk_helper::Vec3 lightColor(0.5f,0.5f,0.5f);

    glUniform1i(mShaderParams.sampler, 0);
    glUniform3f( mShaderParams.eyePos, mCamera.mPos.x_, mCamera.mPos.y_, mCamera.mPos.z_ );
    glUniform3f( mShaderParams.lightColor, lightColor.x_, lightColor.y_, lightColor.z_ );
    glUniformMatrix4fv( mShaderParams.MVP, 1, GL_FALSE, mMVP.Ptr() );
    glUniformMatrix4fv( mShaderParams.M, 1, GL_FALSE, mM.Ptr() );

	// Bind the VBO
    glBindBuffer( GL_ARRAY_BUFFER, mVbo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIbo );

    int32_t iStride = sizeof(Vertex);
    // Pass the vertex data
    glVertexAttribPointer( ATTR_VERTEX, 3, GL_FLOAT, GL_FALSE, iStride,
            BUFFER_OFFSET( 0 ) );
    glEnableVertexAttribArray( ATTR_VERTEX );

    glVertexAttribPointer( ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, iStride,
            BUFFER_OFFSET( 3 * sizeof(GLfloat) ) );
    glEnableVertexAttribArray( ATTR_NORMAL );

    glDrawElements( GL_TRIANGLES, mNumIdices, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0) );

    glDisableVertexAttribArray(ATTR_VERTEX);
    glDisableVertexAttribArray(ATTR_NORMAL);

   // glBindBuffer( GL_ARRAY_BUFFER, 0 );
    //glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void Dice::bindShaderAttributes(GLuint program)
{
	glBindAttribLocation( program,  ATTR_VERTEX, "a_Position" );
	glBindAttribLocation( program, ATTR_NORMAL, "a_Normal" );
}



void Dice::initShaders()
{
	mShaderParams.program = loadShaders( "shaders/dice.vsh", "shaders/dice.fsh" );
	mShaderParams.MVP = glGetUniformLocation( mShaderParams.program, "u_MVPMatrix" );
	mShaderParams.M = glGetUniformLocation( mShaderParams.program, "u_ModelMatrix" );
	mShaderParams.lightColor = glGetUniformLocation( mShaderParams.program, "u_LightColor" );
	mShaderParams.eyePos = glGetUniformLocation( mShaderParams.program, "u_EyePos" );
	mShaderParams.sampler = glGetUniformLocation( mShaderParams.program, "u_TextureUnit" );
}



void Dice::initBuffers()
{
	mNumIdices = sizeof(diceIndices) / sizeof(diceIndices[0]);
	glGenBuffers( 1, &mIbo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIbo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(diceIndices), diceIndices, GL_STATIC_DRAW );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	//Create VBO
	mNumVertices = sizeof(dicePositions) / sizeof(dicePositions[0]) / 3;

	int32_t it = 0;
	Vertex* p = new Vertex[mNumVertices];
	for( int32_t i = 0; i < mNumVertices; ++i )
	{
		p[i].pos[0] = dicePositions[it];
		p[i].pos[1] = dicePositions[it + 1];
		p[i].pos[2] = dicePositions[it + 2];

		p[i].normal[0] = diceNormals[it];
		p[i].normal[1] = diceNormals[it + 1];
		p[i].normal[2] = diceNormals[it + 2];

		it += 3;
	}

	glGenBuffers( 1, &mVbo );
	glBindBuffer( GL_ARRAY_BUFFER, mVbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(Vertex) * mNumVertices, p, GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	delete[] p;
}

void Dice::initTextures()
{
	mTexture = mTextureManager.registerCubeMap("dice/d1.png","dice/d6.png",
											   "dice/d3.png","dice/d4.png",
											   "dice/d5.png","dice/d2.png");
	mTexture->load();
}

void Dice::unload()
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




