#include <QtCore/QCoreApplication>
#include <QString>
#include <QDebug>
#include <QThreadPool>
#include <QRunnable>
#include <QSharedPointer>
#include "threadsafe.h"

QString data1;
QSharedPointer<QString> data(&data1);

QString *tmp = new QString;

class C {
public:
	C() {;}
	void run1(){
		for(int i=0; i<40; i++)
		{
			*data = "A";
			qDebug() << "A -" << *data;
		}
	}
	void run2(){
		for(int i=0; i<40; i++)
		{
			*data = "B";
			qDebug() << "B -" << *data;
		}
	}
};

C test_;
ThreadSafe<C> test(&test_);
ThreadSafe<C> ololo = test;

class A: public QRunnable {
public:
	void run() {
		ThreadLock<C> lock = ololo.lock();
		lock = test.lock();
		test.lock()->run1();
		ololo.lock()->run1();
		test.lock()->run1();
		ololo.lock()->run1();
		/*
		for(int i=0; i<40; i++)
		{
			*data = "A";
			qDebug() << "A -" << *data;
		}
		*/
	}
};

class B: public QRunnable {
public:
	void run() {
		test.lock()->run2();
		/*
		for(int i=0; i<40; i++)
		{
			*data = "B";
			qDebug() << "B -"<< *data;
		}
		*/
	}
};


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
	QThreadPool pool;

	test = ololo;


	pool.start(new A);
	pool.start(new B);
    return a.exec();
}
