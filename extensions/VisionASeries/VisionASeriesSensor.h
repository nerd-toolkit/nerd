#ifndef NERDVisionASeriesSensor_H
#define NERDVisionASeriesSensor_H

#include <QString>
#include <QHash>
#include "Physics/SimObject.h"
#include "Physics/SimBody.h"
#include "Physics/SimSensor.h"
#include "Physics/Physics.h"
#include "Value/InterfaceValue.h"
#include "Value/Vector3DValue.h"

namespace nerd {

    /**
     * A simplie vision sensor for the ASeries
     * Given a origin , track the location of a target
     */
    class VisionASeriesSensor : public SimObject, public virtual SimSensor {
        public:
            VisionASeriesSensor(const QString &name);
            VisionASeriesSensor(const VisionASeriesSensor &other);
            virtual ~VisionASeriesSensor();

            virtual SimObject* createCopy() const;
            
            virtual void setup();
            virtual void clear();
            virtual void updateSensorValues();
            
            virtual void valueChanged(Value *value);
        
        private:
            SimBody *mOriginBody;
            StringValue *mOriginBodyName;
            Vector3DValue *mOriginOffset;
            SimBody *mTargetBody;
            StringValue *mTargetBodyName;
            InterfaceValue *mErrorH;
            InterfaceValue *mErrorV;
            InterfaceValue *mOutOfSight;
            InterfaceValue *mZAxis;
            DoubleValue *mZAxisMin;
            DoubleValue *mZAxisMax;
    };
}

#endif
