#include "GameEntity.hpp"

class CrispyWall : public GameEntity {
public:
    static std::shared_ptr<CrispyWall> Create();
    virtual void Die() override;

private:
    CrispyWall();
};