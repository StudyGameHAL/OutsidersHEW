#pragma once
#include <cstdint>
#include "../graphics/MathStruct.h"

// データのみの Transform。実処理は未実装のまま置いておく。
struct Transform {
    // ワールド位置・回転(Euler)・拡縮
    Float3 position{ 0.0f, 0.0f, 0.0f };
    Float3 rotation{ 0.0f, 0.0f, 0.0f }; // yaw-pitch-roll などの取り扱いは未定
    Float3 scale{ 1.0f, 1.0f, 1.0f };

    // 例: 行列を取得する予定のAPI（実装は後で）
    // Matrix WorldMatrix() const; // 未実装
};
