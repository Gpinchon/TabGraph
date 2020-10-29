#include "GameEntity.hpp"

class CrispyWall : public GameEntity {
public:
    CrispyWall();
    static std::shared_ptr<CrispyWall> Create();
    virtual void Die() override;
};