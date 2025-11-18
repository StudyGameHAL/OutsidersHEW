#pragma once
#include "../graphics/MathStruct.h"

// 物理的な運動データのみを保持する軽量クラス。実処理は未実装。
struct Rigidbody {
    Float3 velocity{ 0.0f, 0.0f, 0.0f };
    Float3 acceleration{ 0.0f, 0.0f, 0.0f };
    float mass{ 1.0f };
    bool isKinematic{ false }; // true の場合、物理シミュレーションから除外予定

    // 将来的なAPIの例（未実装）
    // void AddForce(const Float3& force);
};
