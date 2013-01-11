/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   OrcsModelOptimizer by Chris Reinke (creinke@uni-osnabrueck.de         *
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


#ifndef OrcsModelOptimizerTools_H
#define OrcsModelOptimizerTools_H

namespace nerd { class Value; }
namespace nerd { class StringValue; }
namespace nerd { class IntValue; }
namespace nerd { class DoubleValue; }
namespace nerd { class BoolValue; }
namespace nerd { class ValueManager; }
namespace nerd { class SimObject; }

#include <QHash>
#include <QString>
#include <QStringList>


namespace nerd {

	/**
   * OrcsModelOptimizer Tools.
	 */
  class Tools {
	public:

    enum ErrorModus{ EM_ERROR, EM_WARNING, EM_NOERROR };

    static QString toPathString(const QString &path);


    /////////////////////////////////////////////////////
    // Snapshot handling methods
    /////////////////////////////////////////////////////

    static void loadSnapshot(QHash<SimObject*, QHash<Value*, QString> >  &snapshot);

    static QHash<SimObject*, QHash<Value*, QString> > saveSnapshot();


    /////////////////////////////////////////////////////
    // Time Converter Methods
    /////////////////////////////////////////////////////

    static int convertMsecToSimSteps(int msec);

    static int convertSimStepsToMsec(int simSteps);

    /////////////////////////////////////////////////////
    // getValue Methods for single values
    /////////////////////////////////////////////////////

    static Value* getValue(ValueManager *valueManager,
                           const QString &name,
                           ErrorModus modus,
                           const QString &componentName,
                           bool &isNoError,
                           const QString &additionalMessage = QString(""));

    static StringValue* getStringValue(ValueManager *valueManager,
                                       const QString &name,
                                       ErrorModus modus,
                                       const QString &componentName,
                                       bool &isNoError,
                                       const QString &additionalMessage = QString(""));

    static IntValue* getIntValue(ValueManager *valueManager,
                                       const QString &name,
                                       ErrorModus modus,
                                       const QString &componentName,
                                       bool &isNoError,
                                       const QString &additionalMessage = QString(""));

    static DoubleValue* getDoubleValue(ValueManager *valueManager,
                                 const QString &name,
                                 ErrorModus modus,
                                 const QString &componentName,
                                 bool &isNoError,
                                 const QString &additionalMessage = QString(""));

    /////////////////////////////////////////////////////
    // getOrAddValue Methods for single values
    /////////////////////////////////////////////////////

    static Value* getOrAddValue(ValueManager *valueManager,
                                const QString &name,
                                bool isPrintWarning,
                                const QString &componentName,
                                bool &isNoError,
                                Value *defaultVal,
                                const QString &additionalMessage = QString(""));

    static StringValue* getOrAddStringValue(ValueManager *valueManager,
                                            const QString &name,
                                            bool isPrintWarning,
                                            const QString &componentName,
                                            bool &isNoError,
                                            const QString &defaultVal,
                                            const QString &additionalMessage = QString(""));

    static IntValue* getOrAddIntValue(ValueManager *valueManager,
                                      const QString &name,
                                      bool isPrintWarning,
                                      const QString &componentName,
                                      bool &isNoError,
                                      int defaultVal,
                                      const QString &additionalMessage = QString(""));

    static BoolValue* getOrAddBoolValue(ValueManager *valueManager,
                                      const QString &name,
                                      bool isPrintWarning,
                                      const QString &componentName,
                                      bool &isNoError,
                                      bool defaultVal,
                                      const QString &additionalMessage = QString(""));

    static DoubleValue* getOrAddDoubleValue(ValueManager *valueManager,
                                        const QString &name,
                                        bool isPrintWarning,
                                        const QString &componentName,
                                        bool &isNoError,
                                        double defaultVal,
                                        const QString &additionalMessage = QString(""));

    /////////////////////////////////////////////////////
    // getValue Methods for different value occurences
    /////////////////////////////////////////////////////

    static Value* getValue(ValueManager *valueManager,
                           const QStringList &basePathList,
                           const QString &name,
                           ErrorModus modus,
                           const QString &componentName,
                           bool &isNoError,
                           const QString &additionalMessage = QString(""));

    static StringValue* getStringValue(ValueManager *valueManager,
                                       const QStringList &basePathList,
                                       const QString &name,
                                       ErrorModus modus,
                                       const QString &componentName,
                                       bool &isNoError,
                                       const QString &additionalMessage = QString(""));

    static IntValue* getIntValue(ValueManager *valueManager,
                                 const QStringList &basePathList,
                                 const QString &name,
                                 ErrorModus modus,
                                 const QString &componentName,
                                 bool &isNoError,
                                 const QString &additionalMessage = QString(""));

    static DoubleValue* getDoubleValue(ValueManager *valueManager,
                                       const QStringList &basePathList,
                                       const QString &name,
                                       ErrorModus modus,
                                       const QString &componentName,
                                       bool &isNoError,
                                       const QString &additionalMessage = QString(""));


  /////////////////////////////////////////////////////
  // getOrAddValue Methods for different value occurences
  /////////////////////////////////////////////////////

  static Value* getOrAddValue(ValueManager *valueManager,
                              const QStringList &basePathList,
                              const QString &name,
                              bool isPrintWarning,
                              const QString &componentName,
                              bool &isNoError,
                              Value *defaultVal,
                              const QString &additionalMessage = QString(""));

  static StringValue* getOrAddStringValue(ValueManager *valueManager,
                                          const QStringList &basePathList,
                                          const QString &name,
                                          bool isPrintWarning,
                                          const QString &componentName,
                                          bool &isNoError,
                                          const QString &defaultVal,
                                          const QString &additionalMessage = QString(""));

  static IntValue* getOrAddIntValue(ValueManager *valueManager,
                                    const QStringList &basePathList,
                                    const QString &name,
                                    bool isPrintWarning,
                                    const QString &componentName,
                                    bool &isNoError,
                                    int defaultVal,
                                    const QString &additionalMessage = QString(""));

  static BoolValue* getOrAddBoolValue(ValueManager *valueManager,
                                      const QStringList &basePathList,
                                      const QString &name,
                                      bool isPrintWarning,
                                      const QString &componentName,
                                      bool &isNoError,
                                      bool defaultVal,
                                      const QString &additionalMessage = QString(""));

  static DoubleValue* getOrAddDoubleValue(ValueManager *valueManager,
                                      const QStringList &basePathList,
                                      const QString &name,
                                      bool isPrintWarning,
                                      const QString &componentName,
                                      bool &isNoError,
                                      double defaultVal,
                                      const QString &additionalMessage = QString(""));
  };

}

#endif


