
#include <Windows.h>
#include <sstream>

#ifdef NDEBUG
#define Assert( _exp ) ((void)0)
#else
#define Assert(x)
#endif

#define FastSqrt(x)	(sqrt)(x)
#define CHECK_VALID( _v ) 0
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h

#define ALIGN16 __declspec(align(16))
#define VALVE_RAND_MAX 0x7fff
#define Vector3Expand(v) (v).x, (v).y, (v).z

struct Vector2
{
public:
	Vector2() : x(0.f), y(0.f) {}
	Vector2(float _x, float _y) : x(_x), y(_y) {}

	float x;
	float y;

	Vector2 operator+(Vector2 v)
	{
		return Vector2(x + v.x, y + v.y);
	}

	Vector2 operator-(Vector2 v)
	{
		return Vector2(x - v.x, y - v.y);
	}

	Vector2 operator*(float v)
	{
		return Vector2(x * v, y * v);
	}

	Vector2 operator/(float v)
	{
		return Vector2(x / v, y / v);
	}
};
class Vector3
{
public:
	float x, y, z;
	Vector3(void);
	Vector3(float X, float Y, float Z);
	void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f);
	bool IsValid() const;
	float operator[](int i) const;
	float& operator[](int i);
	inline void Zero();
	bool operator==(const Vector3& v) const;
	bool operator!=(const Vector3& v) const;
	inline Vector3& operator+=(const Vector3& v);
	inline Vector3& operator-=(const Vector3& v);
	inline Vector3& operator*=(const Vector3& v);
	inline Vector3& operator*=(float s);
	inline Vector3& operator/=(const Vector3& v);
	inline Vector3& operator/=(float s);

	inline Vector3& operator+=(float fl);
	inline Vector3& operator-=(float fl);
	inline float	Length() const;
	inline float LengthSqr(void) const
	{
		CHECK_VALID(*this);
		return (x * x + y * y + z * z);
	}
	bool IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance&&
			y > -tolerance && y < tolerance&&
			z > -tolerance && z < tolerance);
	}
	inline Vector3 ToFRotator()
	{
		float RADPI = (float)(180 / M_PI);
		float Yaw = (float)atan2f(y, x) * RADPI;
		float Pitch = (float)atan2f(z, sqrtf(powf(x, 2.0) + powf(y, 2.0))) * RADPI;
		float Roll = 0;
		return  Vector3(Pitch, Yaw, Roll);
	}
	Vector3 to_angle() const
	{
		Vector3 angles;

		if (y == 0.0f && x == 0.0f)
		{
			angles.x = (z > 0.0f) ? 270.0f : 90.0f;
			angles.y = 0.0f;
		}
		else
		{
			angles.x = std::atan2(-z, Length()) * -180 / M_PI;
			angles.y = std::atan2(y, x) * 180 / M_PI;

			if (angles.y > 90)
				angles.y -= 180;
			else if (angles.y < 90)
				angles.y += 180;
			else if (angles.y == 90)
				angles.y = 0;
		}

		angles.z = 0.0f;

		return angles;
	}

	Vector3	Normalize();
	float	NormalizeInPlace();
	inline float	DistTo(const Vector3& vOther) const;
	inline float	DistToSqr(const Vector3& vOther) const;
	float	Dot(const Vector3& vOther) const;
	float	Length2D(void) const;
	float	Length2DSqr(void) const;
	void	MulAdd(const Vector3& a, const Vector3& b, float scalar);
	Vector3& operator=(const Vector3& vOther);
	Vector3	operator-(void) const;
	Vector3	operator+(const Vector3& v) const;
	Vector3	operator-(const Vector3& v) const;
	Vector3	operator*(const Vector3& v) const;
	Vector3	operator/(const Vector3& v) const;
	Vector3	operator*(float fl) const;
	Vector3	operator/(float fl) const;
	// Base address...
	float* Base();
	float const* Base() const;
};

