/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   University of Osnabrueck, Germany                                     *
 *   Institute of Cognitive Science                                        *
 *   Neurocybernetics Group                                                *
 *   http://www.ikw.uni-osnabrueck.de/~neurokybernetik/                    *
 *                                                                         *
 *   Project homepage: nerd.x-bot.org                                      *
 *                                                                         *
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work was funded (2008 - 2011) by EU-Project Number ICT 214856    *
 *                                                                         *
 *                                                                         *
 *   License Agreement:                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 *   Publications based on work using the NERD kit have to state this      *
 *   clearly by citing the nerd homepage and the nerd overview paper.      *
 ***************************************************************************/

#ifndef NERDGroupConstraint_H
#define NERDGroupConstraint_H

#include <QString>
#include <QList>
#include "Core/Object.h"
#include "Network/Neuron.h"
#include "ModularNeuralNetwork/NeuronGroup.h"
#include "Network/NeuralNetworkElement.h"
#include "Core/ParameterizedObject.h"
#include "Command/CommandExecutor.h"


namespace nerd {

	class ModularNeuralNetwork;
	class NeuronGroup;

	/**
	 * GroupConstraint.
	 */
	class GroupConstraint : public ParameterizedObject {
	public:
		GroupConstraint(const QString &name, qulonglong id = 0);
		GroupConstraint(const GroupConstraint &other);
		virtual ~GroupConstraint();
		
		virtual GroupConstraint* createCopy() const = 0;

		void setId(qulonglong id);
		qulonglong getId() const;

		virtual bool attachToGroup(NeuronGroup *group);
		virtual bool detachFromGroup(NeuronGroup *group);

		virtual void networkElementIdChanged(NeuronGroup *owner, qulonglong oldId, qulonglong newId);
		
		virtual void reset();

		virtual bool isValid(NeuronGroup *owner) = 0;
		virtual bool applyConstraint(NeuronGroup *owner, CommandExecutor *executor,
									 QList<NeuralNetworkElement*> &trashcan) = 0;

		virtual bool groupIdsChanged(QHash<qulonglong, qulonglong> changedIds);

		virtual QString getErrorMessage() const;
		virtual void setErrorMessage(const QString &message);
		
		virtual QString getWarningMessage() const;
		virtual void setWarningMessage(const QString &message);

		NeuronGroup* getOwnerGroup() const;
		
		virtual bool equals(GroupConstraint *constraint) const;

	protected:	
		qulonglong mId;
		QString mErrorMessage;
		QString mWarningMessage;
		NeuronGroup *mOwnerGroup;
	};

}

#endif


