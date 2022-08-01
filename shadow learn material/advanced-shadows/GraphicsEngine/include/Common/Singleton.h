#pragma once

template<class T>
class Singleton
{
public:
	static T& Instance();

protected:
	Singleton() {}
	Singleton(const Singleton&) {}

	virtual ~Singleton() {}
};

template<class T>
T& Singleton<T>::Instance()
{
	static T instance;

	return instance;
}
