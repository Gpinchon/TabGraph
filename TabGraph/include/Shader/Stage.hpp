#pragma once

#include "Property.hpp"

#include <string>
#include <map>

namespace Shader {
    class Stage {
    public:
        /**
         * code : the code to be added before main (functions for instance)
         * technique : what is to be added inside of main (function calls for instance)
        */
        struct Code {
            std::string code;
            std::string technique;
            bool operator!=(const Code& other) {
                return code != other.code || technique != other.technique;
            }
            Code& operator+(const Code& other) {
                Code newCode{ code, technique };
                return newCode += other;
            }
            Code& operator-(const Code& other) {
                Code newCode{ code, technique };
                return newCode -= other;
            }
            Code& operator-=(const Code& other) {
                size_t pos = code.find(other.code);
                if (pos != std::string::npos)
                    code.erase(pos, code.length());
                pos = technique.find(other.technique);
                if (pos != std::string::npos)
                    technique.erase(pos, technique.length());
                return *this;
            }
            Code& operator+=(const Code& other) {
                code += other.code;
                technique += other.technique;
                return *this;
            }
        };
        //ORDER IS IMPORTANT
        //Add value between last value and MaxValue
        enum class Type {
            None = -1,
            Geometry,
            Vertex,
            Fragment,
            Compute,
            TessellationEvaluation,
            TessellationControl,
            MaxValue
        };
        PROPERTY(Stage::Type, Type, Stage::Type::None);
        PROPERTY(Code, Code, { "", "" });

    public:
        Stage(const Stage& other);
        Stage(Stage::Type type = Stage::Type::None, const Code& code = { "", "" });
        Stage& operator=(const Stage& other);
        Stage& SetDefine(const std::string& name, const std::string& value = "");
        std::string GetDefine(const std::string& name) const;
        const std::map<std::string, std::string>& GetDefines() const;
        Stage& operator+=(const Code& code);
        Stage& operator-=(const Code& code);

    private:
        std::map<std::string, std::string> _defines{};
    };
};
