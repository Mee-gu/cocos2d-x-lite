#ifndef CC_GFXGLES2_GLES2_DEVICE_H_
#define CC_GFXGLES2_GLES2_DEVICE_H_

NS_CC_BEGIN

class GLES2StateCache;

class CC_GLES2_API GLES2Device : public GFXDevice {
public:
    GLES2Device();
    ~GLES2Device();

    GLES2StateCache *stateCache = nullptr;

public:
    virtual bool initialize(const GFXDeviceInfo &info) override;
    virtual void destroy() override;
    virtual void resize(uint width, uint height) override;
    virtual void acquire() override{};
    virtual void present() override;
    virtual GFXWindow *createWindow(const GFXWindowInfo &info) override;
    virtual GFXFence *createFence(const GFXFenceInfo &info) override;
    virtual GFXQueue *createQueue(const GFXQueueInfo &info) override;
    virtual GFXCommandAllocator *createCommandAllocator(const GFXCommandAllocatorInfo &info) override;
    virtual GFXCommandBuffer *createCommandBuffer(const GFXCommandBufferInfo &info) override;
    virtual GFXBuffer *createBuffer(const GFXBufferInfo &info) override;
    virtual GFXTexture *createTexture(const GFXTextureInfo &info) override;
    virtual GFXSampler *createSampler(const GFXSamplerInfo &info) override;
    virtual GFXShader *createShader(const GFXShaderInfo &info) override;
    virtual GFXInputAssembler *createInputAssembler(const GFXInputAssemblerInfo &info) override;
    virtual GFXRenderPass *createRenderPass(const GFXRenderPassInfo &info) override;
    virtual GFXFramebuffer *createFramebuffer(const GFXFramebufferInfo &info) override;
    virtual GFXBindingLayout *createBindingLayout(const GFXBindingLayoutInfo &info) override;
    virtual GFXPipelineState *createPipelineState(const GFXPipelineStateInfo &info) override;
    virtual GFXPipelineLayout *createPipelineLayout(const GFXPipelineLayoutInfo &info) override;
    virtual void copyBuffersToTexture(const GFXDataArray &buffers, GFXTexture *dst, const GFXBufferTextureCopyList &regions) override;

    CC_INLINE bool useVAO() const { return _useVAO; }
    CC_INLINE bool useDrawInstanced() const { return _useDrawInstanced; }
    CC_INLINE bool useInstancedArrays() const { return _useInstancedArrays; }
    CC_INLINE bool useDiscardFramebuffer() const { return _useDiscardFramebuffer; }

    CC_INLINE bool checkExtension(const String &extension) const {
        for (size_t i = 0; i < _extensions.size(); ++i) {
            if (_extensions[i].find(extension) != String::npos) {
                return true;
            }
        }
        return false;
    }

private:
    StringArray _extensions;
    bool _useVAO = false;
    bool _useDrawInstanced = false;
    bool _useInstancedArrays = false;
    bool _useDiscardFramebuffer = false;
};

NS_CC_END

#endif
