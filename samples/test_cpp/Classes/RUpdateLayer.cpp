#include "RUpdateLayer.h"

USING_NS_CC;
USING_NS_CC_EXT;

// on "init" you need to initialize your instance
bool RUpdateLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
        return false;
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto downloadLabel = Label::createWithTTF("Start Download", "fonts/arial.ttf", 16);
	auto downloadItem = MenuItemLabel::create(downloadLabel, CC_CALLBACK_1(RUpdateLayer::startDownloadCallback, this));
	downloadItem->setPosition(Vec2(origin.x + (visibleSize.width / 2.0f), origin.y + 100));
	_downloadMenu = Menu::create(downloadItem, nullptr);
	_downloadMenu->setPosition(Vec2::ZERO);
	this->addChild(_downloadMenu, 1);

	auto sprite = Sprite::create("Images/Icon.png");
	addChild(sprite);
	sprite->setPosition(origin.x + (visibleSize.width / 2.0f), origin.y + (visibleSize.height / 2.0f));

	TTFConfig config("fonts/tahoma.ttf", 30);
	_progress = Label::createWithTTF(config, "0%", TextHAlignment::CENTER);
	_progress->setPosition(origin.x + (visibleSize.width / 2.0f), origin.y + (visibleSize.height / 2.0f) + 50.0f);
	addChild(_progress);

	std::string manifestPath = "project.manifest";
	std::string storagePath = FileUtils::getInstance()->getWritablePath();
	CCLOG("Storage path for this test : %s", storagePath.c_str());
	_am = AssetsManagerEx::create(manifestPath, storagePath);
	_am->retain();

    return true;
}

void RUpdateLayer::onLoadSucceed()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto backgroundSprite = Sprite::create("grass.png");
	addChild(backgroundSprite, 1);
	backgroundSprite->setPosition(origin.x + (visibleSize.width / 2.0f), origin.y + (visibleSize.height / 2.0f));

	_progress->setString("Succeed!");
}

void RUpdateLayer::onLoadFailed()
{
	_progress->setString("Failed!");
}

void RUpdateLayer::startDownloadCallback(Ref* sender)
{
	removeChild(_downloadMenu);
	_downloadMenu = nullptr;

	if (!_am->getLocalManifest()->isLoaded())
	{
		CCLOG("Fail to update assets, step skipped.");
		onLoadFailed();
	}
	else
	{
		_amListener = cocos2d::extension::EventListenerAssetsManagerEx::create(_am, [this](EventAssetsManagerEx* event){
			static int failCount = 0;
			switch (event->getEventCode())
			{
			case EventAssetsManagerEx::EventCode::ERROR_NO_LOCAL_MANIFEST:
			{
				CCLOG("No local manifest file found, skip assets update.");
				this->onLoadSucceed();
			}
			break;
			case EventAssetsManagerEx::EventCode::UPDATE_PROGRESSION:
			{
				std::string assetId = event->getAssetId();
				float percent = event->getPercent();
				std::string str;
				if (assetId == AssetsManagerEx::VERSION_ID)
				{
					str = StringUtils::format("Version file: %.2f", percent) + "%";
				}
				else if (assetId == AssetsManagerEx::MANIFEST_ID)
				{
					str = StringUtils::format("Manifest file: %.2f", percent) + "%";
				}
				else
				{
					str = StringUtils::format("%.2f", percent) + "%";
					CCLOG("%.2f Percent", percent);
				}
				if (this->_progress != nullptr)
					this->_progress->setString(str);
			}
			break;
			case EventAssetsManagerEx::EventCode::ERROR_DOWNLOAD_MANIFEST:
			case EventAssetsManagerEx::EventCode::ERROR_PARSE_MANIFEST:
			{
				CCLOG("Fail to download manifest file, update skipped.");
				this->onLoadFailed();
			}
			break;
			case EventAssetsManagerEx::EventCode::ALREADY_UP_TO_DATE:
			case EventAssetsManagerEx::EventCode::UPDATE_FINISHED:
			{
				CCLOG("Update finished. %s", event->getMessage().c_str());
				this->onLoadSucceed();
			}
			break;
			case EventAssetsManagerEx::EventCode::UPDATE_FAILED:
			{
				CCLOG("Update failed. %s", event->getMessage().c_str());

				failCount++;
				if (failCount < 5)
				{
					_am->downloadFailedAssets();
				}
				else
				{
					CCLOG("Reach maximum fail count, exit update process");
					failCount = 0;
					this->onLoadFailed();
				}
			}
			break;
			case EventAssetsManagerEx::EventCode::ERROR_UPDATING:
			{
				CCLOG("Asset %s : %s", event->getAssetId().c_str(), event->getMessage().c_str());
			}
			break;
			case EventAssetsManagerEx::EventCode::ERROR_DECOMPRESS:
			{
				CCLOG("%s", event->getMessage().c_str());
			}
			break;
			default:
				break;
			}
		});
		Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_amListener, 1);

		_am->update();
	}
}
