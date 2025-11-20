#pragma once
#include <cassert>
#include <cmath>

class Math
{
public:
	static constexpr float EPSILON{ 1e-4f };
	static constexpr float INF{ 1e+30f }; // std::numeric_limits<flaot>::infinity();
	static constexpr float PI{ 3.141592653f };
	static constexpr float HALF_PI{ PI * 0.5f };
	static constexpr float TWO_PI{ PI * 2.0f };
	static constexpr float DEG_TO_RAD{ PI / 180.0f };
};

// ==== Vector 2 ====
class Vector2
{
public:
	float x{ 0.0f };
	float y{ 0.0f };

	Vector2() = default;
	Vector2(float x_, float y_) : x(x_), y(y_) {}

	float operator[](size_t i) const
	{
		assert(i < 2);
		return i == 0 ? x : y;
	}

	float& operator[](size_t i)
	{
		assert(i < 2);
		return i == 0 ? x : y;
	}

	Vector2 operator+(const Vector2& rhs) const
	{
		return Vector2(x + rhs.x, y + rhs.y);
	}

	Vector2 operator-(const Vector2& rhs) const
	{
		return Vector2(x - rhs.x, y - rhs.y);
	}

	Vector2 operator*(float t) const
	{
		return Vector2{ x * t, y * t };
	}

	float Length() const
	{
		return sqrtf(x * x + y * y);
	}

	float LengthSquared() const
	{
		return x * x + y * y;
	}

	void Normalize()
	{
		float length = Length();
		if (length < Math::EPSILON)
		{
			x = 0.0f;
			y = 0.0f;
			return;
		}
		x /= length;
		y /= length;
	}

	Vector2 GetNormalized() const
	{
		float length = Length();
		if (length < Math::EPSILON)
		{
			return Vector2{ 0.0f, 0.0f };
		}
		return Vector2{ x / length, y / length };
	}

	Vector2 GetNormalized(float length) const
	{
		if (length < Math::EPSILON)
		{
			return Vector2{ 0.0f, 0.0f };
		}
		return Vector2{ x / length, y / length };
	}

	void ClampMagnitude(float max_length)
	{
		if (max_length < Math::EPSILON)
		{
			x = 0.0f;
			y = 0.0f;
			return;
		}
		x = x / max_length;
		y = y / max_length;
	}
	static Vector2 Interpolate(const Vector2& from, const Vector2& to, float t)
	{
		return from * (1.0f - t) + to * t;
	}
	static float Dot(const Vector2& lhs, const Vector2& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}
};

// ==== Vector 3 ====
class Vector3
{
public:
	float x{ 0.0f };
	float y{ 0.0f };
	float z{ 0.0f };

	// constructors
	Vector3() = default;
	Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
	Vector3(const Vector2& vec2) : x(vec2.x), y(vec2.y) {}

	float operator[](size_t i) const
	{
		assert(i < 3);
		return *(&x + i);
	}

	float& operator[](size_t i)
	{
		assert(i < 3);
		return *(&x + i);
	}

	Vector3 operator+(const Vector3& rhs) const
	{
		return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	Vector3 operator-(const Vector3& rhs) const
	{
		return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	Vector3 operator*(float t) const
	{
		return Vector3{ x * t, y * t, z * t };
	}

	float Length() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	float SquaredLength() const
	{
		return x * x + y * y + z * z;
	}

	void Normalize()
	{
		float length = Length();
		if (length < Math::EPSILON)
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
		}
		x /= length;
		y /= length;
		z /= length;
	}

	Vector3 GetNormalized() const
	{
		float length = Length();
		if (length < Math::EPSILON)
		{
			return Vector3{ 0.0f, 0.0f, 0.0f };
		}
		return Vector3{ x / length, y / length, z / length };
	}

	Vector3 CrossProduct(const Vector3& rhs) const
	{
		return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
	}

	static Vector3 Interpolate(const Vector3 from, const Vector3 to, float t)
	{
		return from * (1.0f - t) + to * t;
	}
};

// ==== Vector 4 ====
class Vector4
{
public:
	float x{ 0.0f };
	float y{ 0.0f };
	float z{ 0.0f };
	float w{ 0.0f };

	// constructors
	Vector4() = default;
	Vector4(float x_, float y_, float z_, float w_)
		: x(x_), y(y_), z(z_), w(w_) {
	}

	float operator[](size_t i) const
	{
		assert(i < 4);
		return *(&x + i);
	}

	float& operator[](size_t i)
	{
		assert(i < 4);
		return *(&x + i);
	}

	Vector4 operator+(const Vector4& rhs) const
	{
		return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
	}

	Vector4 operator-(const Vector4& rhs) const
	{
		return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
	}

	Vector4 operator*(float t) const
	{
		return Vector4{ x * t, y * t, z * t, w * t };
	}
};
