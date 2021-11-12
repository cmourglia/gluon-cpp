#pragma once

#include <gluon/lang/cell.h>
#include <gluon/lang/value.h>

#include <beard/containers/hash_map.h>

#include <string>

class Object : public Cell
{
public:
    Value Get(const std::string& name) const;
    void  Add(const std::string& property_name, Value value);

    virtual const char* ToString() const override
    {
        return "Object";
    }

    virtual bool IsFunction() const
    {
        return false;
    }

    void VisitGraph(VisitorCallback Callback) override;

private:
    beard::string_hash_map<Value> m_values;
};
