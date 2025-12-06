//
// Created by zyzyz on 2025/12/05.
//

#include "Collider.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

using namespace DirectX;

namespace {
    // グローバル物理設定（epsilon調整可能）
    PhysicsConfig g_physicsConfig{}; // デフォルト epsilon=1e-5f

    // ---- ユーティリティ ----
    using namespace DirectX;

    inline XMVECTOR Load3(const XMFLOAT3 &v) { return XMVectorSet(v.x, v.y, v.z, 0.0f); }

    inline XMFLOAT3 ToFloat3(FXMVECTOR v) {
        XMFLOAT3 o{};
        XMStoreFloat3(&o, v);
        return o;
    }

    inline float Clamp(float x, float a, float b) { return std::fmax(a, std::fmin(b, x)); }

    // 点を区間[-e,e]にクランプ
    inline float ClampToExtent(float x, float e) { return Clamp(x, -e, e); }

    // 最近点：点からOBB（中心+軸+半サイズを使用）
    inline XMFLOAT3 ClosestPointOnObb(const XMFLOAT3 &p,
                                      const XMFLOAT3 &c,
                                      const XMFLOAT3 axes[3],
                                      const XMFLOAT3 &he) {
        XMVECTOR P = Load3(p);
        XMVECTOR C = Load3(c);
        XMVECTOR d = XMVectorSubtract(P, C);
        float qx = 0, qy = 0, qz = 0;
        // 各軸に投影してクランプ
        XMVECTOR ax = Load3(axes[0]);
        XMVECTOR ay = Load3(axes[1]);
        XMVECTOR az = Load3(axes[2]);
        float x = XMVectorGetX(XMVector3Dot(d, ax));
        float y = XMVectorGetX(XMVector3Dot(d, ay));
        float z = XMVectorGetX(XMVector3Dot(d, az));
        qx = ClampToExtent(x, he.x);
        qy = ClampToExtent(y, he.y);
        qz = ClampToExtent(z, he.z);
        XMVECTOR Q = XMVectorAdd(C, XMVectorAdd(XMVectorScale(ax, qx),
                                                XMVectorAdd(XMVectorScale(ay, qy), XMVectorScale(az, qz))));
        return ToFloat3(Q);
    }

    // 点から線分への最近距離の二乗
    inline float DistPointSegmentSq(const XMFLOAT3 &p, const XMFLOAT3 &a, const XMFLOAT3 &b) {
        XMVECTOR P = Load3(p);
        XMVECTOR A = Load3(a);
        XMVECTOR B = Load3(b);
        XMVECTOR AB = XMVectorSubtract(B, A);
        XMVECTOR AP = XMVectorSubtract(P, A);
        float ab2 = XMVectorGetX(XMVector3Dot(AB, AB));
        float t = ab2 > 0 ? XMVectorGetX(XMVector3Dot(AP, AB)) / ab2 : 0.0f;
        t = Clamp(t, 0.0f, 1.0f);
        XMVECTOR Q = XMVectorAdd(A, XMVectorScale(AB, t));
        XMVECTOR d = XMVectorSubtract(P, Q);
        return XMVectorGetX(XMVector3Dot(d, d));
    }

    // 線分-線分間の最近距離の二乗（オプションで最近点とパラメータを出力）
    inline float DistSegmentSegmentSq(const XMFLOAT3 &p1, const XMFLOAT3 &q1,
                                      const XMFLOAT3 &p2, const XMFLOAT3 &q2,
                                      float *outS = nullptr, float *outT = nullptr,
                                      XMFLOAT3 *outC1 = nullptr, XMFLOAT3 *outC2 = nullptr) {
        // Ericson 5.1.9
        XMVECTOR P1 = Load3(p1), Q1 = Load3(q1), P2 = Load3(p2), Q2 = Load3(q2);
        XMVECTOR d1 = XMVectorSubtract(Q1, P1);
        XMVECTOR d2 = XMVectorSubtract(Q2, P2);
        XMVECTOR r = XMVectorSubtract(P1, P2);
        float a = XMVectorGetX(XMVector3Dot(d1, d1));
        float e = XMVectorGetX(XMVector3Dot(d2, d2));
        float f = XMVectorGetX(XMVector3Dot(d2, r));
        float s, t;
        float eps = GetPhysicsConfig().epsilon;
        if (a <= eps && e <= eps) {
            // both degenerate
            if (outS) *outS = 0.0f;
            if (outT) *outT = 0.0f;
            if (outC1) *outC1 = XMFLOAT3{XMVectorGetX(P1), XMVectorGetY(P1), XMVectorGetZ(P1)};
            if (outC2) *outC2 = XMFLOAT3{XMVectorGetX(P2), XMVectorGetY(P2), XMVectorGetZ(P2)};
            return XMVectorGetX(XMVector3LengthSq(r));
        }
        if (a <= eps) {
            s = 0.0f;
            t = Clamp(f / e, 0.0f, 1.0f);
        } else {
            float c = XMVectorGetX(XMVector3Dot(d1, r));
            if (e <= eps) {
                t = 0.0f;
                s = Clamp(-c / a, 0.0f, 1.0f);
            } else {
                float b = XMVectorGetX(XMVector3Dot(d1, d2));
                float denom = a * e - b * b;
                if (std::fabs(denom) > eps)
                    s = Clamp((b * f - c * e) / denom, 0.0f, 1.0f);
                else
                    s = 0.0f;
                t = (b * s + f) / e;
                if (t < 0.0f) {
                    t = 0.0f;
                    s = Clamp(-c / a, 0.0f, 1.0f);
                } else if (t > 1.0f) {
                    t = 1.0f;
                    s = Clamp((b - c) / a, 0.0f, 1.0f);
                }
            }
        }
        XMVECTOR c1 = XMVectorAdd(P1, XMVectorScale(d1, s));
        XMVECTOR c2 = XMVectorAdd(P2, XMVectorScale(d2, t));
        XMVECTOR d = XMVectorSubtract(c1, c2);
        if (outS) *outS = s;
        if (outT) *outT = t;
        if (outC1) XMStoreFloat3(outC1, c1);
        if (outC2) XMStoreFloat3(outC2, c2);
        return XMVectorGetX(XMVector3Dot(d, d));
    }

