//
// Created by zyzyz on 2025/12/05.
//
#include "Collider.hpp"
#include "core/Transform.h"
#include <cmath>
#include <algorithm>

using namespace DirectX;

static inline bool NearlyEqual(float a, float b, float eps) {
    return std::fabs(a - b) <= eps;
}

static XMMATRIX MakeWorld(const XMFLOAT3 &pos, const XMFLOAT3 &euler, const XMFLOAT3 &scl) {
    XMMATRIX S = XMMatrixScaling(scl.x, scl.y, scl.z);
    XMMATRIX R = XMMatrixRotationRollPitchYaw(euler.x, euler.y, euler.z);
    XMMATRIX T = XMMatrixTranslation(pos.x, pos.y, pos.z);
    return S * R * T;
}

namespace {
    class SphereColliderImpl final : public SphereCollider {
    public:
        explicit SphereColliderImpl(float rLocal)
            : m_radiusLocal(std::max(0.0f, rLocal)) {
        }

        ColliderType kind() const override { return ColliderType::Sphere; }

        // ローカルオフセット：Ownerに対する相対位置
        bool setPosition(const XMFLOAT3 &pos) override {
            m_localOffset = pos;
            return true;
        }

        bool setRotationEuler(const XMFLOAT3 &rotEuler) override {
            m_localRot = rotEuler;
            return true;
        }

        bool setScale(const XMFLOAT3 &scale) override {
            float eps = GetPhysicsConfig().epsilon;
            if (!NearlyEqual(scale.x, scale.y, eps) || !NearlyEqual(scale.x, scale.z, eps)) return false;
            if (scale.x <= 0 || scale.y <= 0 || scale.z <= 0) return false;
            m_scl = scale;
            return true;
        }

        XMFLOAT3 position() const override { return m_localOffset; }
        XMFLOAT3 rotationEuler() const override { return m_localRot; }
        XMFLOAT3 scale() const override { return m_scl; }

        // Ownerワールド姿勢 + ローカルオフセット/回転を組み合わせ
        XMMATRIX world() const override {
            XMMATRIX S = XMMatrixScaling(m_scl.x, m_scl.y, m_scl.z);
            XMMATRIX Rowner = XMMatrixRotationRollPitchYaw(m_ownerRot.x, m_ownerRot.y, m_ownerRot.z);
            XMMATRIX Rlocal = XMMatrixRotationRollPitchYaw(m_localRot.x, m_localRot.y, m_localRot.z);
            XMMATRIX R = Rlocal * Rowner; // 先にローカル、次にOwnerに従う
            // ワールド中心 = ownerPos + Rowner * localOffset（Sphereは回転に対して不感、オフセットのみに使用）
            XMVECTOR off = XMVectorSet(m_localOffset.x, m_localOffset.y, m_localOffset.z, 0);
            off = XMVector3TransformNormal(off, Rowner);
            XMFLOAT3 centerW{m_ownerPos.x, m_ownerPos.y, m_ownerPos.z};
            XMVECTOR c = XMVectorSet(centerW.x, centerW.y, centerW.z, 1.0f);
            c = XMVectorAdd(c, off);
            XMFLOAT3 cw{};
            XMStoreFloat3(&cw, c);
            XMMATRIX T = XMMatrixTranslation(cw.x, cw.y, cw.z);
            return S * R * T;
        }

        bool updateDerived() override { return true; }

        Aabb aabb() const override {
            XMFLOAT3 c = centerWorld();
            float r = radiusWorld();
            return {XMFLOAT3{c.x - r, c.y - r, c.z - r}, XMFLOAT3{c.x + r, c.y + r, c.z + r}};
        }

        void setDebugEnabled(bool enabled) override { m_dbgEnabled = enabled; }
        bool debugEnabled() const override { return m_dbgEnabled; }
        void setDebugColor(const XMFLOAT4 &rgba) override { m_dbgColor = rgba; }
        XMFLOAT4 debugColor() const override { return m_dbgColor; }

        bool setOwnerOffset(const XMFLOAT3 &offset) override {
            m_ownerOffset = offset;
            return true;
        }

        XMFLOAT3 ownerOffset() const override { return m_ownerOffset; }

        void setIsTrigger(bool trigger) override { m_isTrigger = trigger; }
        bool isTrigger() const override { return m_isTrigger; }

        // Sphere固有メソッド
        float radiusLocal() const override { return m_radiusLocal; }
        float radiusWorld() const override { return m_radiusLocal * m_scl.x; }

