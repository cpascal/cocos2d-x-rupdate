#ifndef __RUPDATE_LAYER_H__
#define __RUPDATE_LAYER_H__

#include "cocos2d.h"
#include "cocos-ext.h"

class RUpdateLayer : public cocos2d::Layer
{
public:
    virtual bool init();
    
	void onLoadSucceed();
	void onLoadFailed();
	void startDownloadCallback(cocos2d::Ref* sender);
	
	// implement the "static create()" method manually
	CREATE_FUNC(RUpdateLayer);

protected:
	cocos2d::Menu* _downloadMenu;

	cocos2d::extension::AssetsManagerEx* _am;
	cocos2d::Label* _progress;
	cocos2d::extension::EventListenerAssetsManagerEx* _amListener;
};

#endif // __RUPDATE_LAYER_H__