    // 線分とAABB（中心が原点、half extents=he）の交差テスト（slab法）、入力はローカル空間座標
    inline bool SegmentAabbIntersect(const XMFLOAT3 &p0, const XMFLOAT3 &p1, const XMFLOAT3 &he) {
        float tmin = 0.0f, tmax = 1.0f;
        float eps = GetPhysicsConfig().epsilon;
        XMFLOAT3 d{p1.x - p0.x, p1.y - p0.y, p1.z - p0.z};
        auto slab = [&](float p, float dp, float heAxis)-> bool {
            if (std::fabs(dp) < eps) {
                // 平行：始点は区間内でなければならない
                return (-heAxis <= p && p <= heAxis);
            }
            float invD = 1.0f / dp;
            float t1 = (-heAxis - p) * invD;
            float t2 = (heAxis - p) * invD;
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            return tmin <= tmax;
        };
        if (!slab(p0.x, d.x, he.x)) return false;
        if (!slab(p0.y, d.y, he.y)) return false;
        if (!slab(p0.z, d.z, he.z)) return false;
        return tmax >= 0.0f && tmin <= 1.0f;
    }

    // OBB-OBB SATブールテスト
    struct SatInfo {
        bool intersects = false;
        float penetration = 0;
        XMFLOAT3 axis{0, 0, 0};
    };

    inline bool IntersectObbObb(const ObbCollider &A, const ObbCollider &B) {
        XMFLOAT3 CA = A.centerWorld();
        XMFLOAT3 CB = B.centerWorld();
        XMFLOAT3 EA = A.halfExtentsWorld();
        XMFLOAT3 EB = B.halfExtentsWorld();
        XMFLOAT3 a[3], b[3];
        A.axesWorld(a);
        B.axesWorld(b);

        // 回転行列: Rij = Ai dot Bj
        float R[3][3];
        float AbsR[3][3];
        float eps = GetPhysicsConfig().epsilon;
        for (int i = 0; i < 3; ++i) {
            XMVECTOR ai = Load3(a[i]);
            for (int j = 0; j < 3; ++j) {
                float rij = XMVectorGetX(XMVector3Dot(ai, Load3(b[j])));
                R[i][j] = rij;
                AbsR[i][j] = std::fabs(rij) + 1e-6f; // 共線近似に対応するため小さな余裕を追加
            }
        }
        // 並進ベクトル t（AからB）、Aの座標系で表現：tA = dot(CB-CA, Ai)
        XMVECTOR tVec = XMVectorSubtract(Load3(CB), Load3(CA));
        float tA[3] = {
            XMVectorGetX(XMVector3Dot(tVec, Load3(a[0]))),
            XMVectorGetX(XMVector3Dot(tVec, Load3(a[1]))),
            XMVectorGetX(XMVector3Dot(tVec, Load3(a[2])))
        };

        // Aの面法線軸をテスト
        float ra, rb;
        // A0..A2
        ra = EA.x;
        rb = EB.x * AbsR[0][0] + EB.y * AbsR[0][1] + EB.z * AbsR[0][2];
        if (std::fabs(tA[0]) > ra + rb) return false;
        ra = EA.y;
        rb = EB.x * AbsR[1][0] + EB.y * AbsR[1][1] + EB.z * AbsR[1][2];
        if (std::fabs(tA[1]) > ra + rb) return false;
        ra = EA.z;
        rb = EB.x * AbsR[2][0] + EB.y * AbsR[2][1] + EB.z * AbsR[2][2];
        if (std::fabs(tA[2]) > ra + rb) return false;

        // Bの面法線軸
        float tB0 = tA[0] * R[0][0] + tA[1] * R[1][0] + tA[2] * R[2][0];
        float tB1 = tA[0] * R[0][1] + tA[1] * R[1][1] + tA[2] * R[2][1];
        float tB2 = tA[0] * R[0][2] + tA[1] * R[1][2] + tA[2] * R[2][2];
        ra = EA.x * AbsR[0][0] + EA.y * AbsR[1][0] + EA.z * AbsR[2][0];
        rb = EB.x;
        if (std::fabs(tB0) > ra + rb) return false;
        ra = EA.x * AbsR[0][1] + EA.y * AbsR[1][1] + EA.z * AbsR[2][1];
        rb = EB.y;
        if (std::fabs(tB1) > ra + rb) return false;
        ra = EA.x * AbsR[0][2] + EA.y * AbsR[1][2] + EA.z * AbsR[2][2];
        rb = EB.z;
        if (std::fabs(tB2) > ra + rb) return false;

        // 9つの外積軸 Ai x Bj
        // 軸長がゼロでない場合のみテスト（AbsRにepsを加えて退化を処理済み）
        // A0 x B0..B2
        ra = EA.y * AbsR[2][0] + EA.z * AbsR[1][0];
        rb = EB.y * AbsR[0][2] + EB.z * AbsR[0][1];
        if (std::fabs(tA[2] * R[1][0] - tA[1] * R[2][0]) > ra + rb) return false;
        ra = EA.y * AbsR[2][1] + EA.z * AbsR[1][1];
        rb = EB.x * AbsR[0][2] + EB.z * AbsR[0][0];
        if (std::fabs(tA[2] * R[1][1] - tA[1] * R[2][1]) > ra + rb) return false;
        ra = EA.y * AbsR[2][2] + EA.z * AbsR[1][2];
        rb = EB.x * AbsR[0][1] + EB.y * AbsR[0][0];
        if (std::fabs(tA[2] * R[1][2] - tA[1] * R[2][2]) > ra + rb) return false;
        // A1 x B0..B2
        ra = EA.x * AbsR[2][0] + EA.z * AbsR[0][0];
        rb = EB.y * AbsR[1][2] + EB.z * AbsR[1][1];
        if (std::fabs(tA[0] * R[2][0] - tA[2] * R[0][0]) > ra + rb) return false;
        ra = EA.x * AbsR[2][1] + EA.z * AbsR[0][1];
        rb = EB.x * AbsR[1][2] + EB.z * AbsR[1][0];
        if (std::fabs(tA[0] * R[2][1] - tA[2] * R[0][1]) > ra + rb) return false;
        ra = EA.x * AbsR[2][2] + EA.z * AbsR[0][2];
        rb = EB.x * AbsR[1][1] + EB.y * AbsR[1][0];
        if (std::fabs(tA[0] * R[2][2] - tA[2] * R[0][2]) > ra + rb) return false;
        // A2 x B0..B2
        ra = EA.x * AbsR[1][0] + EA.y * AbsR[0][0];
        rb = EB.y * AbsR[2][2] + EB.z * AbsR[2][1];
        if (std::fabs(tA[1] * R[0][0] - tA[0] * R[1][0]) > ra + rb) return false;
        ra = EA.x * AbsR[1][1] + EA.y * AbsR[0][1];
        rb = EB.x * AbsR[2][2] + EB.z * AbsR[2][0];
        if (std::fabs(tA[1] * R[0][1] - tA[0] * R[1][1]) > ra + rb) return false;
        ra = EA.x * AbsR[1][2] + EA.y * AbsR[0][2];
        rb = EB.x * AbsR[2][1] + EB.y * AbsR[2][0];
        if (std::fabs(tA[1] * R[0][2] - tA[0] * R[1][2]) > ra + rb) return false;

        return true; // 分離軸なし
    }

