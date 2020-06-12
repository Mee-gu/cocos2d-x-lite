#include "VKStd.h"

#include "VKBindingLayout.h"
#include "VKBuffer.h"
#include "VKCommands.h"
#include "VKDevice.h"
#include "VKSampler.h"
#include "VKTexture.h"

NS_CC_BEGIN

CCVKBindingLayout::CCVKBindingLayout(GFXDevice *device)
: GFXBindingLayout(device) {
}

CCVKBindingLayout::~CCVKBindingLayout() {
}

bool CCVKBindingLayout::initialize(const GFXBindingLayoutInfo &info) {
    if (info.bindings.size()) {
        _bindingUnits.resize(info.bindings.size());
        for (size_t i = 0u; i < _bindingUnits.size(); ++i) {
            GFXBindingUnit &bindingUnit = _bindingUnits[i];
            const GFXBinding &binding = info.bindings[i];
            bindingUnit.binding = binding.binding;
            bindingUnit.type = binding.type;
            bindingUnit.name = binding.name;
            bindingUnit.count = binding.count;
            bindingUnit.shaderStages = binding.shaderStages;
        }
    }

    _gpuBindingLayout = CC_NEW(CCVKGPUBindingLayout);
    CCVKCmdFuncCreateBindingLayout((CCVKDevice *)_device, _gpuBindingLayout, _bindingUnits);

    _status = GFXStatus::SUCCESS;

    return true;
}

void CCVKBindingLayout::destroy() {
    if (_gpuBindingLayout) {
        CCVKCmdFuncDestroyBindingLayout((CCVKDevice *)_device, _gpuBindingLayout);
        CC_DELETE(_gpuBindingLayout);
        _gpuBindingLayout = nullptr;
    }

    _status = GFXStatus::UNREADY;
}

void CCVKBindingLayout::update() {
    if (_isDirty && _gpuBindingLayout) {
        for (size_t i = 0u; i < _bindingUnits.size(); ++i) {
            GFXBindingUnit &bindingUnit = _bindingUnits[i];
            VkWriteDescriptorSet &binding = _gpuBindingLayout->bindings[i];

            switch (bindingUnit.type) {
                case GFXBindingType::UNIFORM_BUFFER: {
                    if (bindingUnit.buffer) {
                        CCVKGPUBuffer *buffer = ((CCVKBuffer *)bindingUnit.buffer)->gpuBuffer();
                        VkDescriptorBufferInfo *info = (VkDescriptorBufferInfo *)binding.pBufferInfo;

                        info->buffer = buffer->vkBuffer;
                        info->offset = 0;
                        info->range = buffer->size;
                    }

                    break;
                }
                case GFXBindingType::SAMPLER: {
                    VkDescriptorImageInfo *info = (VkDescriptorImageInfo *)binding.pImageInfo;
                    info->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                    if (bindingUnit.texture) {
                        info->imageView = ((CCVKTexture *)bindingUnit.texture)->gpuTextureView()->vkImageView;
                    } else {
                        info->imageView = ((CCVKDevice *)_device)->nullTexView2D->gpuTextureView()->vkImageView;
                    }

                    if (bindingUnit.sampler) {
                        info->sampler = ((CCVKSampler *)bindingUnit.sampler)->gpuSampler()->vkSampler;
                    }

                    break;
                }
            }
        }

        vkUpdateDescriptorSets(((CCVKDevice *)_device)->gpuDevice()->vkDevice,
                               _gpuBindingLayout->bindings.size(), _gpuBindingLayout->bindings.data(), 0, nullptr);

        _isDirty = false;
    }
}

NS_CC_END
