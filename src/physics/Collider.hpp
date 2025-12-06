//
// Created by zyzyz on 2025/12/05.
//

#ifndef OUTSIDERSHEW_COLLIDER_HPP
#define OUTSIDERSHEW_COLLIDER_HPP
#include <DirectXMath.h>
#include <memory>
#include <vector>


enum class ColliderType { Sphere, Obb, Capsule };

struct Aabb {
    DirectX::XMFLOAT3 min{0, 0, 0};
    DirectX::XMFLOAT3 max{0, 0, 0};
};

struct OverlapResult {
    bool intersects = false; // 初期はブール値のみ使用；後に深度/法線/接触点を拡張
    float penetration = 0.0f;
    DirectX::XMFLOAT3 normal{0, 0, 0};
    DirectX::XMFLOAT3 pointOnA{0, 0, 0};
    DirectX::XMFLOAT3 pointOnB{0, 0, 0};
};

// グローバル数値精度設定（固定だが調整可能）
struct PhysicsConfig {
    float epsilon = 1e-5f;
};

// グローバルPhysicsConfigの取得/設定（宣言のみ、実装は.cpp内）
const PhysicsConfig &GetPhysicsConfig();

void SetPhysicsEpsilon(float e);

// 基底クラスインターフェース：契約のみ定義、ストレージは提供しない
class ColliderBase {
public:
    virtual ~ColliderBase() = default;

    // 型クエリ
    virtual ColliderType kind() const = 0;

    // 変換：ローカルオフセット位置/ローカルオフセット回転（オイラー）/スケール。制約違反時はfalseを返し、内部状態を変更しない。
    // 注意：setPosition/setRotationEulerのセマンティクスは【ローカルオフセット】に変更済み、ワールドではない！
    virtual bool setPosition(const DirectX::XMFLOAT3 &posLocalOffset) = 0;

    virtual bool setRotationEuler(const DirectX::XMFLOAT3 &rotEulerLocalOffset) = 0; // pitch, yaw, roll（ラジアン）
    virtual bool setScale(const DirectX::XMFLOAT3 &scale) = 0;

    // 現在のローカルオフセットTRSを読み取り（ワールドではない）
    virtual DirectX::XMFLOAT3 position() const = 0;

    virtual DirectX::XMFLOAT3 rotationEuler() const = 0;

    virtual DirectX::XMFLOAT3 scale() const = 0;

    // ワールド行列（S*R*T）、レンダリング/デバッグ用；実装側は遅延更新または即座構築を選択可能
    virtual DirectX::XMMATRIX world() const = 0;

    // 派生データ更新（AABB/ワールドパラメータなどを更新）。内部状態が不正な場合（スケール不一致など）はfalseを返すことができる。
    virtual bool updateDerived() = 0;

    // AABB（ワールド空間、キャッシュまたは即座計算どちらでも可）
    virtual Aabb aabb() const = 0;

    // DebugDraw制御（各コライダー独立した色とオン/オフ）
    virtual void setDebugEnabled(bool enabled) = 0;

    virtual bool debugEnabled() const = 0;

    virtual void setDebugColor(const DirectX::XMFLOAT4 &rgba) = 0;

    virtual DirectX::XMFLOAT4 debugColor() const = 0;

    // バインドオフセット（バインドされたGameObjectの参照変換点に対する位置オフセット、デフォルト{0,0,0}）
    // 用途：1つのGameObjectに複数のコライダーをバインドする際、各コライダーの主体に対する位置オフセットを永続記録し、
    //      同期/シリアライズプロセス中に中心差が失われてレイアウトを復元できなくなることを防ぐ。
    // 説明：このオフセットはworld()の計算方法に強制されず、上位層（エンティティ/同期器）が必要に応じて組み合わせて使用する。
    virtual bool setOwnerOffset(const DirectX::XMFLOAT3 &offset) = 0;

    virtual DirectX::XMFLOAT3 ownerOffset() const = 0;

    // Triggerフラグ：triggerとしてマークされたコライダーはイベント発動のみに使用され、物理衝突応答は生成しない
    virtual void setIsTrigger(bool trigger) = 0;

    virtual bool isTrigger() const = 0;

    // ?? 新規追加：Ownerワールド姿勢の注入/読み取り ??
    // 上位層は毎フレームこれらのインターフェースを呼び出してOwnerワールド姿勢をColliderに書き込むべき（格納のみ、所有権なし）。
    virtual void setOwnerWorldPosition(const DirectX::XMFLOAT3 &ownerPosW) = 0;

    virtual void setOwnerWorldRotationEuler(const DirectX::XMFLOAT3 &ownerRotEulerW) = 0;

    virtual DirectX::XMFLOAT3 ownerWorldPosition() const = 0;

    virtual DirectX::XMFLOAT3 ownerWorldRotationEuler() const = 0;
};

// Sphere：ローカルパラメータはcenterLocal（オプション）+ radiusLocal；ワールド半径=radiusLocal*uniformScale
class SphereCollider : public ColliderBase {
public:
    // アクセッサー（読み取り専用）
    virtual float radiusLocal() const = 0;

    virtual float radiusWorld() const = 0;

    virtual DirectX::XMFLOAT3 centerWorld() const = 0;
};

// OBB：ローカルパラメータ center + halfExtents；ワールド派生：centerW, axesW[3], halfExtentsW
class ObbCollider : public ColliderBase {
public:
    virtual DirectX::XMFLOAT3 centerWorld() const = 0;

    virtual void axesWorld(DirectX::XMFLOAT3 outAxes[3]) const = 0; // 3つの単位軸
    virtual DirectX::XMFLOAT3 halfExtentsWorld() const = 0;
};

// Capsule：ローカルパラメータ p0, p1（長軸線分）+ radius；ワールド派生：p0W, p1W, radiusW
class CapsuleCollider : public ColliderBase {
public:
    virtual std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> segmentWorld() const = 0; // p0W, p1W
    virtual float radiusWorld() const = 0;
};

// 統一検出エントリーポイント（宣言のみ、実装は.cpp内）
bool Intersect(const ColliderBase &A, const ColliderBase &B); // 初期ブール交差
bool Intersect(const ColliderBase &A, const ColliderBase &B, OverlapResult &out);


// シンプルなファクトリー：3種類のコライダーのデフォルト実装インスタンスを作成（第1段階は最小限の実装を提供）
std::unique_ptr<SphereCollider> MakeSphereCollider(float radiusLocal);

std::unique_ptr<ObbCollider> MakeObbCollider(const DirectX::XMFLOAT3 &halfExtentsLocal);

std::unique_ptr<CapsuleCollider> MakeCapsuleCollider(const DirectX::XMFLOAT3 &p0Local,
                                                     const DirectX::XMFLOAT3 &p1Local,
                                                     float radiusLocal);
#endif //OUTSIDERSHEW_COLLIDER_HPP