    // SAT 最浅貫通軸と深度を同時に出力（ワールド空間）
    inline SatInfo ObbObbSatWithAxis(const ObbCollider &A, const ObbCollider &B) {
        SatInfo info{};
        XMFLOAT3 CA = A.centerWorld();
        XMFLOAT3 CB = B.centerWorld();
        XMFLOAT3 EA = A.halfExtentsWorld();
        XMFLOAT3 EB = B.halfExtentsWorld();
        XMFLOAT3 a[3], b[3];
        A.axesWorld(a);
        B.axesWorld(b);
        float R[3][3];
        float AbsR[3][3];
        float minPen = std::numeric_limits<float>::infinity();
        XMFLOAT3 minAxis{0, 0, 0};
        XMVECTOR tVec = XMVectorSubtract(Load3(CB), Load3(CA));
        float tA[3];
        for (int i = 0; i < 3; ++i) {
            XMVECTOR ai = Load3(a[i]);
            tA[i] = XMVectorGetX(XMVector3Dot(tVec, ai));
            for (int j = 0; j < 3; ++j) {
                float rij = XMVectorGetX(XMVector3Dot(ai, Load3(b[j])));
                R[i][j] = rij;
                AbsR[i][j] = std::fabs(rij) + 1e-6f;
            }
        }
        auto testAxis = [&](XMFLOAT3 axis, float dist, float ra, float rb) {
            float overlap = ra + rb - std::fabs(dist);
            if (overlap < 0) {
                info.intersects = false;
                return false;
            }
            if (overlap < minPen) {
                minPen = overlap;
                minAxis = axis;
            }
            return true;
        };
        // A's face axes
        if (!testAxis(a[0], tA[0], EA.x, EB.x * AbsR[0][0] + EB.y * AbsR[0][1] + EB.z * AbsR[0][2])) return info;
        if (!testAxis(a[1], tA[1], EA.y, EB.x * AbsR[1][0] + EB.y * AbsR[1][1] + EB.z * AbsR[1][2])) return info;
        if (!testAxis(a[2], tA[2], EA.z, EB.x * AbsR[2][0] + EB.y * AbsR[2][1] + EB.z * AbsR[2][2])) return info;
        // B's face axes
        float tB0 = tA[0] * R[0][0] + tA[1] * R[1][0] + tA[2] * R[2][0];
        float tB1 = tA[0] * R[0][1] + tA[1] * R[1][1] + tA[2] * R[2][1];
        float tB2 = tA[0] * R[0][2] + tA[1] * R[1][2] + tA[2] * R[2][2];
        if (!testAxis(b[0], tB0, EA.x * AbsR[0][0] + EA.y * AbsR[1][0] + EA.z * AbsR[2][0], EB.x)) return info;
        if (!testAxis(b[1], tB1, EA.x * AbsR[0][1] + EA.y * AbsR[1][1] + EA.z * AbsR[2][1], EB.y)) return info;
        if (!testAxis(b[2], tB2, EA.x * AbsR[0][2] + EA.y * AbsR[1][2] + EA.z * AbsR[2][2], EB.z)) return info;
        // Cross axes Ai x Bj
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                // axis = a[i] x b[j]
                XMVECTOR ai = Load3(a[i]);
                XMVECTOR bj = Load3(b[j]);
                XMVECTOR axv = XMVector3Cross(ai, bj);
                // skip near-zero axis
                XMVECTOR nrm = XMVector3Normalize(axv);
                float len2 = XMVectorGetX(XMVector3Dot(axv, axv));
                if (len2 < 1e-8f) continue;
                XMFLOAT3 axis;
                XMStoreFloat3(&axis, nrm);
                float dist = XMVectorGetX(XMVector3Dot(tVec, nrm));
                float ra = EA.y * AbsR[(i + 2) % 3][j] + EA.z * AbsR[(i + 1) % 3][j];
                float rb = EB.y * AbsR[i][(j + 2) % 3] + EB.z * AbsR[i][(j + 1) % 3];
                if (!testAxis(axis, dist, ra, rb)) return info;
            }
        }
        info.intersects = true;
        info.penetration = (minPen == std::numeric_limits<float>::infinity() ? 0.0f : minPen);
        info.axis = minAxis;
        return info;
    }

    inline XMFLOAT3 SupportPointOnObb(const XMFLOAT3 &center, const XMFLOAT3 axes[3], const XMFLOAT3 &he,
                                      const XMFLOAT3 &dir) {
        // 返回 OBB 在方向 dir 上的最?点
        XMVECTOR D = Load3(dir);
        XMFLOAT3 o = center;
        float s0 = XMVectorGetX(XMVector3Dot(D, Load3(axes[0]))) >= 0 ? 1.f : -1.f;
        float s1 = XMVectorGetX(XMVector3Dot(D, Load3(axes[1]))) >= 0 ? 1.f : -1.f;
        float s2 = XMVectorGetX(XMVector3Dot(D, Load3(axes[2]))) >= 0 ? 1.f : -1.f;
        o.x += axes[0].x * he.x * s0 + axes[1].x * he.y * s1 + axes[2].x * he.z * s2;
        o.y += axes[0].y * he.x * s0 + axes[1].y * he.y * s1 + axes[2].y * he.z * s2;
        o.z += axes[0].z * he.x * s0 + axes[1].z * he.y * s1 + axes[2].z * he.z * s2;
        return o;
    }

    inline XMFLOAT3 ClampPointToAabbLocal(const XMFLOAT3 &p, const XMFLOAT3 &he) {
        return XMFLOAT3{ClampToExtent(p.x, he.x), ClampToExtent(p.y, he.y), ClampToExtent(p.z, he.z)};
    }

    // ?段与 AABB 最近点?（全部在 OBB 局部空?）；返回最小距?平方，同??出最近点
    inline float ClosestPtSegmentAabbLocal(const XMFLOAT3 &p0, const XMFLOAT3 &p1, const XMFLOAT3 &he,
                                           XMFLOAT3 &outSeg, XMFLOAT3 &outBox) {
        // 候?集：t=0,1 以及与?个平面的交点 t（若在[0,1]）
        XMFLOAT3 d{p1.x - p0.x, p1.y - p0.y, p1.z - p0.z};
        float bestD2 = std::numeric_limits<float>::infinity();
        auto evalT = [&](float t) {
            t = Clamp(t, 0.0f, 1.0f);
            XMFLOAT3 p{p0.x + d.x * t, p0.y + d.y * t, p0.z + d.z * t};
            XMFLOAT3 q = ClampPointToAabbLocal(p, he);
            XMVECTOR diff = XMVectorSubtract(Load3(p), Load3(q));
            float d2 = XMVectorGetX(XMVector3Dot(diff, diff));
            if (d2 < bestD2) {
                bestD2 = d2;
                outSeg = p;
                outBox = q;
            }
        };
        evalT(0.0f);
        evalT(1.0f);
        if (std::fabs(d.x) > 1e-8f) {
            evalT((he.x - p0.x) / d.x);
            evalT((-he.x - p0.x) / d.x);
        }
        if (std::fabs(d.y) > 1e-8f) {
            evalT((he.y - p0.y) / d.y);
            evalT((-he.y - p0.y) / d.y);
        }
        if (std::fabs(d.z) > 1e-8f) {
            evalT((he.z - p0.z) / d.z);
            evalT((-he.z - p0.z) / d.z);
        }
        return bestD2;
    }
}

