#include "HelloWorldScene.h"
#include "TextureBlur.h"
#include "2d/CCRenderTexture.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
	Size screenSize = Director::getInstance()->getWinSize();
	
	std::string fileName = "back_blur.png";
	std::string fullFileName = FileUtils::getInstance()->getWritablePath() + fileName;
	
	if (FileUtils::getInstance()->isFileExist(fullFileName))
	{
		completionCallback(fullFileName);
	}
	else
	{
		auto sky = Sprite::create("sky.png");
		sky->retain();
		sky->setPosition(0.5f*screenSize.width, 0.5f*screenSize.height);
		sky->setScaleX(screenSize.width/sky->getBoundingBox().size.width);
		sky->setScaleY(screenSize.height/sky->getBoundingBox().size.height);
		
		auto clouds = Sprite::create("clouds.png");
		clouds->retain();
		clouds->setScale(screenSize.width/clouds->getBoundingBox().size.width);
		clouds->setAnchorPoint(Point(0.5f, 1.0f));
		clouds->setPosition(Point(0.5f*screenSize.width, screenSize.height));
		
		auto ground = Sprite::create("ground.png");
		ground->retain();
		ground->setScale(screenSize.width/ground->getBoundingBox().size.width);
		ground->setAnchorPoint(Point(0.5f, 0.0f));
		ground->setPosition(0.5f*screenSize.width, 0.0f);
		
		auto rt = RenderTexture::create(screenSize.width, screenSize.height, Texture2D::PixelFormat::RGBA8888);
		rt->retain();
		rt->begin();
		sky->visit();
		ground->visit();
		clouds->visit();
		rt->end();

		TextureBlur::create(rt->getSprite()->getTexture(), 15, fileName, std::bind(&HelloWorld::completionCallback, this, fullFileName));
	}
	
	return true;
}

void HelloWorld::completionCallback(const std::string& name) {
	Size screenSize = Director::getInstance()->getWinSize();
	auto picture = Sprite::create(name);
	picture->setPosition(0.5f*screenSize);
	addChild(picture);
}