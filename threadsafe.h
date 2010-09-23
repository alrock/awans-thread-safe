#ifndef THREADSAFE_H
#define THREADSAFE_H

#include <QMutex>
#include <QSharedPointer>

/*
 О читатель! Мы тебе сочувствуем.
 */

template <class T> class ThreadSafe;

template <class T> class ThreadLock {
public:
	ThreadLock(ThreadSafe<T>* object);
	ThreadLock(const ThreadLock<T>& lock);

	ThreadLock<T>& operator = (const ThreadLock<T>& lock);
	T* operator -> ();
	// operator T* ();

	~ThreadLock();
private:
	ThreadSafe<T>* object;
};

template <class T>
ThreadLock<T>::ThreadLock(ThreadSafe<T>* object): object(object) {
	object->mutex->lock();
}

template <class T>
ThreadLock<T>::ThreadLock(const ThreadLock<T>& lock) {
	object = lock.object;
	object->mutex->lock();
}

template <class T>
ThreadLock<T>& ThreadLock<T>::operator= (const ThreadLock<T>& lock) {
	object->mutex->unlock();
	object = lock.object;
	object->mutex->lock();

	return *this;
}

template <class T>
T* ThreadLock<T>::operator ->() {
	return object->object;
}

/*template <class T>
ThreadLock<T>::operator T*() {
	return object->object;
}*/

template <class T>
ThreadLock<T>::~ThreadLock() {
	object->mutex->unlock();
}

template <class T> class ThreadSafe {
public:
	friend class ThreadLock<T>;

    ThreadSafe();
	ThreadSafe(T* object);
	ThreadSafe(const ThreadSafe<T>& safe);

	ThreadSafe<T>& operator = (const ThreadSafe<T>& safe);

	ThreadLock<T> lock();

	~ThreadSafe();
private:
	T* object;

	QSharedPointer<QMutex> mutex;
};

template <class T>
ThreadSafe<T>::~ThreadSafe() {
	delete object;
}

template <class T>
ThreadSafe<T>::ThreadSafe(): mutex(new QMutex(QMutex::Recursive)) {
	object = new T;
}

template <class T>
ThreadSafe<T>::ThreadSafe(T* object)
	: mutex(new QMutex(QMutex::Recursive)), object(object) {}

template <class T>
ThreadSafe<T>::ThreadSafe(const ThreadSafe<T> &safe): mutex(safe.mutex) {}

template <class T>
ThreadSafe<T>& ThreadSafe<T>::operator = (const ThreadSafe<T>& safe) {
	object = safe.object;
	mutex = safe.mutex;
}

template <class T>
ThreadLock<T> ThreadSafe<T>::lock() {
	ThreadLock<T> o(this);
	return o;
}

#endif // THREADSAFE_H