        XMFLOAT3 centerWorld() const override {
            XMMATRIX Rowner = XMMatrixRotationRollPitchYaw(m_ownerRot.x, m_ownerRot.y, m_ownerRot.z);
            XMVECTOR off = XMVectorSet(m_localOffset.x, m_localOffset.y, m_localOffset.z, 0);
            off = XMVector3TransformNormal(off, Rowner);
            XMVECTOR base = XMVectorSet(m_ownerPos.x, m_ownerPos.y, m_ownerPos.z, 1.0f);
            XMVECTOR c = XMVectorAdd(base, off);
            XMFLOAT3 cw{};
            XMStoreFloat3(&cw, c);
            return cw;
        }

        // Ownerワールド姿勢の注入/読み取り
        void setOwnerWorldPosition(const XMFLOAT3 &ownerPosW) override { m_ownerPos = ownerPosW; }
        void setOwnerWorldRotationEuler(const XMFLOAT3 &ownerRotEulerW) override { m_ownerRot = ownerRotEulerW; }
        XMFLOAT3 ownerWorldPosition() const override { return m_ownerPos; }
        XMFLOAT3 ownerWorldRotationEuler() const override { return m_ownerRot; }

    private:
        // Ownerワールド姿勢
        XMFLOAT3 m_ownerPos{0, 0, 0};
        XMFLOAT3 m_ownerRot{0, 0, 0};
        // ローカルオフセット/回転：Ownerに対する相対位置
        XMFLOAT3 m_localOffset{0, 0, 0};
        XMFLOAT3 m_localRot{0, 0, 0};
        XMFLOAT3 m_scl{1, 1, 1};
        float m_radiusLocal{0};
        bool m_dbgEnabled{false};
        XMFLOAT4 m_dbgColor{0, 0.5f, 1, 1};
        XMFLOAT3 m_ownerOffset{0, 0, 0};
        bool m_isTrigger{false};
    };

    class ObbColliderImpl final : public ObbCollider {
    public:
        explicit ObbColliderImpl(const XMFLOAT3 &heLocal) : m_halfLocal(heLocal) {
        }

        ColliderType kind() const override { return ColliderType::Obb; }

        bool setPosition(const XMFLOAT3 &pos) override {
            m_localOffset = pos;
            return true;
        }

        bool setRotationEuler(const XMFLOAT3 &rotEuler) override {
            m_localRot = rotEuler;
            return true;
        }

        bool setScale(const XMFLOAT3 &scale) override {
            if (scale.x <= 0 || scale.y <= 0 || scale.z <= 0) return false;
            m_scl = scale;
            return true;
        }

        XMFLOAT3 position() const override { return m_localOffset; }
        XMFLOAT3 rotationEuler() const override { return m_localRot; }
        XMFLOAT3 scale() const override { return m_scl; }

        XMMATRIX world() const override {
            XMMATRIX S = XMMatrixScaling(m_scl.x, m_scl.y, m_scl.z);
            XMMATRIX Rowner = XMMatrixRotationRollPitchYaw(m_ownerRot.x, m_ownerRot.y, m_ownerRot.z);
            XMMATRIX Rlocal = XMMatrixRotationRollPitchYaw(m_localRot.x, m_localRot.y, m_localRot.z);
            XMMATRIX R = Rlocal * Rowner;
            XMVECTOR off = XMVectorSet(m_localOffset.x, m_localOffset.y, m_localOffset.z, 0);
            off = XMVector3TransformNormal(off, Rowner);
            XMVECTOR base = XMVectorSet(m_ownerPos.x, m_ownerPos.y, m_ownerPos.z, 1.0f);
            XMVECTOR c = XMVectorAdd(base, off);
            XMFLOAT3 cw{};
            XMStoreFloat3(&cw, c);
            XMMATRIX T = XMMatrixTranslation(cw.x, cw.y, cw.z);
            return S * R * T;
        }

        bool updateDerived() override { return true; }

