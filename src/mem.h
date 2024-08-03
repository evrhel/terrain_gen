#pragma once

#include <cstdlib>
#include <cassert>

class Object
{
public:
	constexpr void retain()
	{
		_refs++;
	}

	inline void release()
	{
		if (--_refs == 0)
			delete this;
	}

	constexpr Object() : _refs(1) {}

	inline virtual ~Object()
	{
		assert(_refs == 0);
	}
private:
	size_t _refs;
};

template <typename T>
class AutoRelease
{
public:
	constexpr T *operator->() const
	{
		return _object;
	}

	constexpr T &operator*() const
	{
		return *_object;
	}

	constexpr T *get() const
	{
		return _object;
	}

	constexpr operator bool() const
	{
		return _object != nullptr;
	}

	template <typename U>
	inline AutoRelease<T> &operator=(U *object)
	{
		if (_object == object)
			return *this;

		if (_object)
			_object->release();

		_object = static_cast<T *>(object);

		if (_object)
			_object->retain();

		return *this;
	}

	inline AutoRelease<T> &operator=(std::nullptr_t n)
	{
		if (_object)
			_object->release();
		_object = nullptr;
		return *this;
	}

	template <typename U>
	inline AutoRelease<T> &operator=(const AutoRelease<U> &o)
	{
		if (this == &o)
			return *this;
		return operator=(o._object);
	}

	template <typename U>
	inline AutoRelease<T> &operator=(AutoRelease<U> &&o) noexcept
	{
		if (this == &o)
			return *this;

		if (_object)
			_object->release();

		_object = o._object;

		o._object = nullptr;

		return *this;
	}

	constexpr AutoRelease() : _object(nullptr) {}
	constexpr AutoRelease(T *object) : _object(object) {}
	constexpr AutoRelease(std::nullptr_t) : _object(nullptr) {}

	template <typename U>
	constexpr AutoRelease(const AutoRelease<U> &o) :
		_object(static_cast<T *>(o._object))
	{
		if (_object)
			_object->retain();
	}
	
	constexpr AutoRelease(const AutoRelease<T> &o) :
		_object(o._object)
	{
		if (_object)
			_object->retain();
	}

	template <typename U>
	constexpr AutoRelease(AutoRelease<U> &&o) noexcept :
		_object(o._object)
	{
		o._object = nullptr;
	}

	inline ~AutoRelease()
	{
		if (_object)
			_object->release();
	}
private:
	T *_object;
};