// ---- グローバル設定 ----
const PhysicsConfig &GetPhysicsConfig() { return g_physicsConfig; }
void SetPhysicsEpsilon(float e) { g_physicsConfig.epsilon = e; }

// ---- 統一検出エントリーポイント（フェーズ2：ブール狭域） ----
static bool IntersectSphereSphere(const SphereCollider &A, const SphereCollider &B) {
    XMFLOAT3 ca = A.centerWorld();
    XMFLOAT3 cb = B.centerWorld();
    float ra = A.radiusWorld();
    float rb = B.radiusWorld();
    XMVECTOR d = XMVectorSubtract(Load3(cb), Load3(ca));
    float dist2 = XMVectorGetX(XMVector3Dot(d, d));
    float r = ra + rb;
    return dist2 <= r * r + GetPhysicsConfig().epsilon;
}

static bool IntersectSphereObb(const SphereCollider &S, const ObbCollider &B) {
    XMFLOAT3 axes[3];
    B.axesWorld(axes);
    XMFLOAT3 q = ClosestPointOnObb(S.centerWorld(), B.centerWorld(), axes, B.halfExtentsWorld());
    XMVECTOR diff = XMVectorSubtract(Load3(S.centerWorld()), Load3(q));
    float dist2 = XMVectorGetX(XMVector3Dot(diff, diff));
    float r = S.radiusWorld();
    return dist2 <= r * r + GetPhysicsConfig().epsilon;
}