        Aabb aabb() const override {
            // ワールド中心と軸と半範囲を使用してワールド空間軸平行境界ボックスを計算
            XMFLOAT3 center = centerWorld();
            XMFLOAT3 heW = halfExtentsWorld();
            XMFLOAT3 axes[3];
            axesWorld(axes);
            // ワールド軸に沿った範囲
            XMFLOAT3 e{};
            e.x = std::fabs(axes[0].x) * heW.x + std::fabs(axes[1].x) * heW.y + std::fabs(axes[2].x) * heW.z;
            e.y = std::fabs(axes[0].y) * heW.x + std::fabs(axes[1].y) * heW.y + std::fabs(axes[2].y) * heW.z;
            e.z = std::fabs(axes[0].z) * heW.x + std::fabs(axes[1].z) * heW.y + std::fabs(axes[2].z) * heW.z;
            return {
                XMFLOAT3{center.x - e.x, center.y - e.y, center.z - e.z},
                XMFLOAT3{center.x + e.x, center.y + e.y, center.z + e.z}
            };
        }

        void setDebugEnabled(bool enabled) override { m_dbgEnabled = enabled; }
        bool debugEnabled() const override { return m_dbgEnabled; }
        void setDebugColor(const XMFLOAT4 &rgba) override { m_dbgColor = rgba; }
        XMFLOAT4 debugColor() const override { return m_dbgColor; }

        bool setOwnerOffset(const XMFLOAT3 &offset) override {
            m_ownerOffset = offset;
            return true;
        }

        XMFLOAT3 ownerOffset() const override { return m_ownerOffset; }

        void setIsTrigger(bool trigger) override { m_isTrigger = trigger; }
        bool isTrigger() const override { return m_isTrigger; }

        // Ownerワールド姿勢の注入/読み取り
        void setOwnerWorldPosition(const XMFLOAT3 &ownerPosW) override { m_ownerPos = ownerPosW; }
        void setOwnerWorldRotationEuler(const XMFLOAT3 &ownerRotEulerW) override { m_ownerRot = ownerRotEulerW; }
        XMFLOAT3 ownerWorldPosition() const override { return m_ownerPos; }
        XMFLOAT3 ownerWorldRotationEuler() const override { return m_ownerRot; }

        // OBB固有メソッド
        XMFLOAT3 centerWorld() const override {
            XMMATRIX Rowner = XMMatrixRotationRollPitchYaw(m_ownerRot.x, m_ownerRot.y, m_ownerRot.z);
            XMVECTOR off = XMVectorSet(m_localOffset.x, m_localOffset.y, m_localOffset.z, 0);
            off = XMVector3TransformNormal(off, Rowner);
            XMVECTOR base = XMVectorSet(m_ownerPos.x, m_ownerPos.y, m_ownerPos.z, 1.0f);
            XMVECTOR c = XMVectorAdd(base, off);
            XMFLOAT3 cw{};
            XMStoreFloat3(&cw, c);
            return cw;
        }

