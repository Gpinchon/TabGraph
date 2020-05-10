#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Transform
{
public:
    virtual glm::mat4 GetTransformMatrix() const;
    virtual glm::mat4 GetTranslationMatrix() const;
    virtual glm::mat4 GetRotationMatrix() const;
    virtual glm::mat4 GetScaleMatrix() const;
	/** @return the node local position */
    virtual glm::vec3 GetPosition() const;
    /** @argument position : the node local position */
    virtual void SetPosition(glm::vec3 position);
    /** @return the node local rotation */
    virtual glm::quat GetRotation() const;
    /** @argument rotation : the node local rotation */
    virtual void SetRotation(glm::vec3 rotation);
    /** @return the node local scale */
    virtual void SetRotation(glm::quat rotation);
    /** @return the node local scale */
    virtual glm::vec3 GetScale() const;
    /** @argument scale : the node local scale */
    virtual void SetScale(glm::vec3 scale);
private:
	glm::vec3 _position { 0, 0, 0 };
    glm::quat _rotation { 0, 0, 0, 1 };
    glm::vec3 _scale { 1, 1, 1 };
};