static bool IntersectSphereCapsule(const SphereCollider &S, const CapsuleCollider &C) {
    auto seg = C.segmentWorld();
    float d2 = DistPointSegmentSq(S.centerWorld(), seg.first, seg.second);
    float r = S.radiusWorld() + C.radiusWorld();
    return d2 <= r * r + GetPhysicsConfig().epsilon;
}

static bool IntersectObbObbPublic(const ObbCollider &A, const ObbCollider &B) {
    return IntersectObbObb(A, B);
}

static bool IntersectObbCapsule(const ObbCollider &B, const CapsuleCollider &C) {
    // 将?段??到 OBB 的局部空?：pL = [ dot(p - Cb, axis_i) ]
    XMFLOAT3 axes[3];
    B.axesWorld(axes);
    XMFLOAT3 cB = B.centerWorld();
    auto seg = C.segmentWorld();
    auto toLocal = [&](const XMFLOAT3 &p)-> XMFLOAT3 {
        XMVECTOR v = XMVectorSubtract(Load3(p), Load3(cB));
        return XMFLOAT3{
            XMVectorGetX(XMVector3Dot(v, Load3(axes[0]))),
            XMVectorGetX(XMVector3Dot(v, Load3(axes[1]))),
            XMVectorGetX(XMVector3Dot(v, Load3(axes[2])))
        };
    };
    XMFLOAT3 p0L = toLocal(seg.first);
    XMFLOAT3 p1L = toLocal(seg.second);
    XMFLOAT3 heInflated = B.halfExtentsWorld();
    float r = C.radiusWorld();
    heInflated.x += r;
    heInflated.y += r;
    heInflated.z += r;
    return SegmentAabbIntersect(p0L, p1L, heInflated);
}

static bool IntersectCapsuleCapsule(const CapsuleCollider &A, const CapsuleCollider &B) {
    auto sa = A.segmentWorld();
    auto sb = B.segmentWorld();
    float d2 = DistSegmentSegmentSq(sa.first, sa.second, sb.first, sb.second);
    float r = A.radiusWorld() + B.radiusWorld();
    return d2 <= r * r + GetPhysicsConfig().epsilon;
}

bool Intersect(const ColliderBase &A, const ColliderBase &B) {
    ColliderType ta = A.kind();
    ColliderType tb = B.kind();
    // 上三角分?，必要?交?
    auto swapAB = [&]() { return Intersect(B, A); };
    switch (ta) {
        case ColliderType::Sphere:
            switch (tb) {
                case ColliderType::Sphere:
                    return IntersectSphereSphere(static_cast<const SphereCollider &>(A),
                                                 static_cast<const SphereCollider &>(B));
                case ColliderType::Obb:
                    return IntersectSphereObb(static_cast<const SphereCollider &>(A),
                                              static_cast<const ObbCollider &>(B));
                case ColliderType::Capsule:
                    return IntersectSphereCapsule(static_cast<const SphereCollider &>(A),
                                                  static_cast<const CapsuleCollider &>(B));
            }
            break;
        case ColliderType::Obb:
            switch (tb) {
                case ColliderType::Sphere:
                    // ?称
                    return IntersectSphereObb(static_cast<const SphereCollider &>(B),
                                              static_cast<const ObbCollider &>(A));
                case ColliderType::Obb:
                    return IntersectObbObbPublic(static_cast<const ObbCollider &>(A),
                                                 static_cast<const ObbCollider &>(B));
                case ColliderType::Capsule:
                    return IntersectObbCapsule(static_cast<const ObbCollider &>(A),
                                               static_cast<const CapsuleCollider &>(B));
            }
            break;
        case ColliderType::Capsule:
            switch (tb) {
                case ColliderType::Sphere:
                    return IntersectSphereCapsule(static_cast<const SphereCollider &>(B),
                                                  static_cast<const CapsuleCollider &>(A));
                case ColliderType::Obb:
                    return IntersectObbCapsule(static_cast<const ObbCollider &>(B),
                                               static_cast<const CapsuleCollider &>(A));
                case ColliderType::Capsule:
                    return IntersectCapsuleCapsule(static_cast<const CapsuleCollider &>(A),
                                                   static_cast<const CapsuleCollider &>(B));
            }
            break;
    }
    return false;
}

