#pragma once

#include <string>

class Layer
{
public:
    Layer(const std::string &name = "Layer");
    virtual ~Layer();

    virtual void OnAttach(){};
    virtual void OnDetack(){};
    virtual void OnUpdate(){};

    inline const std::string &GetName() const { return m_DebugName; }

protected:
    std::string m_DebugName;
};