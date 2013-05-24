#include "VisionASeriesSensor.h"
#include "Core/Core.h"
#include "Value/StringValue.h"
#include "Math/Math.h"
#include "Math/Vector3D.h"
#include "Math/Quaternion.h"
#include <math.h>

#include <iostream>
using namespace std;

namespace nerd {
    /**
     * Construct a new VisionASeriesSensor
     */
    VisionASeriesSensor::VisionASeriesSensor(const QString &name) 
        : SimSensor(), SimObject(name), mOriginBody(0), mTargetBody(0)
    {
        mOriginBodyName = new StringValue("");
        mOriginOffset = new Vector3DValue(0,0,0);
        mTargetBodyName = new StringValue("");

        mZAxisMin = new DoubleValue(-1);
        mZAxisMax = new DoubleValue(1);

        mErrorH = new InterfaceValue(name, "ErrorH", 0, -1, 1);
        mErrorV = new InterfaceValue(name, "ErrorV", 0, -1, 1);
        mZAxis = new InterfaceValue(name, "ZAxis", 0, mZAxisMin->get(), mZAxisMax->get());
        mOutOfSight = new InterfaceValue(name, "OutOfSight", 0, 0, 1);

        mOriginBodyName->setDescription("The full name of the object this sensor used as Origin.");
        mOriginOffset->setDescription("The offset from the center of the origin.");
        mTargetBodyName->setDescription("The full name of the object this sensor should track.");
        mErrorH->setDescription("The measured horizontal error.");
        mErrorV->setDescription("The measured vertical error.");
        mZAxis->setDescription("The distance on ZAxis of the tracked object from the coordinate system of the Origin");
        mOutOfSight->setDescription("The object is behind the view hemisphere of the robot.");

        addParameter("Origin", mOriginBodyName);
        addParameter("Origin Offset", mOriginOffset);
        addParameter("Target", mTargetBodyName);
        addParameter("ErrorH", mErrorH);
        addParameter("ErrorV", mErrorV);
        addParameter("ZAxisMin", mZAxisMin);
        addParameter("ZAxisMax", mZAxisMax);
        addParameter("ZAxis", mZAxis);
        addParameter("OutOfSight", mOutOfSight);

        mOutputValues.append(mErrorH);
        mOutputValues.append(mErrorV);
        mOutputValues.append(mOutOfSight);
        mOutputValues.append(mZAxis);

        cerr << "Vision ASeries created" << endl;
    }

    /**
     * Copy constructor
     *
     * @param other the VisionASeriesSensor object to copy.
     */
    VisionASeriesSensor::VisionASeriesSensor(const VisionASeriesSensor &other)
        : Object(), ValueChangedListener(), SimSensor(), SimObject(other), mOriginBody(0), mOriginBodyName(0), mTargetBody(0), mTargetBodyName(0), mErrorH(0), mErrorV(0), mOutOfSight(0)
    {
        mOriginBodyName = dynamic_cast<StringValue*>(getParameter("Origin"));
        mOriginOffset = dynamic_cast<Vector3DValue*>(getParameter("Origin Offset"));
        mTargetBodyName = dynamic_cast<StringValue*>(getParameter("Target"));
        mErrorH = dynamic_cast<InterfaceValue*>(getParameter("ErrorH"));
        mErrorV = dynamic_cast<InterfaceValue*>(getParameter("ErrorV"));
        mOutOfSight = dynamic_cast<InterfaceValue*>(getParameter("OutOfSight"));
       
        mZAxisMin = dynamic_cast<DoubleValue*>(getParameter("ZAxisMin"));
        mZAxisMax = dynamic_cast<DoubleValue*>(getParameter("ZAxisMax"));
        mZAxis = dynamic_cast<InterfaceValue*>(getParameter("ZAxis"));

        mOutputValues.append(mErrorH);
        mOutputValues.append(mErrorV);
        mOutputValues.append(mOutOfSight);
        mOutputValues.append(mZAxis);
    }

    VisionASeriesSensor::~VisionASeriesSensor() {
    }

    SimObject* VisionASeriesSensor::createCopy() const {
        return new VisionASeriesSensor(*this);
    }

    void VisionASeriesSensor::setup() {
        SimObject::setup();

        if(mOriginBody != 0) {
            //remove stuff
            mOriginBody = 0;
        }

        mOriginBody = Physics::getPhysicsManager()->getSimBody(mOriginBodyName->get());
        
        if(mOriginBody == 0){
                Core::log("Origin object does not exist",true);
            }
        
        if(mTargetBody != 0) {
            //remove stuff
            mTargetBody = 0;
        }

        mTargetBody = Physics::getPhysicsManager()->getSimBody(mTargetBodyName->get());
        
        if(mTargetBody == 0){
                Core::log("Target object does not exist",true);
        }
        Core::log("VisionASeriesSensor: Setup completed", true);
    }

    void VisionASeriesSensor::clear() {
        SimObject::clear();

        if(mOriginBody != 0) {
            //remove stuff
            mOriginBody = 0;
        }

        if(mTargetBody != 0) {
            //remove stuff
            mTargetBody = 0;
        }
    }

    void VisionASeriesSensor::updateSensorValues() {
        if (mOriginBody == 0 || mTargetBody == 0){
            Core::log("Target or Body is null.",true);
            return;
        } 
        Vector3D originPos = mOriginBody->getPositionValue()->get();
        //Add offset
        originPos = originPos + mOriginOffset->get();
        
        Quaternion originQuatOri = mOriginBody->getQuaternionOrientationValue()->get();

        Vector3D targetPos = mTargetBody->getPositionValue()->get();
        
        Vector3D sollOri = targetPos - originPos;
        
        Quaternion sollQuat = Quaternion(0,sollOri.getX(), sollOri.getY(), sollOri.getZ());
        //Translate
        originQuatOri = originQuatOri.getInverse();
        sollQuat = (originQuatOri * sollQuat) * originQuatOri.getConjugate(); 
        
        Vector3D translatedSollOri = Vector3D(sollQuat.getX(), sollQuat.getY(), sollQuat.getZ());

        double z = translatedSollOri.getZ(); 
        if (z>0) {
            double x = translatedSollOri.getX(); 
            double y = translatedSollOri.getY(); 
            double dx = sqrt(x*x+z*z);
            double dy = sqrt(y*y+z*z);
            mErrorH->set(x/dx);
            mErrorV->set(y/dy);
            mOutOfSight->set(0);
        } 
        else {
            mErrorH->set(0);
            mErrorV->set(0);
            mOutOfSight->set(1);
        }
        mZAxis->set(z);
    }

    void VisionASeriesSensor::valueChanged(Value *value) {
      SimObject::valueChanged(value);
        if(value == 0){
            return;
        }
        else if (value == mOriginBodyName){
            mOriginBody = Physics::getPhysicsManager()->getSimBody(mOriginBodyName->get());
        Core::log("VisionASeriesSensor: Origin changed to:: "+mOriginBodyName->get() , true);
            if(mOriginBody == 0){
                Core::log("Origin object does not exist",true);
            }
        }
        else if (value == mTargetBodyName){
            mTargetBody = Physics::getPhysicsManager()->getSimBody(mTargetBodyName->get());
        Core::log("VisionASeriesSensor: Target changed", true);
            if(mTargetBody == 0){
                Core::log("Target object does not exist",true);
            }
        }
        else if (value == mZAxisMin || value == mZAxisMax) {
          mZAxis->setMin(mZAxisMin->get());
          mZAxis->setMax(mZAxisMax->get());
        }
    }
}
