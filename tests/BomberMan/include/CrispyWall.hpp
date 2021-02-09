#include "GameEntity.hpp"

class CrispyWall : public GameEntity {
public:
    CrispyWall();
    static std::shared_ptr<CrispyWall> Create();
    virtual void Die() override;
private:
    virtual std::shared_ptr<Component> _Clone() override {
        return std::shared_ptr<CrispyWall>(new CrispyWall(*this));
    }
};