        void axesWorld(XMFLOAT3 outAxes[3]) const override {
            XMMATRIX Rowner = XMMatrixRotationRollPitchYaw(m_ownerRot.x, m_ownerRot.y, m_ownerRot.z);
            XMMATRIX Rlocal = XMMatrixRotationRollPitchYaw(m_localRot.x, m_localRot.y, m_localRot.z);
            XMMATRIX R = Rlocal * Rowner;
            // 単位基底を変換してワールド空間の向き取得（スケールと移動を無視）
            XMVECTOR x = XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), R);
            XMVECTOR y = XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), R);
            XMVECTOR z = XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), R);
            XMStoreFloat3(&outAxes[0], XMVector3Normalize(x));
            XMStoreFloat3(&outAxes[1], XMVector3Normalize(y));
            XMStoreFloat3(&outAxes[2], XMVector3Normalize(z));
        }

        XMFLOAT3 halfExtentsWorld() const override {
            return XMFLOAT3{
                std::fabs(m_scl.x) * m_halfLocal.x,
                std::fabs(m_scl.y) * m_halfLocal.y,
                std::fabs(m_scl.z) * m_halfLocal.z
            };
        }

    private:
        XMFLOAT3 m_ownerPos{0, 0, 0};
        XMFLOAT3 m_ownerRot{0, 0, 0};
        XMFLOAT3 m_localOffset{0, 0, 0};
        XMFLOAT3 m_localRot{0, 0, 0};
        XMFLOAT3 m_scl{1, 1, 1};
        XMFLOAT3 m_halfLocal{0.5f, 0.5f, 0.5f};
        bool m_dbgEnabled{false};
        XMFLOAT4 m_dbgColor{0, 1, 0, 1};
        XMFLOAT3 m_ownerOffset{0, 0, 0};
        bool m_isTrigger{false};
    };

    class CapsuleColliderImpl final : public CapsuleCollider {
    public:
        CapsuleColliderImpl(const XMFLOAT3 &p0Local, const XMFLOAT3 &p1Local, float radiusLocal)
            : m_p0Local(p0Local), m_p1Local(p1Local), m_radiusLocal(std::max(0.0f, radiusLocal)) {
        }

        ColliderType kind() const override { return ColliderType::Capsule; }

        bool setPosition(const XMFLOAT3 &pos) override {
            m_localOffset = pos;
            return true;
        }

        bool setRotationEuler(const XMFLOAT3 &rotEuler) override {
            m_localRot = rotEuler;
            return true;
        }

        // 簡略化された制約：任意の2つのスケール成分が等しいことのみ要求（半径方向として）
        bool setScale(const XMFLOAT3 &scale) override {
            if (scale.x <= 0 || scale.y <= 0 || scale.z <= 0) return false;
            const float eps = GetPhysicsConfig().epsilon;
            // 1つの軸が独立（長軸）、他の2つは等しくなければならない（半径）
            bool valid = NearlyEqual(scale.x, scale.y, eps) ||
                         NearlyEqual(scale.y, scale.z, eps) ||
                         NearlyEqual(scale.x, scale.z, eps);
            if (!valid) return false;
            m_scl = scale;
            return true;
        }

        XMFLOAT3 position() const override { return m_localOffset; }
        XMFLOAT3 rotationEuler() const override { return m_localRot; }
        XMFLOAT3 scale() const override { return m_scl; }

        XMMATRIX world() const override {
            XMMATRIX S = XMMatrixScaling(m_scl.x, m_scl.y, m_scl.z);
            XMMATRIX Rowner = XMMatrixRotationRollPitchYaw(m_ownerRot.x, m_ownerRot.y, m_ownerRot.z);
            XMMATRIX Rlocal = XMMatrixRotationRollPitchYaw(m_localRot.x, m_localRot.y, m_localRot.z);
            XMMATRIX R = Rlocal * Rowner;
            XMVECTOR off = XMVectorSet(m_localOffset.x, m_localOffset.y, m_localOffset.z, 0);
            off = XMVector3TransformNormal(off, Rowner);
            XMVECTOR base = XMVectorSet(m_ownerPos.x, m_ownerPos.y, m_ownerPos.z, 1.0f);
            XMVECTOR c = XMVectorAdd(base, off);
            XMFLOAT3 cw{};
            XMStoreFloat3(&cw, c);
            XMMATRIX T = XMMatrixTranslation(cw.x, cw.y, cw.z);
            return S * R * T;
        }

        bool updateDerived() override { return true; }

        Aabb aabb() const override {
            auto seg = segmentWorld();
            XMFLOAT3 p0 = seg.first, p1 = seg.second;
            float r = radiusWorld();
            XMFLOAT3 mn{std::min(p0.x, p1.x) - r, std::min(p0.y, p1.y) - r, std::min(p0.z, p1.z) - r};
            XMFLOAT3 mx{std::max(p0.x, p1.x) + r, std::max(p0.y, p1.y) + r, std::max(p0.z, p1.z) + r};
            return {mn, mx};
        }

        void setDebugEnabled(bool enabled) override { m_dbgEnabled = enabled; }
        bool debugEnabled() const override { return m_dbgEnabled; }
        void setDebugColor(const XMFLOAT4 &rgba) override { m_dbgColor = rgba; }
        XMFLOAT4 debugColor() const override { return m_dbgColor; }

        bool setOwnerOffset(const XMFLOAT3 &offset) override {
            m_ownerOffset = offset;
            return true;
        }

        XMFLOAT3 ownerOffset() const override { return m_ownerOffset; }

        void setIsTrigger(bool trigger) override { m_isTrigger = trigger; }
        bool isTrigger() const override { return m_isTrigger; }

        // Ownerワールド姿勢の注入/読み取り
        void setOwnerWorldPosition(const XMFLOAT3 &ownerPosW) override { m_ownerPos = ownerPosW; }
        void setOwnerWorldRotationEuler(const XMFLOAT3 &ownerRotEulerW) override { m_ownerRot = ownerRotEulerW; }
        XMFLOAT3 ownerWorldPosition() const override { return m_ownerPos; }
        XMFLOAT3 ownerWorldRotationEuler() const override { return m_ownerRot; }

        // Capsule固有メソッド
        std::pair<XMFLOAT3, XMFLOAT3> segmentWorld() const override {
            XMMATRIX S = XMMatrixScaling(m_scl.x, m_scl.y, m_scl.z);
            XMMATRIX Rowner = XMMatrixRotationRollPitchYaw(m_ownerRot.x, m_ownerRot.y, m_ownerRot.z);
            XMMATRIX Rlocal = XMMatrixRotationRollPitchYaw(m_localRot.x, m_localRot.y, m_localRot.z);
            XMMATRIX R = Rlocal * Rowner;
            XMVECTOR off = XMVectorSet(m_localOffset.x, m_localOffset.y, m_localOffset.z, 0);
            off = XMVector3TransformNormal(off, Rowner);
            XMVECTOR base = XMVectorSet(m_ownerPos.x, m_ownerPos.y, m_ownerPos.z, 1.0f);
            XMVECTOR c = XMVectorAdd(base, off);
            XMFLOAT3 cw{};
            XMStoreFloat3(&cw, c);
            XMMATRIX T = XMMatrixTranslation(cw.x, cw.y, cw.z);
            XMMATRIX M = S * R * T;
            XMVECTOR p0 = XMVectorSet(m_p0Local.x, m_p0Local.y, m_p0Local.z, 1.0f);
            XMVECTOR p1 = XMVectorSet(m_p1Local.x, m_p1Local.y, m_p1Local.z, 1.0f);
            p0 = XMVector3TransformCoord(p0, M);
            p1 = XMVector3TransformCoord(p1, M);
            XMFLOAT3 o0{}, o1{};
            XMStoreFloat3(&o0, p0);
            XMStoreFloat3(&o1, p1);
            return {o0, o1};
        }

        float radiusWorld() const override {
            // 半径方向縮放を選択：長軸と正交する等比尺度
            XMFLOAT3 axis = localAxisUnit();
            float ax = std::fabs(axis.x), ay = std::fabs(axis.y), az = std::fabs(axis.z);
            float rScale = m_scl.x; // デフォルト
            if (ax >= ay && ax >= az) {
                // 長軸 ~ X、径方向はY/Zを使用 (等価性はsetScaleメソッドで検証済み)
                rScale = m_scl.y; // == m_scl.z
            } else if (ay >= ax && ay >= az) {
                rScale = m_scl.x; // == m_scl.z
            } else {
                rScale = m_scl.x; // == m_scl.y
            }
            return m_radiusLocal * rScale;
        }

    private:
        XMFLOAT3 localAxisUnit() const {
            XMVECTOR p0 = XMVectorSet(m_p0Local.x, m_p0Local.y, m_p0Local.z, 0);
            XMVECTOR p1 = XMVectorSet(m_p1Local.x, m_p1Local.y, m_p1Local.z, 0);
            XMVECTOR d = XMVectorSubtract(p1, p0);
            d = XMVector3Normalize(d);
            XMFLOAT3 u{};
            XMStoreFloat3(&u, d);
            return u;
        }

        XMFLOAT3 m_ownerPos{0, 0, 0};
        XMFLOAT3 m_ownerRot{0, 0, 0};
        XMFLOAT3 m_localOffset{0, 0, 0};
        XMFLOAT3 m_localRot{0, 0, 0};
        XMFLOAT3 m_scl{1, 1, 1};
        XMFLOAT3 m_p0Local{0, -0.5f, 0};
        XMFLOAT3 m_p1Local{0, 0.5f, 0};
        float m_radiusLocal{0.5f};
        bool m_dbgEnabled{false};
        XMFLOAT4 m_dbgColor{1, 0.5f, 0, 1};
        XMFLOAT3 m_ownerOffset{0, 0, 0};
        bool m_isTrigger{false};
    };
} // namespace

// ---- ファクトリー関数 ----
std::unique_ptr<SphereCollider> MakeSphereCollider(float radiusLocal) {
    return std::make_unique<SphereColliderImpl>(radiusLocal);
}

std::unique_ptr<ObbCollider> MakeObbCollider(const XMFLOAT3 &halfExtentsLocal) {
    return std::make_unique<ObbColliderImpl>(halfExtentsLocal);
}

std::unique_ptr<CapsuleCollider> MakeCapsuleCollider(const XMFLOAT3 &p0Local, const XMFLOAT3 &p1Local,
                                                     float radiusLocal) {
    return std::make_unique<CapsuleColliderImpl>(p0Local, p1Local, radiusLocal);
}