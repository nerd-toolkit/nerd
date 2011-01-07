



#ifndef ORCSTestPlugIn_H
#define ORCSTestPlugIn_H

#include <QtPlugin>
#include "PlugIns/PlugIn.h"
#include <QObject>

namespace nerd {

	/**
	 * TestPlugIn.
	 */
	class TestPlugIn : public QObject, public PlugIn {
	Q_OBJECT
	Q_INTERFACES(nerd::PlugIn);
	public:
		TestPlugIn();
		virtual ~TestPlugIn();

		virtual QString getName();
		virtual bool install();
		
	private:		
	};

}

#endif

