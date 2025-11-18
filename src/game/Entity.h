#pragma once
#include <string>
#include "../core/Interfaces.h"
#include "../core/Transform.h"
#include "../core/Collider.h"

// ゲーム内の基本エンティティ。
// 必須の役割: 更新・描画・衝突応答。
class Entity : public IRenderable, public IUpdatable, public ICollidable {
public:
    Entity() = default;
    virtual ~Entity() = default;

    // IUpdatable
    void Update(float /*dt*/) override {}

    // IRenderable
    void Draw() override {}

    // ICollidable
    void OnCollision(ICollidable* /*other*/) override {}

    // 付帯データ
    Transform transform{};
    Collider collider{};

    // 便利フィールド
    bool active{ true };
    std::string name{};
};
