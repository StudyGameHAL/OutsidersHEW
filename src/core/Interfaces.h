#pragma once

// 空の役割インタフェース群。実装は各クラス側で行う想定。
struct IUpdatable {
    virtual ~IUpdatable() = default;
    virtual void Update(float /*dt*/) = 0; // 1フレーム分の更新。内部実装は後で
};

struct IRenderable {
    virtual ~IRenderable() = default;
    virtual void Draw() = 0; // 描画フック。内部実装は後で
};

struct ICollidable {
    virtual ~ICollidable() = default;
    virtual void OnCollision(ICollidable* /*other*/) = 0; // 衝突時のコールバック。内部実装は後で
};
