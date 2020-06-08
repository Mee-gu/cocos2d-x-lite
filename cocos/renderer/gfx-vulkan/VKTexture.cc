#include "VKStd.h"
#include "VKTexture.h"
#include "VKCommands.h"

NS_CC_BEGIN

CCVKTexture::CCVKTexture(GFXDevice* device)
    : GFXTexture(device)
{
}

CCVKTexture::~CCVKTexture()
{
}

bool CCVKTexture::initialize(const GFXTextureInfo &info)
{
    _type = info.type;
    _usage = info.usage;
    _format = info.format;
    _width = info.width;
    _height = info.height;
    _depth = info.depth;
    _arrayLayer = info.arrayLayer;
    _mipLevel = info.mipLevel;
    _samples = info.samples;
    _flags = info.flags;
    _size = GFXFormatSize(_format, _width, _height, _depth);

    if (_flags & GFXTextureFlags::BAKUP_BUFFER)
    {
        _buffer = (uint8_t*)CC_MALLOC(_size);
        _device->getMemoryStatus().textureSize += _size;
    }

    _gpuTexture = CC_NEW(CCVKGPUTexture);
    _gpuTexture->type = _type;
    _gpuTexture->format = _format;
    _gpuTexture->usage = _usage;
    _gpuTexture->width = _width;
    _gpuTexture->height = _height;
    _gpuTexture->depth = _depth;
    _gpuTexture->size = _size;
    _gpuTexture->arrayLayer = _arrayLayer;
    _gpuTexture->mipLevel = _mipLevel;
    _gpuTexture->samples = _samples;
    _gpuTexture->flags = _flags;
    _gpuTexture->isPowerOf2 = math::IsPowerOfTwo(_width) && math::IsPowerOfTwo(_height);

    if (CCVKCmdFuncCreateTexture((CCVKDevice*)_device, _gpuTexture))
    {
        _device->getMemoryStatus().textureSize += _size;
        _status = GFXStatus::SUCCESS;
    }
    else
    {
        CC_DELETE(_gpuTexture);
        _gpuTexture = nullptr;
        _status = GFXStatus::FAILED;
        return false;
    }

    _gpuTextureView = CC_NEW(CCVKGPUTextureView);
    _gpuTextureView->gpuTexture = _gpuTexture;
    _gpuTextureView->type = _type;
    _gpuTextureView->format = _format;
    _gpuTextureView->baseLevel = 0;
    _gpuTextureView->levelCount = _mipLevel;
    CCVKCmdFuncCreateTextureView((CCVKDevice*)_device, _gpuTextureView);

    return true;
}

bool CCVKTexture::initialize(const GFXTextureViewInfo& info)
{
    if (!info.texture)
    {
        CC_LOG_ERROR("CCVKTexture::initialize: texture should not be nullptr when create a texture view.");
        _status = GFXStatus::FAILED;
        return false;
    }

    _type = info.type;
    _format = info.format;
    _arrayLayer = info.layerCount;
    _mipLevel = info.levelCount;
    _usage = info.texture->getUsage();
    _width = info.texture->getWidth();
    _height = info.texture->getHeight();
    _depth = info.texture->getDepth();
    _samples = info.texture->getSamples();
    _flags = info.texture->getFlags();
    _size = GFXFormatSize(_format, _width, _height, _depth);
    
    _gpuTextureView->gpuTexture = static_cast<CCVKTexture*>(info.texture)->gpuTexture();
    _gpuTextureView->type = _type;
    _gpuTextureView->format = _format;
    _gpuTextureView->baseLevel = info.baseLevel;
    _gpuTextureView->levelCount = info.levelCount;
    CCVKCmdFuncCreateTextureView((CCVKDevice*)_device, _gpuTextureView);

    _status = GFXStatus::SUCCESS;
    return true;
}

void CCVKTexture::destroy()
{
    if (_gpuTexture)
    {
        CCVKCmdFuncDestroyTexture((CCVKDevice*)_device, _gpuTexture);
        _device->getMemoryStatus().textureSize -= _size;
        CC_DELETE(_gpuTexture);
        _gpuTexture = nullptr;
    }

    if (_buffer)
    {
        CC_FREE(_buffer);
        _device->getMemoryStatus().textureSize -= _size;
        _buffer = nullptr;
    }

    if (_gpuTextureView)
    {
        CCVKCmdFuncDestroyTextureView((CCVKDevice*)_device, _gpuTextureView);
        CC_DELETE(_gpuTextureView);
        _gpuTextureView = nullptr;
    }

    _status = GFXStatus::UNREADY;
}

void CCVKTexture::resize(uint width, uint height)
{
    uint size = GFXFormatSize(_format, width, height, _depth);
    if (_size != size)
    {
        const uint old_size = _size;
        _width = width;
        _height = height;
        _size = size;

        GFXMemoryStatus& status = _device->getMemoryStatus();
        _gpuTexture->width = _width;
        _gpuTexture->height = _height;
        _gpuTexture->size = _size;
        CCVKCmdFuncResizeTexture((CCVKDevice*)_device, _gpuTexture);
        status.bufferSize -= old_size;
        status.bufferSize += _size;

        if (_buffer)
        {
            const uint8_t* old_buff = _buffer;
            _buffer = (uint8_t*)CC_MALLOC(_size);
            memcpy(_buffer, old_buff, old_size);
            CC_FREE(_buffer);
            status.bufferSize -= old_size;
            status.bufferSize += _size;
        }
    }

    _status = GFXStatus::UNREADY;
}

NS_CC_END
