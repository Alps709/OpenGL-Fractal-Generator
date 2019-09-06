#pragma once

#include "Debug.h"
#include <vector>
#include <cassert>

struct VBElementInfo
{
	GLuint type;
	GLuint count;
	bool normalised;

	static GLuint GetTypeSize(GLuint _type)
	{
		switch (_type)
		{
			case GL_DOUBLE:       return 8;
			case GL_FLOAT:        return 4;
			case GL_UNSIGNED_INT: return 4;
		default: ;
		}
		assert(false);
		return 0;
	}
};

class VertexBufferInfo
{
public:
	VertexBufferInfo();
	~VertexBufferInfo();

	template<typename T>
	void Push(GLuint _count);

	template<>
	void Push<double>(GLuint _count);

	template<>
	void Push<float>(GLuint _count);

	template<>
	void Push<unsigned int>(GLuint _count);

	inline const std::vector<VBElementInfo>& Getindices() const;

	inline GLuint GetStride() const;


private:
	std::vector <VBElementInfo> m_indices;
	GLuint m_Stride;
};

template <typename T>
void VertexBufferInfo::Push(GLuint _count)
{
	static_assert(false);
}

template <>
inline void VertexBufferInfo::Push<double>(GLuint _count)
{
	m_indices.push_back({GL_DOUBLE, _count, false});
	m_Stride += _count * sizeof(GLdouble);
}

template <>
inline void VertexBufferInfo::Push<float>(GLuint _count)
{
	m_indices.push_back({GL_FLOAT, _count, false});
	m_Stride += _count * sizeof(GLfloat);
}

template <>
inline void VertexBufferInfo::Push<unsigned>(GLuint _count)
{
	m_indices.push_back({GL_UNSIGNED_INT, _count, false});
	m_Stride += _count * sizeof(GLuint);
}

inline const std::vector<VBElementInfo>& VertexBufferInfo::Getindices() const
{
	return m_indices;
}

inline GLuint VertexBufferInfo::GetStride() const
{
	return m_Stride;
}