//===============================================
inline void Vector3::Init(float ix, float iy, float iz)
{
	x = ix; y = iy; z = iz;
	CHECK_VALID(*this);
}
//===============================================
inline Vector3::Vector3(float X, float Y, float Z)
{
	x = X; y = Y; z = Z;
	CHECK_VALID(*this);
}
//===============================================
inline Vector3::Vector3(void) { }
//===============================================
inline void Vector3::Zero()
{
	x = y = z = 0.0f;
}
//===============================================
inline void Vector3Clear(Vector3& a)
{
	a.x = a.y = a.z = 0.0f;
}
//===============================================
inline Vector3& Vector3::operator=(const Vector3& vOther)
{
	CHECK_VALID(vOther);
	x = vOther.x; y = vOther.y; z = vOther.z;
	return *this;
}
//===============================================
inline float& Vector3::operator[](int i)
{
	Assert((i >= 0) && (i < 3));
	return ((float*)this)[i];
}
//===============================================
inline float Vector3::operator[](int i) const
{
	Assert((i >= 0) && (i < 3));
	return ((float*)this)[i];
}
//===============================================
inline bool Vector3::operator==(const Vector3& src) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x == x) && (src.y == y) && (src.z == z);
}
//===============================================
inline bool Vector3::operator!=(const Vector3& src) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x != x) || (src.y != y) || (src.z != z);
}
//===============================================
inline void Vector3Copy(const Vector3& src, Vector3& dst)
{
	CHECK_VALID(src);
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}
//===============================================
inline  Vector3& Vector3::operator+=(const Vector3& v)
{
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x += v.x; y += v.y; z += v.z;
	return *this;
}
//===============================================
inline  Vector3& Vector3::operator-=(const Vector3& v)
{
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x -= v.x; y -= v.y; z -= v.z;
	return *this;
}
//===============================================
inline  Vector3& Vector3::operator*=(float fl)
{
	x *= fl;
	y *= fl;
	z *= fl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
inline  Vector3& Vector3::operator*=(const Vector3& v)
{
	CHECK_VALID(v);
	x *= v.x;
	y *= v.y;
	z *= v.z;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
inline Vector3& Vector3::operator+=(float fl)
{
	x += fl;
	y += fl;
	z += fl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
inline Vector3& Vector3::operator-=(float fl)
{
	x -= fl;
	y -= fl;
	z -= fl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
inline  Vector3& Vector3::operator/=(float fl)
{
	Assert(fl != 0.0f);
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
inline  Vector3& Vector3::operator/=(const Vector3& v)
{
	CHECK_VALID(v);
	Assert(v.x != 0.0f && v.y != 0.0f && v.z != 0.0f);
	x /= v.x;
	y /= v.y;
	z /= v.z;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
inline float Vector3::Length(void) const
{
	CHECK_VALID(*this);

	float root = 0.0f;

	float sqsr = x * x + y * y + z * z;

	root = sqrt(sqsr);

	return root;
}
//===============================================
inline float Vector3::Length2D(void) const
{
	CHECK_VALID(*this);

	float root = 0.0f;

	float sqst = x * x + y * y;

	root = sqrt(sqst);

	return root;
}
//===============================================
inline float Vector3::Length2DSqr(void) const
{
	return (x * x + y * y);
}
//===============================================
inline Vector3 CrossProduct(const Vector3& a, const Vector3& b)
{
	return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}
//===============================================
float Vector3::DistTo(const Vector3& vOther) const
{
	Vector3 delta;

	delta.x = x - vOther.x;
	delta.y = y - vOther.y;
	delta.z = z - vOther.z;

	return delta.Length();
}
float Vector3::DistToSqr(const Vector3& vOther) const
{
	Vector3 delta;

	delta.x = x - vOther.x;
	delta.y = y - vOther.y;
	delta.z = z - vOther.z;

	return delta.LengthSqr();
}
//===============================================
inline Vector3 Vector3::Normalize()
{
	Vector3 Vector3;
	float length = this->Length();

	if (length != 0)
	{
		Vector3.x = x / length;
		Vector3.y = y / length;
		Vector3.z = z / length;
	}
	else
	{
		Vector3.x = Vector3.y = 0.0f; Vector3.z = 1.0f;
	}

	return Vector3;
}
//===============================================
// changed that to fit awall, paste from xaE
inline float Vector3::NormalizeInPlace()
{
	float radius = FastSqrt(x * x + y * y + z * z);

	// FLT_EPSILON is added to the radius to eliminate the possibility of divide by zero.
	float iradius = 1.f / (radius + FLT_EPSILON);

	x *= iradius;
	y *= iradius;
	z *= iradius;

	return radius;
}
//===============================================
inline void Vector3::MulAdd(const Vector3& a, const Vector3& b, float scalar)
{
	x = a.x + b.x * scalar;
	y = a.y + b.y * scalar;
	z = a.z + b.z * scalar;
}
//===============================================
inline float Vector3Normalize(Vector3& v)
{
	Assert(v.IsValid());
	float l = v.Length();
	if (l != 0.0f)
	{
		v /= l;
	}
	else
	{
		// FIXME:
		// Just copying the existing implemenation; shouldn't res.z == 0?
		v.x = v.y = 0.0f; v.z = 1.0f;
	}
	return l;
}
//===============================================
inline float Vector3Normalize(float* v)
{
	return Vector3Normalize(*(reinterpret_cast<Vector3*>(v)));
}
//===============================================
inline Vector3 Vector3::operator+(const Vector3& v) const
{
	Vector3 res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;
	return res;
}

//===============================================
inline Vector3 Vector3::operator-(const Vector3& v) const
{
	Vector3 res;
	res.x = x - v.x;
	res.y = y - v.y;
	res.z = z - v.z;
	return res;
}
//===============================================
inline Vector3 Vector3::operator*(float fl) const
{
	Vector3 res;
	res.x = x * fl;
	res.y = y * fl;
	res.z = z * fl;
	return res;
}
//===============================================
inline Vector3 Vector3::operator*(const Vector3& v) const
{
	Vector3 res;
	res.x = x * v.x;
	res.y = y * v.y;
	res.z = z * v.z;
	return res;
}
//===============================================
inline Vector3 Vector3::operator/(float fl) const
{
	Vector3 res;
	res.x = x / fl;
	res.y = y / fl;
	res.z = z / fl;
	return res;
}
//===============================================
inline Vector3 Vector3::operator/(const Vector3& v) const
{
	Vector3 res;
	res.x = x / v.x;
	res.y = y / v.y;
	res.z = z / v.z;
	return res;
}
inline float Vector3::Dot(const Vector3& vOther) const
{
	const Vector3& a = *this;

	return(a.x * vOther.x + a.y * vOther.y + a.z * vOther.z);
}
struct Vector4
{
public:
	Vector4() : x(0.f), y(0.f), z(0.f), w(0.f) {}

	Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

	float x;
	float y;
	float z;
	float w;
};
typedef struct _D3DMATRIX {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		float m[4][4];
	};
} D3DMATRIX;