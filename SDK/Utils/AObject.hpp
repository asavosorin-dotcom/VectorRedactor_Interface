#ifndef A_OBJECT_HPP
#define A_OBJECT_HPP

#include <list>
#include <memory>

#include "SDK/Utils/BoxF.hpp"
#include "SDK/Utils/Event.hpp"

class AObject {
private:
    BoxF box_;
    std::list<std::unique_ptr<AObject>> children_;
    AObject* parent_ = nullptr;

public:
    virtual ~AObject() = default;

    void setParent(AObject* new_parent) {
        parent_ = new_parent;
    }

    AObject* getParent() const {
        return parent_;
    }

    const std::list<std::unique_ptr<AObject>>& getChildren() const {
        return children_;
    }

    std::list<std::unique_ptr<AObject>>& getChildren() {
        return children_;
    }

    void addChild(std::unique_ptr<AObject> child) {
        child->setParent(this);
        children_.push_back(std::move(child));
    }

    virtual bool processEvent(const Event& event) {
        for (auto& child : children_) {
            if (child->processEvent(event)) {
                return true;
            }
        }
        return false;
    }

    virtual void render() {
        for (auto& child : children_) {
            child->render();
        }
    }
};

#endif