// ---- 接触情報付き交差 ----
static void ComputeSphereSphere(const SphereCollider &A, const SphereCollider &B, OverlapResult &out) {
    XMFLOAT3 ca = A.centerWorld();
    XMFLOAT3 cb = B.centerWorld();
    float ra = A.radiusWorld(), rb = B.radiusWorld();
    XMVECTOR v = XMVectorSubtract(Load3(cb), Load3(ca));
    float d = std::sqrt(std::max(0.0f, XMVectorGetX(XMVector3Dot(v, v))));
    XMFLOAT3 n{1, 0, 0};
    if (d > GetPhysicsConfig().epsilon) {
        XMStoreFloat3(&n, XMVectorScale(v, 1.0f / d));
    }
    float pen = ra + rb - d;
    out.intersects = pen >= -GetPhysicsConfig().epsilon;
    if (!out.intersects) return;
    out.penetration = std::max(0.0f, pen);
    out.normal = n;
    out.pointOnA = XMFLOAT3{ca.x + n.x * ra, ca.y + n.y * ra, ca.z + n.z * ra};
    out.pointOnB = XMFLOAT3{cb.x - n.x * rb, cb.y - n.y * rb, cb.z - n.z * rb};
}

static void ComputeSphereObb(const SphereCollider &S, const ObbCollider &B, OverlapResult &out) {
    XMFLOAT3 axes[3];
    B.axesWorld(axes);
    XMFLOAT3 cB = B.centerWorld();
    XMFLOAT3 he = B.halfExtentsWorld();
    XMFLOAT3 cs = S.centerWorld();
    float r = S.radiusWorld();
    XMFLOAT3 q = ClosestPointOnObb(cs, cB, axes, he);
    XMVECTOR diff = XMVectorSubtract(Load3(cs), Load3(q));
    float d2 = XMVectorGetX(XMVector3Dot(diff, diff));
    float d = std::sqrt(std::max(0.0f, d2));
    out.intersects = d <= r + GetPhysicsConfig().epsilon;
    if (!out.intersects) return;
    XMFLOAT3 n{0, 1, 0};
    if (d > GetPhysicsConfig().epsilon) {
        XMStoreFloat3(&n, XMVectorScale(diff, 1.0f / d));
        out.penetration = std::max(0.0f, r - d);
        out.pointOnA = XMFLOAT3{cs.x - n.x * r, cs.y - n.y * r, cs.z - n.z * r};
        out.pointOnB = q;
        out.normal = n;
    } else {
        // 球心在盒内：从 OBB 局部找最近面
        // 局部坐? u = dot(cs - cB, axis)
        XMVECTOR dV = XMVectorSubtract(Load3(cs), Load3(cB));
        float u[3] = {
            XMVectorGetX(XMVector3Dot(dV, Load3(axes[0]))),
            XMVectorGetX(XMVector3Dot(dV, Load3(axes[1]))),
            XMVectorGetX(XMVector3Dot(dV, Load3(axes[2])))
        };
        float dFace[3] = {he.x - std::fabs(u[0]), he.y - std::fabs(u[1]), he.z - std::fabs(u[2])};
        int k = 0;
        if (dFace[1] < dFace[k]) k = 1;
        if (dFace[2] < dFace[k]) k = 2;
        float s = (u[k] >= 0) ? 1.0f : -1.0f;
        XMFLOAT3 axis = axes[k];
        n = XMFLOAT3{axis.x * s, axis.y * s, axis.z * s};
        out.normal = n;
        out.penetration = r + dFace[k];
        out.pointOnB = XMFLOAT3{cs.x - n.x * dFace[k], cs.y - n.y * dFace[k], cs.z - n.z * dFace[k]};
        out.pointOnA = XMFLOAT3{cs.x - n.x * r, cs.y - n.y * r, cs.z - n.z * r};
    }
}

static void ComputeSphereCapsule(const SphereCollider &S, const CapsuleCollider &C, OverlapResult &out) {
    auto seg = C.segmentWorld();
    // 最近点 q 在?段上
    // 重用 DistPointSegmentSq 但自己求 q
    XMVECTOR P = Load3(S.centerWorld());
    XMVECTOR A = Load3(seg.first);
    XMVECTOR Bv = Load3(seg.second);
    XMVECTOR AB = XMVectorSubtract(Bv, A);
    XMVECTOR AP = XMVectorSubtract(P, A);
    float ab2 = XMVectorGetX(XMVector3Dot(AB, AB));
    float t = ab2 > 0 ? XMVectorGetX(XMVector3Dot(AP, AB)) / ab2 : 0.0f;
    t = Clamp(t, 0.0f, 1.0f);
    XMVECTOR Q = XMVectorAdd(A, XMVectorScale(AB, t));
    XMVECTOR diff = XMVectorSubtract(P, Q);
    float d = std::sqrt(std::max(0.0f, XMVectorGetX(XMVector3Dot(diff, diff))));
    float rSum = S.radiusWorld() + C.radiusWorld();
    out.intersects = d <= rSum + GetPhysicsConfig().epsilon;
    if (!out.intersects) return;
    XMFLOAT3 n{1, 0, 0};
    if (d > GetPhysicsConfig().epsilon) XMStoreFloat3(&n, XMVectorScale(diff, 1.0f / d));
    out.normal = n;
    out.penetration = std::max(0.0f, rSum - d);
    XMFLOAT3 q{};
    XMStoreFloat3(&q, Q);
    XMFLOAT3 cs = S.centerWorld();
    out.pointOnA = XMFLOAT3{cs.x - n.x * S.radiusWorld(), cs.y - n.y * S.radiusWorld(), cs.z - n.z * S.radiusWorld()};
    out.pointOnB = XMFLOAT3{q.x + n.x * C.radiusWorld(), q.y + n.y * C.radiusWorld(), q.z + n.z * C.radiusWorld()};
}

