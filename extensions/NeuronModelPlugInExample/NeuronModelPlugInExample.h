



#ifndef NERDNeuronModelPlugInExample_H
#define NERDNeuronModelPlugInExample_H

#include <QtPlugin>
#include <QObject>
#include "../../nerd/system/nerd/PlugIns/PlugIn.h"

/*
 * Important: The include path here must be relative to the PlugIn.h file. 
 */

namespace nerd {

	/**
	 * NeuronModelPlugInExample.
	 */
	class NeuronModelPlugInExample : public QObject, public PlugIn {
	Q_OBJECT
	Q_INTERFACES(nerd::PlugIn);
	public:
		virtual QString getName();
		virtual bool install();
		
	private:		
	};

}

#endif

