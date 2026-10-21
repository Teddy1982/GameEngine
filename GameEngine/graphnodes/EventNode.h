#pragma once

#include "GraphNodeBase.h"

#include "../vulkan/Enums.h"
#include "../model/AssimpInstance.h"
#include "../vulkan/ModelInstanceCamData.h"

class EventNode : public GraphNodeBase {
  public:
    EventNode(int nodeId, float cooldown = 1.0f);

    virtual void draw(ModelInstanceCamData modInstCamData) override;
    virtual void update(float deltaTime) override;
    virtual void activate() override {};
    virtual void deactivate(bool informParentNodes) override;
    virtual bool isActive() override { return mEventTriggered; };
    virtual std::shared_ptr<GraphNodeBase> clone() override;
    virtual std::optional<std::map<std::string, std::string>> exportData() override;
    virtual void importData(std::map<std::string, std::string> data) override;

    virtual bool listensToEvent(nodeEvent event) override;
    virtual void handleEvent() override;

  private:
    int mStaticIdStart = 0;
    int mOutId = 0;

    nodeEvent mTriggerEvent = nodeEvent::none;
    bool mEventTriggered = false;

    float mEventCooldown = 0.0f;
    float mCooldown = 0.0f;
};