static void ComputeCapsuleCapsule(const CapsuleCollider &A, const CapsuleCollider &B, OverlapResult &out) {
    auto sa = A.segmentWorld();
    auto sb = B.segmentWorld();
    XMFLOAT3 pa, pb;
    float s = 0, t = 0; // 最近点
    float d2 = DistSegmentSegmentSq(sa.first, sa.second, sb.first, sb.second, &s, &t, &pa, &pb);
    float d = std::sqrt(std::max(0.0f, d2));
    float rSum = A.radiusWorld() + B.radiusWorld();
    out.intersects = d <= rSum + GetPhysicsConfig().epsilon;
    if (!out.intersects) return;
    XMFLOAT3 n{1, 0, 0};
    XMVECTOR diff = XMVectorSubtract(Load3(pb), Load3(pa));
    if (d > GetPhysicsConfig().epsilon) XMStoreFloat3(&n, XMVectorScale(diff, 1.0f / d));
    out.normal = n;
    out.penetration = std::max(0.0f, rSum - d);
    out.pointOnA = XMFLOAT3{pa.x - n.x * A.radiusWorld(), pa.y - n.y * A.radiusWorld(), pa.z - n.z * A.radiusWorld()};
    out.pointOnB = XMFLOAT3{pb.x + n.x * B.radiusWorld(), pb.y + n.y * B.radiusWorld(), pb.z + n.z * B.radiusWorld()};
}

static void ComputeObbObb(const ObbCollider &A, const ObbCollider &B, OverlapResult &out) {
    SatInfo si = ObbObbSatWithAxis(A, B);
    out.intersects = si.intersects;
    if (!si.intersects) return;
    // 法?方向?从 A 指向 B
    XMVECTOR dC = XMVectorSubtract(Load3(B.centerWorld()), Load3(A.centerWorld()));
    XMVECTOR nV = Load3(si.axis);
    float sign = XMVectorGetX(XMVector3Dot(dC, nV)) >= 0 ? 1.0f : -1.0f;
    XMFLOAT3 n = XMFLOAT3{si.axis.x * sign, si.axis.y * sign, si.axis.z * sign};
    out.normal = n;
    out.penetration = si.penetration;
    // 近似接触点：用支持点
    XMFLOAT3 axesA[3];
    A.axesWorld(axesA);
    XMFLOAT3 axesB[3];
    B.axesWorld(axesB);
    XMFLOAT3 pA = A.centerWorld();
    XMFLOAT3 heA = A.halfExtentsWorld();
    XMFLOAT3 pB = B.centerWorld();
    XMFLOAT3 heB = B.halfExtentsWorld();
    // support along -n for A, +n for B
    // OBB（直方体）の指定方向へのサポートポイント（最も遠い点）を計算する関数
    // See: SupportPointOnObb
    auto support = [](const XMFLOAT3 &c, const XMFLOAT3 axes[3], const XMFLOAT3 &he, const XMFLOAT3 &dir) {
        XMVECTOR D = Load3(dir);
        float s0 = XMVectorGetX(XMVector3Dot(D, Load3(axes[0]))) >= 0 ? 1.f : -1.f;
        float s1 = XMVectorGetX(XMVector3Dot(D, Load3(axes[1]))) >= 0 ? 1.f : -1.f;
        float s2 = XMVectorGetX(XMVector3Dot(D, Load3(axes[2]))) >= 0 ? 1.f : -1.f;
        XMFLOAT3 o = c;
        o.x += axes[0].x * he.x * s0 + axes[1].x * he.y * s1 + axes[2].x * he.z * s2;
        o.y += axes[0].y * he.x * s0 + axes[1].y * he.y * s1 + axes[2].y * he.z * s2;
        o.z += axes[0].z * he.x * s0 + axes[1].z * he.y * s1 + axes[2].z * he.z * s2;
        return o;
    };
    XMFLOAT3 nNeg{-n.x, -n.y, -n.z};
    out.pointOnA = support(pA, axesA, heA, nNeg);
    out.pointOnB = support(pB, axesB, heB, n);
}

