#ifndef _QUAD_DICE_
#define _QUAD_DICE_

#include "Shape.h"
#include "ShadowMap.h"
#include "../Camera.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

class Quad : public Shape<Quad>
{
public:
	Quad(){};
    ~Quad();
    void init();
    void unload();
    void draw(ShadowMap& shadowMap);
private:
    struct ShaderParams { GLuint program, sampler; };
    enum SHADER_ATTR { ATTR_VERTEX, ATTR_TEXTURE };
    void initShaders();

	void bindShaderAttributes(GLuint program);

	ShaderParams mShaderParams;
};

#endif
