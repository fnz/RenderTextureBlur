#include "TextureBlur.h"

USING_NS_CC;

static const int maxRadius = 64;

void TextureBlur::calculateGaussianWeights(const int points, float* weights)
{
    float dx = 1.0f/float(points-1);
    float sigma = 1.0f/3.0f;
    float norm = 1.0f/(sqrtf(2.0f*M_PI)*sigma*points);
    float divsigma2 = 0.5f/(sigma*sigma);
    weights[0] = 1.0f;
    for (int i = 1; i < points; i++)
    {
        float x = float(i)*dx;
        weights[i] = norm*expf(-x*x*divsigma2);
        weights[0] -= 2.0f*weights[i];
    }
}

GLProgram* TextureBlur::getBlurShader(Vec2 pixelSize, Vec2 direction, const int radius, float* weights)
{
	std::string blurShaderPath = FileUtils::getInstance()->fullPathForFilename("Shaders/TextureBlur.fsh");
	const GLchar* blur_frag = String::createWithContentsOfFile(blurShaderPath.c_str())->getCString();

	GLProgram* blur = GLProgram::createWithByteArrays(ccPositionTextureColor_vert, blur_frag);
	
	GLProgramState* state = GLProgramState::getOrCreateWithGLProgram(blur);
	state->setUniformVec2("pixelSize", pixelSize);
	state->setUniformVec2("direction", direction);
	state->setUniformInt("radius", radius);
	state->setUniformFloatv("weights", radius, weights);

    return blur;
}

void TextureBlur::create(Texture2D* target, const int radius, const std::string& fileName, std::function<void()> callback, const int step)
{
    CCASSERT(target != nullptr, "Null pointer passed as a texture to blur");
    CCASSERT(radius <= maxRadius, "Blur radius is too big");
    CCASSERT(radius > 0, "Blur radius is too small");
    CCASSERT(!fileName.empty(), "File name can not be empty");
    CCASSERT(step <= radius/2 + 1 , "Step is too big");
    CCASSERT(step > 0 , "Step is too small");
    
	Size textureSize = target->getContentSize();
    Vec2 pixelSize = Vec2(float(step)/textureSize.width, float(step)/textureSize.height);
    int radiusWithStep = radius/step;
    
    float* weights = new float[maxRadius];
    calculateGaussianWeights(radiusWithStep, weights);
    
    Sprite* stepX = CCSprite::createWithTexture(target);
    stepX->retain();
    stepX->setPosition(Point(0.5f*textureSize.width, 0.5f*textureSize.height));
    stepX->setFlippedY(true);
    
    GLProgram* blurX = getBlurShader(pixelSize, Vec2(1.0f, 0.0f), radiusWithStep, weights);
    stepX->setGLProgram(blurX);
    
    RenderTexture* rtX = RenderTexture::create(textureSize.width, textureSize.height);
    rtX->retain();
    rtX->begin();
    stepX->visit();
    rtX->end();
    
    Sprite* stepY = CCSprite::createWithTexture(rtX->getSprite()->getTexture());
    stepY->retain();
    stepY->setPosition(Point(0.5f*textureSize.width, 0.5f*textureSize.height));
    stepY->setFlippedY(true);
    
    GLProgram* blurY = getBlurShader(pixelSize, Vec2(0.0f, 1.0f), radiusWithStep, weights);
    stepY->setGLProgram(blurY);
    
    RenderTexture* rtY = RenderTexture::create(textureSize.width, textureSize.height);
    rtY->retain();
    rtY->begin();
    stepY->visit();
    rtY->end();
    
	auto completionCallback = [rtX, rtY, stepX, stepY, callback](RenderTexture* rt, const std::string& filename)
	{
        stepX->release();
        stepY->release();
        rtX->release();
        rtY->release();
        callback();
    };
	
    rtY->saveToFile(fileName, true, completionCallback);
}

