#pragma once
#include <cstdint>
#include "../graphics/MathStruct.h"

// 最小限の衝突形状データ。アルゴリズム実装は後で追加。
struct Collider {
    enum class Shape : uint8_t { None = 0, AABB, Sphere, Capsule };

    Shape shape{ Shape::AABB };

    // AABB 用の半径（ハーフエクステント）
    Float3 halfExtent{ 0.5f, 0.5f, 0.5f };

    // Sphere 用の半径
    float radius{ 0.5f };

    // Capsule 用のパラメータ（軸方向長さなど）
    float height{ 1.0f };

    bool isTrigger{ false }; // トリガー（検知のみ）

    // 将来的なAPI（未実装）
    // bool Intersects(const Collider& other, const Transform& selfTf, const Transform& otherTf) const;
};