static void ComputeObbCapsule(const ObbCollider &B, const CapsuleCollider &C, OverlapResult &out) {
    // 在 OBB 局部做最近点
    XMFLOAT3 axes[3];
    B.axesWorld(axes);
    XMFLOAT3 cB = B.centerWorld();
    XMFLOAT3 he = B.halfExtentsWorld();
    auto seg = C.segmentWorld();
    auto toLocal = [&](const XMFLOAT3 &p)-> XMFLOAT3 {
        XMVECTOR v = XMVectorSubtract(Load3(p), Load3(cB));
        return XMFLOAT3{
            XMVectorGetX(XMVector3Dot(v, Load3(axes[0]))),
            XMVectorGetX(XMVector3Dot(v, Load3(axes[1]))),
            XMVectorGetX(XMVector3Dot(v, Load3(axes[2])))
        };
    };
    auto toWorld = [&](const XMFLOAT3 &pL)-> XMFLOAT3 {
        // pW = cB + sum axes[i]*pL[i]
        return XMFLOAT3{
            cB.x + axes[0].x * pL.x + axes[1].x * pL.y + axes[2].x * pL.z,
            cB.y + axes[0].y * pL.x + axes[1].y * pL.y + axes[2].y * pL.z,
            cB.z + axes[0].z * pL.x + axes[1].z * pL.y + axes[2].z * pL.z
        };
    };
    XMFLOAT3 p0L = toLocal(seg.first);
    XMFLOAT3 p1L = toLocal(seg.second);
    XMFLOAT3 pL, qL;
    float d2 = ClosestPtSegmentAabbLocal(p0L, p1L, he, pL, qL);
    float d = std::sqrt(std::max(0.0f, d2));
    float r = C.radiusWorld();
    out.intersects = d <= r + GetPhysicsConfig().epsilon;
    if (!out.intersects) return;
    XMFLOAT3 pw = toWorld(pL);
    XMFLOAT3 qw = toWorld(qL);
    XMVECTOR diff = XMVectorSubtract(Load3(qw), Load3(pw));
    XMFLOAT3 n{1, 0, 0};
    if (d > GetPhysicsConfig().epsilon) XMStoreFloat3(&n, XMVectorScale(diff, 1.0f / d));
    else {
        // 使用指向最近面的法?
        XMFLOAT3 u = pL; // 近似使用段点局部位置
        float ex[3] = {he.x - std::fabs(u.x), he.y - std::fabs(u.y), he.z - std::fabs(u.z)};
        int k = 0;
        if (ex[1] < ex[k]) k = 1;
        if (ex[2] < ex[k]) k = 2;
        float sign = (k == 0 ? (u.x >= 0 ? 1.f : -1.f) : k == 1 ? (u.y >= 0 ? 1.f : -1.f) : (u.z >= 0 ? 1.f : -1.f));
        XMFLOAT3 ax = axes[k];
        n = XMFLOAT3{ax.x * sign, ax.y * sign, ax.z * sign};
    }
    out.normal = n;
    out.penetration = std::max(0.0f, r - d);
    out.pointOnA = XMFLOAT3{pw.x + n.x * r, pw.y + n.y * r, pw.z + n.z * r};
    out.pointOnB = qw;
}

bool Intersect(const ColliderBase &A, const ColliderBase &B, OverlapResult &out) {
    out = OverlapResult{}; // 清零
    ColliderType ta = A.kind();
    ColliderType tb = B.kind();
    switch (ta) {
        case ColliderType::Sphere:
            switch (tb) {
                case ColliderType::Sphere:
                    ComputeSphereSphere(static_cast<const SphereCollider &>(A), static_cast<const SphereCollider &>(B),
                                        out);
                    return out.intersects;
                case ColliderType::Obb:
                    ComputeSphereObb(static_cast<const SphereCollider &>(A), static_cast<const ObbCollider &>(B), out);
                    return out.intersects;
                case ColliderType::Capsule:
                    ComputeSphereCapsule(static_cast<const SphereCollider &>(A),
                                         static_cast<const CapsuleCollider &>(B), out);
                    return out.intersects;
            }
            break;
        case ColliderType::Obb:
            switch (tb) {
                case ColliderType::Sphere:
                    ComputeSphereObb(static_cast<const SphereCollider &>(B), static_cast<const ObbCollider &>(A), out);
                    if (out.intersects) {
                        // 方向需要从 A->B，当前?算的是 Sphere vs Obb，把法?取反
                        out.normal = XMFLOAT3{-out.normal.x, -out.normal.y, -out.normal.z};
                        std::swap(out.pointOnA, out.pointOnB);
                    }
                    return out.intersects;
                case ColliderType::Obb:
                    ComputeObbObb(static_cast<const ObbCollider &>(A), static_cast<const ObbCollider &>(B), out);
                    return out.intersects;
                case ColliderType::Capsule:
                    ComputeObbCapsule(static_cast<const ObbCollider &>(A), static_cast<const CapsuleCollider &>(B),
                                      out);
                    return out.intersects;
            }
            break;
        case ColliderType::Capsule:
            switch (tb) {
                case ColliderType::Sphere:
                    ComputeSphereCapsule(static_cast<const SphereCollider &>(B),
                                         static_cast<const CapsuleCollider &>(A), out);
                    if (out.intersects) {
                        out.normal = XMFLOAT3{-out.normal.x, -out.normal.y, -out.normal.z};
                        std::swap(out.pointOnA, out.pointOnB);
                    }
                    return out.intersects;
                case ColliderType::Obb:
                    ComputeObbCapsule(static_cast<const ObbCollider &>(B), static_cast<const CapsuleCollider &>(A),
                                      out);
                    if (out.intersects) {
                        out.normal = XMFLOAT3{-out.normal.x, -out.normal.y, -out.normal.z};
                        std::swap(out.pointOnA, out.pointOnB);
                    }
                    return out.intersects;
                case ColliderType::Capsule:
                    ComputeCapsuleCapsule(static_cast<const CapsuleCollider &>(A),
                                          static_cast<const CapsuleCollider &>(B), out);
                    return out.intersects;
            }
            break;
    }
    return false;
}
