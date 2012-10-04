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
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
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


#include "OpenGLVisualization.h"
#include "Value/BoolValue.h"
#include "Value/Vector3DValue.h"
#include "Physics/SimBody.h"
#include "Core/Core.h"
#include "Physics/PhysicsManager.h"
#include "Physics/TriangleGeom.h"
#include "Physics/RayGeom.h"
#include "Value/ColorValue.h"
#include "Physics/SimJoint.h"
#include "Collision/CollisionManager.h"
#include "Collision/CollisionObject.h"
#include "Collision/MaterialProperties.h"
#include "Physics/SphereGeom.h"
#include "Physics/BoxGeom.h"
#include "Physics/CylinderGeom.h"
#include "Physics/PlaneBody.h"
#include "Math/Math.h"
#include "Value/QuaternionValue.h"
#include "Event/TriggerEventTask.h"
#include "Physics/Physics.h"
#include "SimulationConstants.h"
#include "NerdConstants.h"
#include "Physics/CapsuleGeom.h"

#include <math.h>
#include <QDir>
#include <QApplication>
#include <iostream>
#include <QMutex>
#include <QMutexLocker>
#include "PlugIns/CommandLineArgument.h"

using namespace std;

namespace nerd{


#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_LEFT 4
#define KEY_RIGHT 8
#define KEY_FWD 16
#define KEY_BWD 32


#define SHADOW_INTENSITY (0.65f)
#define DO_OFFSET         0
#define FLOOR_SIZE        100
#define OFFSET_X_FLOOR    0.5
#define OFFSET_Y_FLOOR    0.5
#define SCALE_GROUND      1.0/1.0f

#define SKY_SIZE          800
#define HEIGHT_SKY        15
#define SCALE_SKY         5
#define PLANE_SIZE        50


class ChangeVectorValueTask : public Task {

	public:
		ChangeVectorValueTask(Vector3DValue *value, double x, double y, double z) {
			mValueToBeChanged = value;
			mX = x;
			mY = y;
			mZ = z;
		}

	bool runTask() {
		mValueToBeChanged->set(mX, mY, mZ);
		return true;
	}

	private:
		Vector3DValue *mValueToBeChanged;
		double mX;
		double mY;
		double mZ;
};

class ChangeDoubleValueTask : public Task {

	public:
		ChangeDoubleValueTask(DoubleValue *value, double x) {
			mValueToBeChanged = value;
			mX = x;
		}

	bool runTask() {
		mValueToBeChanged->set(mX);
		return true;
	}

	private:
		DoubleValue *mValueToBeChanged;
		double mX;
};

// TODO: possible optimization: instead of using: glTexImage2D to choose the current texture data, use glBindTexture and a whole vector of texture images.

OpenGLVisualization::OpenGLVisualization(bool isManipulatable, SimBody *referenceBody,
							const QString &name, bool publishValues, QWidget *w)
	: QGLWidget(w), ParameterizedObject(name), mReferenceBody(referenceBody),
        mLastMousePosition(0), mPosition(0), mDiffuseLight(0), mSpecularLight(0),
        mAmbientLight(0), mStartPosition(0), mStartOrientation(0), mCurrentPosition(0),
        mCurrentOrientation(0), mReferenceBodyPosition(0), mReferenceBodyOrientation(0),
        mViewPortUpdateTimerInterval(0), mShininess(0), mMinMoveStepSize(0), mMaxMoveStepSize(0),
        mMinSideStepSize(0), mMaxSideStepSize(0), mMouseRotationStepSize(0), mSimulationTime(0),
        mRealTime(0), mTimeDisplaySize(0), mClearColorValue(0), mTimeTextColorValue(0),
		mIsManipulatable(0), mPauseValue(0), mUseTexturesValue(0), mDisplaySimulationTime(0),
		mShowOnlySimulationTime(0),
		mChangeViewportTimer(0), mVisualizationTimer(0), mStepsPerSecondValue(0),
		mRunInPerformanceMode(0), mGlIsUpdating(false), mUseAsFrameGrabber(0), mDisableTexturesArg(0)
{
	mRealTimeRecorderRunning = 0;
	
	setWhatsThis("Ctrl+Shift+v:	Save current position as new initial viewpoint.\nShift+v:"
		" 	Restore initial viewpoint.\nCtrl+x:	Show/Hide Textures.\nCtrl+t: 	"
		"Show/Hide time information.\nCtrl+a: 	Show/Hide Coordinate Axes\nCtrl+g: 	"
		"Show/Hide Grid.\n\nCamera control: see Help (F1)");
	off = -1;
	QString title = name;
	if(mReferenceBody != 0) {
		mReferenceBodyPosition = mReferenceBody->getPositionValue();
		mReferenceBodyOrientation = mReferenceBody->getQuaternionOrientationValue();
	}
	else {
		mReferenceBodyPosition = new Vector3DValue(0.0, 0.0, 0.0);
		mReferenceBodyOrientation = new QuaternionValue();
	}
	setWindowTitle(title);


	mDisableTexturesArg = new CommandLineArgument("disableTextures", "", "",
					"Disables textures. This is useful for computers with mesa texture problems.",
					0, 0, true, true);


	mLightChanged = false;
	mInitSuccessful = false;
	mShiftHold = false;
	mRightButtonPress = false;
	mLeftButtonPress = false;
	mMiddleButtonPress = false;
	mShowAxisAndCollisionObjects = false;
	mTimerChanged = false;
	mViewChanged = false;

	mWindowPosX = 0;
	mWindowPosY = 0;
	mUpdateInterval = 0;
	mPressedMovementKeys = 0;
	mWindowRation = 0.0;

	alpha = 0.0;
	beta = 0.0;
	gamma = 0.0;
	mX = 0.0;
	mY = 0.0;
	mZ = 0.0;
	mStepSizeDistance = 0.0;


	mVisualizationName = name;
	mPublishValues = publishValues;
	
	Physics::getPhysicsManager();
	mSwitchYZAxes = Core::getInstance()->getValueManager()->getBoolValue(SimulationConstants::VALUE_SWITCH_YZ_AXES);

	if(mSwitchYZAxes == 0 || mSwitchYZAxes->get()) {
		mStartPosition = new Vector3DValue(0.0, 0.5, 1.5);
		mStartOrientation = new Vector3DValue(-10.0, 0.3, 0.0);
	}
	else {
		mStartPosition = new Vector3DValue(0.5, -5, 0.75);
		mStartOrientation = new Vector3DValue(90,0,0);
	}
	
	
	mStartPosition->setDescription("The camera position after view reset (STRG+V)");
	mStartOrientation->setDescription("The camera orientation after view reset (STRG+V)");
	mCurrentOrientation = new Vector3DValue();
	mCurrentOrientation->setDescription("The current camera orientation");
	mCurrentPosition = new Vector3DValue();
	mCurrentPosition->setDescription("The current camera position");
	mShowCoordinateSystemLines = new BoolValue(false);
	mShowCoordinateSystemLines->setDescription("Enables/Disables the coordinate system lines.");
	mShowStepsPerSecond = new BoolValue(true);
	mShowStepsPerSecond->setDescription("Displays the number of steps per second.");

	mOpeningAngleValue = new DoubleValue(getDefaultOpeningAngle());
	mOpeningAngleValue->setDescription("The opening angle of the camera.");
	mMinDistanceCutoffValue = new DoubleValue(getDefaultMinCutoff());
	mMinDistanceCutoffValue->setDescription("Only objects between min and max cuttoff range are shown.");
	mMaxDistanceCutoffValue = new DoubleValue(getDefaultMaxCutoff());
	mMaxDistanceCutoffValue->setDescription("Only objects between min and max cuttoff range are shown.");

	mReferenceBodyName = new StringValue("");
	mReferenceBodyName->setDescription("The SimObject the camera is attached to. The name is without the leading /Sim.");
	mReferenceBodyName->addValueChangedListener(this);

	mOpeningAngle = mOpeningAngleValue->get();
	mMinDistanceCutoff = mMinDistanceCutoffValue->get();
	mMaxDistanceCutoff = mMaxDistanceCutoffValue->get();

	mIsTranslationalValue = new BoolValue(false);
	mIsTranslationalValue->setDescription("In translation mode, the camera is not following the orientation.");
	mIgnoreHeightAxisInTranslationalMode = new BoolValue(true);
	mIgnoreHeightAxisInTranslationalMode->setDescription("In translational mode, the camera ignores the y axis changes of the reference body.");
	mUseAsFrameGrabber = new BoolValue(false);
	mUseAsFrameGrabber->setDescription("In frame grabber mode the camera image is stored in a buffer file.");
	mDrawOnTopOfPreviousFrame = new BoolValue(false);
	mDrawOnTopOfPreviousFrame->setDescription("Prevents that the drawing area is cleared before drawing.");

	mValuePrefix = "/OpenGLVisualization/";
	mValuePrefix.append(name).append("/");

	mDisplaySimulationTime = new BoolValue(false);
    mDisplaySimulationTime->setDescription("Displays the simulation and acutally passed time.");
	mShowOnlySimulationTime = new BoolValue(false);
	mShowOnlySimulationTime->setDescription("In the time display, show only simulation time.");
	mTimeDisplaySize = new IntValue(10);
	mTimeDisplaySize->setDescription("The font size of the simulation time display.");
	mTimeTextColorValue = new ColorValue("white");
    mTimeTextColorValue->setDescription("The font color of the simulation time display");
	mHideRecordSymbol = new BoolValue(false);
	mHideRecordSymbol->setDescription("Hides the REC text when the screen recorder is running.");

	addParameter("StartOrientation", mStartOrientation, publishValues);
	addParameter("StartPosition", mStartPosition, publishValues);
	addParameter("CurrentPosition", mCurrentPosition, publishValues);
	addParameter("CurrentOrientation", mCurrentOrientation, publishValues);
	addParameter("ShowHelperCoordinates", mShowCoordinateSystemLines, publishValues);
	addParameter("ShowStepsPerSecond", mShowStepsPerSecond, publishValues);
	addParameter("OpeningAngle", mOpeningAngleValue, publishValues);
	addParameter("MinCutoff", mMinDistanceCutoffValue, publishValues);
	addParameter("MaxCutoff", mMaxDistanceCutoffValue, publishValues);
	addParameter("Translational", mIsTranslationalValue, publishValues);
	addParameter("IgnoreHeightInTranslationalMode", mIgnoreHeightAxisInTranslationalMode, publishValues);
	addParameter("ReferenceBodyName", mReferenceBodyName, publishValues);
	addParameter("UseAsFrameGrabber", mUseAsFrameGrabber, publishValues);
	addParameter("DrawOnTopOfPreviousFrame", mDrawOnTopOfPreviousFrame, publishValues);
	addParameter("DisplaySimulationTime", mDisplaySimulationTime, publishValues);
	addParameter("ShowOnlySimulationTime", mShowOnlySimulationTime, publishValues);
	addParameter("TimeDisplaySize", mTimeDisplaySize, publishValues);
	addParameter("TimeDisplayColor", mTimeTextColorValue, mPublishValues);
	addParameter("HideRecordSymbol", mHideRecordSymbol, mPublishValues);

	mIsManipulatable = new BoolValue(isManipulatable);
	setUp();
	resize(200,200);

	makeCurrent();
	mFrameBuffer = new QGLFramebufferObject(200, 200);
}

OpenGLVisualization::~OpenGLVisualization() {

	if(mRunInPerformanceMode != 0) {
		mRunInPerformanceMode->removeValueChangedListener(this);
	}
	if(mPauseValue != 0) {
		mPauseValue->removeValueChangedListener(this);
	}

	emit stopVisualizationTimer();
	mShininess->removeValueChangedListener(this);
	mAmbientLight->removeValueChangedListener(this);
	mDiffuseLight->removeValueChangedListener(this);
	mSpecularLight->removeValueChangedListener(this);
	mPosition->removeValueChangedListener(this);

	for(QListIterator<Event*> i(mTriggerPaintEvents); i.hasNext();) {
		Event *e = i.next();
		if(e != 0) {
			e->removeEventListener(this);
		}
	}


	mUpdateInterval = mPaintUpdateTimerInterval->get();
	mPaintUpdateTimerInterval->addValueChangedListener(this);

	mVisualizationTimer->stop();
	mChangeViewportTimer->stop();

	delete mVisualizationTimer;
	delete mChangeViewportTimer;
	delete mLastMousePosition;
	delete mFrameBuffer;
}

void OpenGLVisualization::closeEvent(QCloseEvent*) {
	mWindowPosX = x();
	mWindowPosY = y();
}

void OpenGLVisualization::resetViewport() {
	mCurrentPosition->set(mStartPosition->get());
	mCurrentOrientation->set(mStartOrientation->get());

	mX = mStartPosition->getX();
	mY = mStartPosition->getY();
	mZ = mStartPosition->getZ();

	alpha = mStartOrientation->getX();
	beta = mStartOrientation->getY();
	gamma = mStartOrientation->getZ();
}

void OpenGLVisualization::setUp() {

	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);
	mVisualizationTimer = new QTimer(this);
	mChangeViewportTimer = new QTimer(this)
;
	connect(mVisualizationTimer, SIGNAL(timeout()),
			this, SLOT(updateGL()));
	connect(this, SIGNAL(stopVisualizationTimer()),
			mVisualizationTimer, SLOT(stop()));
	connect(this, SIGNAL(startVisualizationTimer()),
			mVisualizationTimer, SLOT(start()));
	connect(mChangeViewportTimer, SIGNAL(timeout()),
			this, SLOT(moveViewPoint()));

	init();
	mLightChanged = false;
	//enable focus events (required for focus_out)
	setFocusPolicy(Qt::StrongFocus);

	mGluObject = gluNewQuadric();
	gluQuadricNormals(mGluObject, GLU_SMOOTH);
	gluQuadricTexture(mGluObject, GL_TRUE);

	ValueManager *vManager = Core::getInstance()->getValueManager();

	if(vManager->getValue("/OpenGLVisualization/Coordinate Axes") == 0) {
		mDrawCoordinateAxes = new BoolValue(true);
		vManager->addValue("/OpenGLVisualization/Coordinate Axes", mDrawCoordinateAxes);
	}
	else {
		mDrawCoordinateAxes = dynamic_cast<BoolValue*>(
					vManager->getValue("/OpenGLVisualization/Coordinate Axes"));
	}

	if(vManager->getValue("/OpenGLVisualization/Shininess") == 0) {
		mShininess = new DoubleValue(100.0);
		vManager->addValue("/OpenGLVisualization/Shininess", mShininess);
	}
	else {
		mShininess = dynamic_cast<DoubleValue*>(
					vManager->getValue("/OpenGLVisualization/Shininess"));
	}

	if(vManager->getValue("/OpenGLVisualization/AmbientLight") == 0) {
		mAmbientLight = new Vector3DValue(0.75, 0.75, 0.75);
		vManager->addValue("/OpenGLVisualization/AmbientLight", mAmbientLight);
	}
	else {
		mAmbientLight = dynamic_cast<Vector3DValue*>(
					vManager->getValue("/OpenGLVisualization/AmbientLight"));
	}

	if(vManager->getValue("/OpenGLVisualization/DiffuseLight") == 0) {
		mDiffuseLight = new Vector3DValue(1.0, 1.0, 0.8);
		vManager->addValue("/OpenGLVisualization/DiffuseLight", mDiffuseLight);
	}
	else {
		mDiffuseLight = dynamic_cast<Vector3DValue*>(
					vManager->getValue("/OpenGLVisualization/DiffuseLight"));
	}

	if(vManager->getValue("/OpenGLVisualization/SpecularLight") == 0) {
		mSpecularLight = new Vector3DValue(1.0, 1.0, 0.8);
		vManager->addValue("/OpenGLVisualization/SpecularLight", mSpecularLight);
	}
	else {
		mSpecularLight = dynamic_cast<Vector3DValue*>(
					vManager->getValue("/OpenGLVisualization/SpecularLight"));
	}

	if(vManager->getValue("/OpenGLVisualization/PositionLight") == 0) {
		mPosition = new Vector3DValue(0.0, 5.0, 0.0);
		vManager->addValue("/OpenGLVisualization/PositionLight", mPosition);
	}
	else {
		mPosition = dynamic_cast<Vector3DValue*>(
					vManager->getValue("/OpenGLVisualization/PositionLight"));
	}

	if(vManager->getValue("/OpenGLVisualization/ClearColor") != 0) {
		mClearColorValue = dynamic_cast<ColorValue*>(
					vManager->getValue("/OpenGLVisualization/ClearColor"));
	}else {
		mClearColorValue = new ColorValue(125, 125, 255, 255);
		vManager->addValue("/OpenGLVisualization/ClearColor", mClearColorValue);
	}

	if(vManager->getValue("/OpenGLVisualization/Textures") != 0) {
		mUseTexturesValue = dynamic_cast<BoolValue*>(
					vManager->getValue("/OpenGLVisualization/Textures"));
	}else {
		//TODO change for release (if textures are less slow)
		mUseTexturesValue = new BoolValue(false);
		vManager->addValue("/OpenGLVisualization/Textures", mUseTexturesValue);
	}

	if(vManager->getValue("/OpenGLVisualization/PaintUpdateTimerInterval") != 0) {
		mPaintUpdateTimerInterval = dynamic_cast<IntValue*>(
					vManager->getValue("/OpenGLVisualization/PaintUpdateTimerInterval"));
	}else {
		mPaintUpdateTimerInterval = new IntValue(100);
		vManager->addValue("/OpenGLVisualization/PaintUpdateTimerInterval", mPaintUpdateTimerInterval);
	}

	mShininess->addValueChangedListener(this);
	mAmbientLight->addValueChangedListener(this);
	mDiffuseLight->addValueChangedListener(this);
	mSpecularLight->addValueChangedListener(this);
	mPosition->addValueChangedListener(this);


	mUpdateInterval = mPaintUpdateTimerInterval->get();
	mPaintUpdateTimerInterval->addValueChangedListener(this);



	setMouseTracking(false);

	mViewPortUpdateTimerInterval = new IntValue(30);
	addParameter("ViewportUpdateInterval", mViewPortUpdateTimerInterval, mPublishValues);

	mMinMoveStepSize = new DoubleValue(0.05);
	mMaxMoveStepSize = new DoubleValue(0.2);
	mMinSideStepSize = new DoubleValue(0.02);
	mMaxSideStepSize = new DoubleValue(0.1);
	mMouseRotationStepSize = new DoubleValue(0.3);
	mStepSizeDistance = 0.01;

	addParameter("MinMoveStepSize", mMinMoveStepSize, mPublishValues);
	addParameter("MaxMoveStepSize", mMaxMoveStepSize, mPublishValues);
	addParameter("MinSideStepSize", mMinSideStepSize, mPublishValues);
	addParameter("MaxSideStepSize", mMaxSideStepSize, mPublishValues);
	addParameter("MouseRotationStepSize", mMouseRotationStepSize, mPublishValues);

	//load textures if textures are not disabled
	if(mDisableTexturesArg->getNumberOfEntries() <= 0) {
		loadTextures();
	}

}

void OpenGLVisualization::loadTextures() {
	QString configDirPath = Core::getInstance()->getConfigDirectoryPath().append("/textures/");

	QImage image;

	QImage textureImage;
	QDir internalTexturesDir(QString(":/resources/textures"));
	QList<QFileInfo> entries = internalTexturesDir.entryInfoList(QDir::Files, QDir::Type);
	for(int i = 0; i < entries.size(); i++) {
		QString fileName = entries.at(i).filePath();
		QString imageName = entries.at(i).baseName();
			if(!textureImage.load(fileName)) {
				Core::getInstance()->logMessage(
				QString("OpenGLVisualization: Could not load required TextureImage: ")
				.append(fileName.toStdString().c_str()));
			}
			else {
				int index = Physics::getCollisionManager()->getMaterialProperties()->
					getTextureIndex(imageName);
				if(index < 0) {
					Physics::getCollisionManager()->getMaterialProperties()->addTexture(imageName);
				}
				mTextureImages[Physics::getCollisionManager()->getMaterialProperties()->
					getTextureIndex(imageName)] = QGLWidget::convertToGLFormat(textureImage);
			}
	}
	QImage materialImage;
	QDir textureSearchDirectory(configDirPath);
	entries = textureSearchDirectory.entryInfoList(QDir::Files, QDir::Type);
	for(int i = 0; i < entries.size(); i++) {
		QString fileName = entries.at(i).filePath();
		QString imageName = entries.at(i).baseName();
			if(!materialImage.load(fileName)) {
				Core::getInstance()->logMessage(
				QString("OpenGLVisualization: Could not load required TextureImage: ")
				.append(fileName.toStdString().c_str()));
			}
			else {
				int index = Physics::getCollisionManager()->getMaterialProperties()->
					getTextureIndex(imageName);
				if(index < 0) {
					Physics::getCollisionManager()->getMaterialProperties()->addTexture(imageName);
				}
				mTextureImages[Physics::getCollisionManager()->getMaterialProperties()->
					getTextureIndex(imageName)] = QGLWidget::convertToGLFormat(materialImage);
			}
	}

	QString fileName = configDirPath + "/textures/Sky.jpg";

	if(!image.load(fileName)) {
		fileName = QString(":/resources/textures/Sky.jpg");
		if(!image.load(fileName)){
			Core::getInstance()->logMessage(
			QString("OpenGLVisualization: Could not load required TextureImage: ")
				.append(fileName.toStdString().c_str()));
		}
	}
	if(!image.isNull()) {
		int index = Physics::getCollisionManager()->getMaterialProperties()->getTextureIndex("Sky");
		if(index < 0) {
			Physics::getCollisionManager()->getMaterialProperties()->addTexture("Sky");
		}
		mSkyTextureImage = QGLWidget::convertToGLFormat(image);
	}

	glGenTextures(1, &mTexture[0]);
	glGenTextures(1, &mSkyTexture[0]);
}

QString OpenGLVisualization::getName() const {
	return mVisualizationName;
}

void OpenGLVisualization::init() {

	bool ok = true;

	if(Core::getInstance()->isInitialized() == false) {
		ok = false;
	}

	ValueManager *vm = Core::getInstance()->getValueManager();

	mSimulationTime = vm->getDoubleValue("/Simulation/SimulationTime");
	mRealTime = vm->getDoubleValue("/Simulation/RealTime");
	mStepsPerSecondValue = vm->getIntValue(NerdConstants::VALUE_NERD_STEPS_PER_SECOND);
	mRunInPerformanceMode = vm->getBoolValue(NerdConstants::VALUE_RUN_IN_PERFORMANCE_MODE);
	mPauseValue = vm->getBoolValue(SimulationConstants::VALUE_EXECUTION_PAUSE);

	if(mRunInPerformanceMode != 0) {
		mRunInPerformanceMode->addValueChangedListener(this);
	}
	if(mPauseValue != 0) {
		mPauseValue->addValueChangedListener(this);
	}

	//Note: all parameters may be NULL

	//register for paint trigger events
	EventManager *em = Core::getInstance()->getEventManager();
	Event *event = 0;
	if(em->getEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED, false) != 0) {
		event = em->registerForEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED, this);
		if(event != 0) {
			mTriggerPaintEvents.append(event);
		}
	}
	if(em->getEvent(NerdConstants::EVENT_EXECUTION_RESET_COMPLETED, false) != 0) {
		event = em->registerForEvent(NerdConstants::EVENT_EXECUTION_RESET_COMPLETED, this);
		if(event != 0) {
			mTriggerPaintEvents.append(event);
		}
	}
	
	mRealTimeRecorderRunning = vm->getBoolValue(SimulationConstants::VALUE_RUN_REAL_TIME_RECORDER);

	if(!ok)
	{
		mInitSuccessful = false;
	}
	else {
		mInitSuccessful = true;
	}
}


void OpenGLVisualization::updateVisualization() {

	
	glClearColor (mClearColorValue->get().red() / 255.0,
					mClearColorValue->get().green() / 255.0,
					mClearColorValue->get().blue() / 255.0,
					mClearColorValue->get().alpha() / 255.0);

	glMatrixMode(GL_MODELVIEW);

	glRotated(-alpha, 1, 0, 0);
	glRotated(-beta, 0, 1, 0);
	glRotated(-gamma, 0, 0, 1);

	glTranslated(-mX, -mY, -mZ);


	if(!mIsTranslationalValue->get()) {
		Quaternion quaternion = mReferenceBodyOrientation->get();
		double angle = 2 * acos(quaternion.getW());
		double scale = sqrt(quaternion.getX() * quaternion.getX()
								+ quaternion.getY() * quaternion.getY()
								+ quaternion.getZ() * quaternion.getZ());
		double x  = 0.0;
		double y = 0.0;
		double z = 0.0;

		if(scale != 0) {
			x = quaternion.getX() / scale;
			y = quaternion.getY() / scale;
			z = quaternion.getZ() / scale;
		}

		angle = angle*180.0/Math::PI;
		glRotated(-angle, x, y, z);
	}

	if(mIgnoreHeightAxisInTranslationalMode->get()) {
		if(mSwitchYZAxes == 0 || mSwitchYZAxes->get()) {
			glTranslated(-mReferenceBodyPosition->getX(),
						0.0,
						-mReferenceBodyPosition->getZ());
		}
		else {
			glTranslated(-mReferenceBodyPosition->getX(),
						 -mReferenceBodyPosition->getY(),
						 0.0);
		}
	}
	else {
		glTranslated(-mReferenceBodyPosition->getX(),
					-mReferenceBodyPosition->getY(),
					-mReferenceBodyPosition->getZ());
	}


	//synchronize with PhysicsManager reset.
	QMutex *resetMutex = Physics::getPhysicsManager()->getResetMutex();

	if(!resetMutex->tryLock(1000)) {
		//stop painting the visualization.
		return;
	}
	QMutexLocker resetMutexLocker(resetMutex);
	resetMutex->unlock(); //TODO: why is this unlocked here? Check.


	mSimBodies = Physics::getPhysicsManager()->getSimBodies().toVector();
	mSimJoints = Physics::getPhysicsManager()->getSimJoints().toVector();

	drawAxis();

	if(mUseTexturesValue->get() && !mSkyTextureImage.isNull() && !mDrawOnTopOfPreviousFrame->get()) {
		drawSky();
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for(int paintOrder = 0; paintOrder < 2; ++paintOrder) {

		glPushMatrix();
		//prevent translucent objects to write before opaque objects.
		if(paintOrder == 0) {
			glAlphaFunc(GL_GREATER, 0.8);
			glEnable(GL_ALPHA_TEST);
		}
		else {
			glAlphaFunc(GL_GREATER, 0.05);
			glAlphaFunc(GL_LEQUAL, 0.8);
			glEnable(GL_ALPHA_TEST);
			glDepthMask(0);
		}

		for(int i = 0; i < mSimBodies.size(); i++)  {
			SimBody *currentBody = mSimBodies.at(i);

			glPushMatrix();

			// Use Quaternion instead of rotation matrix!
			Quaternion quaternion = currentBody->getQuaternionOrientationValue()->get();
			double angle = 2 * acos(quaternion.getW());
			double scale = sqrt(quaternion.getX() * quaternion.getX()
												+ quaternion.getY() * quaternion.getY()
												+ quaternion.getZ() * quaternion.getZ());
			double x  = 0.0;
			double y = 0.0;
			double z = 0.0;

			if(scale != 0) {
				x = quaternion.getX() / scale;
				y = quaternion.getY() / scale;
				z = quaternion.getZ() / scale;
			}

			angle = angle * 180.0 / Math::PI;
			glTranslated(currentBody->getPositionValue()->getX(),
						currentBody->getPositionValue()->getY(),
						currentBody->getPositionValue()->getZ());
			glRotated(angle, x, y, z);

			for(int j = 0; j < currentBody->getCollisionObjects().size(); j++) {

				glDisable(GL_TEXTURE_2D);
				CollisionObject *collisionObject = currentBody->getCollisionObjects().at(j);
				SimGeom *currentGeom = collisionObject->getGeometry();

				if(currentGeom->getColor().alpha() < 5) {
					//do not draw objects that are almost transparent.
					continue;
				}

				bool hasTex = false;
				if(mUseTexturesValue->get()) {
					QList<int> textureTypes =
							currentBody->getCollisionObjects().at(j)->getTextureTypeInformation();

					for(int i = 0; i < textureTypes.size(); i++) {
						if(mTextureImages.contains(textureTypes.at(i))){
							hasTex = true;
							glEnable(GL_TEXTURE_2D);
							break;
						}
					}
				}

				//disable lighting for material light.
				if(collisionObject->getMaterialType() == MaterialProperties::MATERIAL_NUMBER_LIGHT) {
					glDisable(GL_LIGHTING);
				}
				else {
					glEnable(GL_LIGHTING);
				}

				if(dynamic_cast<PlaneBody*>(mSimBodies.at(i)) != 0) {
					if(hasTex) {
						int type = currentBody->getCollisionObjects().at(j)->
							getTextureTypeInformation().at(0);
						glBindTexture( GL_TEXTURE_2D, mTexture[0] );
						glTexImage2D( GL_TEXTURE_2D, 0, 3, mTextureImages.value(type).width(),
							mTextureImages.value(type).height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
							mTextureImages.value(type).bits() );
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
						glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
					}
					PlaneBody *plane = dynamic_cast<PlaneBody*>(mSimBodies.at(i));
					drawPlane(plane);
				}
				else if(currentGeom != 0 && currentGeom->isDrawingEnabled()) {

					glColor4f(currentGeom->getColor().red() / 255.0, currentGeom->getColor().green() / 255.0,
						currentGeom->getColor().blue() / 255.0, currentGeom->getColor().alpha() / 255.0);

					if(dynamic_cast<BoxGeom*>(currentGeom) != 0) {
						drawBox(currentBody->getCollisionObjects().at(j), hasTex);
					}
					else if(dynamic_cast<TriangleGeom*>(currentGeom) != 0) {
							drawTriangleGeom(currentBody->getCollisionObjects().at(j),
								dynamic_cast<TriangleGeom*>(currentGeom), hasTex);
					}
					else if(dynamic_cast<SphereGeom*>(currentGeom) != 0) {
						drawSphere(currentBody->getCollisionObjects().at(j), hasTex);
					}
					else if (dynamic_cast<CylinderGeom*>(currentGeom) != 0) {
						drawCylinder(currentBody->getCollisionObjects().at(j), hasTex);
					}
					else if(dynamic_cast<RayGeom*>(currentGeom) != 0) {
						drawRay(currentBody->getCollisionObjects().at(j));
					}
					else if(dynamic_cast<CapsuleGeom*>(currentGeom) != 0) {
						drawCapsule(currentBody->getCollisionObjects().at(j), hasTex);
					}
				}
			}
			glPopMatrix();
			glDisable(GL_TEXTURE_2D);
		}
		glDisable(GL_ALPHA_TEST);
		glDepthMask(1);
		glPopMatrix();
	}
	glDisable(GL_BLEND);
}

void OpenGLVisualization::drawPlane(PlaneBody *plane) {

// 	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mShininess->get());

	Vector3DValue *axis = dynamic_cast<Vector3DValue*>(plane->getParameter("Axis"));
	DoubleValue *distance = dynamic_cast<DoubleValue*>(plane->getParameter("Distance"));
	ColorValue *color = dynamic_cast<ColorValue*>(plane->getParameter("Color"));
	StringValue *material = dynamic_cast<StringValue*>(plane->getParameter("Material"));

	if(axis != 0 && distance != 0 && color != 0 && material != 0) {
		const float gsize = PLANE_SIZE;
		
		//TODO What was this line for? @verena
		//Vector3D newVector = distance->get() * axis->get();
		
		
		glColor4f(color->get().red() / 255.0, color->get().green() / 255.0,
			color->get().blue() / 255.0, color->get().alpha() / 255.0);

		Vector3D point1(-gsize,distance->get(),-gsize);
		Vector3D point2(gsize,distance->get(),-gsize);
		Vector3D point3(gsize,distance->get(),gsize);
		Vector3D point4(-gsize,distance->get(),gsize);

		Vector3D defaultNorm(0,1,0);
		// calculate rotation of the plane
		double alphaP = acos(axis->get()*defaultNorm);

		alphaP = alphaP * 180.0 / Math::PI;

		Vector3D rotationAxis;
		rotationAxis = Vector3D::crossProduct(axis->get(), defaultNorm);

		float offsetX = 0.5;
		float offsetY = 0.5;
		glPushMatrix();

		glRotated(alphaP, rotationAxis.getX(), rotationAxis.getY(), rotationAxis.getZ());

		double textureSize = gsize * 2.0;

		glBegin (GL_QUADS);
		glNormal3f (0,0,1);
		glTexCoord2f (-textureSize + offsetX, -textureSize + offsetY);
		glVertex3f (point1.getX(), point1.getY(), point1.getZ());

		glTexCoord2f (textureSize + offsetX, -textureSize + offsetY);
		glVertex3f (point2.getX(), point2.getY(), point2.getZ());

		glTexCoord2f (textureSize + offsetX, textureSize + offsetY);
		glVertex3f (point3.getX(), point3.getY(), point3.getZ());

		glTexCoord2f (-textureSize + offsetX, textureSize + offsetY);
		glVertex3f (point4.getX(), point4.getY(), point4.getZ());
		glEnd();

		glPopMatrix();
	}
}

void OpenGLVisualization::drawAxis() {
	glShadeModel(GL_FLAT);
	
	double axisLength = 1;
	
	if(mDrawCoordinateAxes->get()) {
		glLineWidth(2);
		glColor3f(255,0,0);
		glBegin(GL_LINES);
		glVertex3d(0,0,0);
		glVertex3d(axisLength,0,0);
		glEnd();
		glColor3f(0,255,0);
		glBegin(GL_LINES);
		glVertex3d(0,0,0);
		glVertex3d(0,axisLength,0);
		glEnd();
		glColor3f(0,0,255);
		glBegin(GL_LINES);
		glVertex3d(0,0,0);
		glVertex3d(0,0,axisLength);

		glEnd();
	}
	
	
	glColor4f(0, 0, 0, 100);
	axisLength = 2 * PLANE_SIZE;
				glLineWidth(2.0);
	double off = 0.0005;
	
	if(mShowCoordinateSystemLines->get()){
		for(int k = 0; k < 1; k++) {
			for(int i = -PLANE_SIZE; i < PLANE_SIZE; i++) {
				int mod = i % 5;
				if(mod == 0) {
					glColor4f(0, 0, 0, 100);
				}
				else {
					glColor4f(255,  255,255, 100);
				}
				glPushMatrix();
				if(mSwitchYZAxes == 0 || mSwitchYZAxes->get()) {
					glTranslatef(i, k+off, 0);
				}
				else {
					glTranslatef(i, 0, k+off);
				}
				glBegin(GL_LINES);
					if(mSwitchYZAxes == 0 || mSwitchYZAxes->get()) {
						glVertex3d(0, 0, -axisLength / 2.0);
						glVertex3d(0, 0, axisLength / 2.0);
					}
					else {
						glVertex3d(0, -axisLength / 2.0, 0.0);
						glVertex3d(0, axisLength / 2.0, 0.0);
					}
				glEnd();

				glPopMatrix();
				glPushMatrix();
				if(mSwitchYZAxes == 0 || mSwitchYZAxes->get()) {
					glTranslatef(0, k + off, i);
				}
				else {
					glTranslatef(0, i, k + off);
				}
				glBegin(GL_LINES);
				glVertex3d(-axisLength / 2.0, 0, 0);
				glVertex3d(axisLength / 2.0, 0, 0);
				glEnd();
				glPopMatrix();
			}
		}
	}
}

void OpenGLVisualization::initializeGL() {
	glClearColor (mClearColorValue->get().red() / 255.0,
					mClearColorValue->get().green() / 255.0,
					mClearColorValue->get().blue() / 255.0,
					mClearColorValue->get().alpha() / 255.0);
	glEnable(GL_DEPTH_TEST);
	createEnvironmentConditions();
	setAutoBufferSwap(true);
	mLastMousePosition = new Vector3D(0, 0, 0);
	mVisualizationTimer->start(mUpdateInterval);
}


void OpenGLVisualization::paintGL() {

	makeCurrent();
	bool frameGrabberMode = mUseAsFrameGrabber->get();
	if(frameGrabberMode) {
		mFrameBuffer->bind();
	}

	if(!mInitSuccessful) {
		init();
	}
	if(!mInitSuccessful || (!isVisible() && !frameGrabberMode)
		|| (mRunInPerformanceMode != 0 && (mRunInPerformanceMode->get() && !frameGrabberMode)))
	{
		//clear screen and return.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);

		glPushMatrix();
		glFlush();

		return;
	}
	if(!mDrawOnTopOfPreviousFrame->get()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else {
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();

	if(Core::getInstance()->isInitialized()) {
		updateVisualization();
	}
	glPopMatrix();

	Color timeTextColor = mTimeTextColorValue->get();
	qglColor(QColor(timeTextColor.red(),
					timeTextColor.green(),
					timeTextColor.blue(),
					timeTextColor.alpha()));

	if(Core::getInstance()->isInitialized()) {
		int fontSize = mTimeDisplaySize->get();
		int posX = 5 + ((int) (7 * ((double) fontSize)));
		int posY = (int) (1.5 * ((double) fontSize));

		if(mDisplaySimulationTime->get() && mInitSuccessful) {
			// Draw Simulation time information, if according value does exist.
			QFont font(QApplication::font());
			font.setPointSize(fontSize);

			if(mSimulationTime != 0) {
				if(mShowOnlySimulationTime->get()) {
					renderText(5, posY, "Time", font);
					renderText((posX - 5) / 2.0 + 5, posY,
						QString(": ").append(QString::number(mSimulationTime->get())), font);
				}
				else {
					renderText(5, posY, "Sim. Time", font);
					renderText(posX, posY,
						QString(": ").append(QString::number(mSimulationTime->get())), font);
				}
			}
			if(!(mShowOnlySimulationTime->get()) && mRealTime != 0) {
				renderText(5, 2 * posY, "Real Time", font);
				renderText(posX, 2 * posY, QString(": ")
						.append(QString::number(mRealTime->get())), font);
			}
			if(mShowStepsPerSecond->get() && mStepsPerSecondValue != 0) {
				renderText(5, 3 * posY, "SPS", font);
				renderText(posX, 3 * posY, QString(": ")
						.append(QString::number(mStepsPerSecondValue->get())), font);
			}
			
			if(!mHideRecordSymbol->get()) {
				//show REC sign if screen recorder is running.
				fontSize = Math::max(fontSize, 20);
				font.setPointSize(fontSize);
				posY = (int) (1.5 * ((double) fontSize));
				if(mRealTimeRecorderRunning != 0 && mRealTimeRecorderRunning->get()) {
					renderText(width() - (20.0 + ((int) (3 * ((double) fontSize)))), posY, "Rec", font);
				}
			}
		}
	}

	glFlush();
	if(frameGrabberMode) {
		mFrameBuffer->release();
		mFrameBuffer->toImage().save(QString("framegrabber_")
										.append(getName().replace("/", "_"))
										.append(".png"), "PNG");
	}
}

void OpenGLVisualization::updateGL() {

	//cerr << "UpdateGl: " << getName().toStdString().c_str() << endl;

	if(mGlIsUpdating) {
		return;
	}
	if(!mInitSuccessful) {
		init();
	}
	if(!mInitSuccessful) {
		return;
	}

	bool runVisualizationTimer = true;
	if(((mRunInPerformanceMode != 0 && mRunInPerformanceMode->get())
				|| (mPauseValue != 0 && mPauseValue->get()))
			&& !(mRightButtonPress || mLeftButtonPress || mMiddleButtonPress))
	{
		runVisualizationTimer = false;
	}

	if(isVisible() || mUseAsFrameGrabber->get()) {

		mGlIsUpdating = true;

		if(mTimerChanged) {
			mVisualizationTimer->setInterval(mUpdateInterval);
			mTimerChanged = false;
		}
		makeCurrent();
		if(mLightChanged) {
			changeLighting();
			mLightChanged = false;
		}
		if(mViewChanged){
			changeView();
		}

		//bool runVisualizationTimer = true;
		if(!runVisualizationTimer) {
			//stop visualization timer if system runs in pause or performance mode AND
			//the visualization is not manipulated via mouse.
			emit stopVisualizationTimer();
		}
		glDraw();

		mGlIsUpdating = false;

		//TODO remove, just to try out.
		//QImage img = grabFrameBuffer(true);
		//img.save(QString("screenImage_").append(getName()).append(".png"), "PNG");
	}

	if(runVisualizationTimer) {
		//restart visualization timer (so the interval starts after painting.
		emit startVisualizationTimer();
	}
}

void OpenGLVisualization::resizeGL(int width, int height){
	delete mFrameBuffer;
	mFrameBuffer = new QGLFramebufferObject(width, height);

	glViewport (0, 0, width, height);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	mWindowRation = (GLfloat) width/(GLfloat) height;
	if(width>height ) {
		glScalef(mWindowRation, mWindowRation, 1);
	}

	gluPerspective((GLfloat) mOpeningAngleValue->get(), mWindowRation, mMinDistanceCutoffValue->get(), mMaxDistanceCutoffValue->get());

	glMatrixMode (GL_MODELVIEW);
}


/**
 * Called when the widget is loosing keyboard and mouse focus.
 * All movement actions are canceled as this happens.
 */
void OpenGLVisualization::focusOutEvent(QFocusEvent*) {
	mChangeViewportTimer->stop();
	mPressedMovementKeys = 0;
}


void OpenGLVisualization::createEnvironmentConditions() {
	GLfloat pos[4] = {mPosition->getX(), mPosition->getY(), mPosition->getZ(), 1.0};
	GLfloat white[4] = {mDiffuseLight->getX(), mDiffuseLight->getY(),
		mDiffuseLight->getZ(), 1.0};
	GLfloat black[4] = {mSpecularLight->getX(), mSpecularLight->getY(),
		mSpecularLight->getZ(), 0.0};

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_POSITION, pos);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT1, GL_SPECULAR, black);

	GLfloat LModelAmbient[4] ={mAmbientLight->getX(), mAmbientLight->getY(),
		mAmbientLight->getZ(),1.0F};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,&LModelAmbient[0]);

	glEnable (GL_COLOR_MATERIAL);
	glColorMaterial (GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv (GL_FRONT, GL_SPECULAR, black);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLVisualization::changeLighting() {
	GLfloat pos[4] = {mPosition->getX(), mPosition->getY(), mPosition->getZ(), 1.0};
 	GLfloat white[4] = {mDiffuseLight->getX(), mDiffuseLight->getY(),
		mDiffuseLight->getZ(), 1.0};
	GLfloat black[4] = {mSpecularLight->getX(), mSpecularLight->getY(),
		mSpecularLight->getZ(), 0.0};

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_POSITION, pos);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT1, GL_SPECULAR, black);

	GLfloat LModelAmbient[4] ={mAmbientLight->getX(), mAmbientLight->getY(),
		mAmbientLight->getZ(),1.0F};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,&LModelAmbient[0]);

	glEnable (GL_COLOR_MATERIAL);
	glColorMaterial (GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv (GL_FRONT, GL_SPECULAR, black);

	glMatrixMode(GL_MODELVIEW);
}

void OpenGLVisualization::drawBox(CollisionObject *currentCollisionObject, bool hasATexture) {
	BoxGeom *boxGeom = dynamic_cast<BoxGeom*>(currentCollisionObject->getGeometry());

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mShininess->get());


	QVector<Vector3D> points = boxGeom->getPoints();
	Vector3D normal;
	Vector3D first = points.at(0);
	Vector3D second = points.at(1);
	Vector3D third = points.at(2);
	Vector3D fourth = points.at(3);
	Vector3D fifth = points.at(4);
	Vector3D sixth = points.at(5);
	Vector3D seventh = points.at(6);
	Vector3D eighth = points.at(7);
	Vector3D firstCross;
	Vector3D secondCross;
	
	//TODO Could there be an error: fourth is never used! @verena?

	if(hasATexture) {
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		int textureType = -1;
		QList<int> objectFaceInfos = currentCollisionObject->getTextureTypeInformation();

		int defaultT  = Physics::getCollisionManager()->getMaterialProperties()->
			getTextureIndex("Default");

		double scaling = 1;

		firstCross = third - first;
		secondCross = second - first;

		normal = Vector3D::crossProduct(firstCross, secondCross);
		normal.normalize();


		textureType = objectFaceInfos.at(0);

		if(textureType == defaultT || !mTextureImages.contains(textureType)) {
			glDisable(GL_TEXTURE_2D);
		}
		glBindTexture( GL_TEXTURE_2D, mTexture[0] );
		glTexImage2D( GL_TEXTURE_2D, 0, 3, mTextureImages.value(textureType).width(),
			mTextureImages.value(textureType).height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
			mTextureImages.value(textureType).bits());

  		glBegin(GL_POLYGON);
			glNormal3d(normal.getX(), normal.getY(), normal.getZ());
			glTexCoord2f(0.0, 0.0);
			glVertex3d(points.at(0).getX(), points.at(0).getY(), points.at(0).getZ());
			glTexCoord2f(0.0, scaling);
			glVertex3d(points.at(1).getX(), points.at(1).getY(), points.at(1).getZ());
			glTexCoord2f(scaling, scaling);
			glVertex3d(points.at(2).getX(), points.at(2).getY(), points.at(2).getZ());
			glTexCoord2f(scaling, 0.0);
			glVertex3d(points.at(3).getX(), points.at(3).getY(), points.at(3).getZ());
		glEnd();

		firstCross = seventh - third;
		secondCross = eighth - third;

		normal = Vector3D::crossProduct(firstCross, secondCross);
		normal.normalize();

		if(textureType != objectFaceInfos.at(1)) {
			textureType = objectFaceInfos.at(1);

			if(textureType == defaultT || !mTextureImages.contains(textureType)) {
					glDisable(GL_TEXTURE_2D);
			}
			else {
				glEnable(GL_TEXTURE_2D);
				glBindTexture( GL_TEXTURE_2D, mTexture[0] );
				glTexImage2D( GL_TEXTURE_2D, 0, 3,
					mTextureImages.value(textureType).width(),
					mTextureImages.value(textureType).height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
					mTextureImages.value(textureType).bits() );
			}
		}
		glBegin(GL_POLYGON);
			glNormal3d(normal.getX(), normal.getY(), normal.getZ());
			glTexCoord2f(0.0, scaling);
			glVertex3d(points.at(2).getX(), points.at(2).getY(), points.at(2).getZ());
			glTexCoord2f(scaling, scaling);
			glVertex3d(points.at(7).getX(), points.at(7).getY(), points.at(7).getZ());
			glTexCoord2f(scaling, 0.0);
			glVertex3d(points.at(6).getX(), points.at(6).getY(), points.at(6).getZ());
			glTexCoord2f(0.0, 0.0);
			glVertex3d(points.at(3).getX(), points.at(3).getY(), points.at(3).getZ());
		glEnd();

		firstCross = sixth - first;
		secondCross = seventh - first;

		normal = Vector3D::crossProduct(firstCross, secondCross);
		normal.normalize();

		if(textureType != objectFaceInfos.at(2)) {
			textureType = objectFaceInfos.at(2);

			if(textureType == defaultT || !mTextureImages.contains(textureType)) {
					glDisable(GL_TEXTURE_2D);
			}
			else {
				glEnable(GL_TEXTURE_2D);

				glBindTexture( GL_TEXTURE_2D, mTexture[0] );
				glTexImage2D( GL_TEXTURE_2D, 0, 3,
					mTextureImages.value(textureType).width(),
					mTextureImages.value(textureType).height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
					mTextureImages.value(textureType).bits() );
			}
		}

		glBegin(GL_POLYGON);
			glNormal3d(normal.getX(), normal.getY(), normal.getZ());
			glTexCoord2f(0.0, scaling);
			glVertex3d(points.at(0).getX(), points.at(0).getY(), points.at(0).getZ());
			glTexCoord2f(0.0, 0.0);
			glVertex3d(points.at(5).getX(), points.at(5).getY(), points.at(5).getZ());
			glTexCoord2f(scaling, 0.0);
			glVertex3d(points.at(6).getX(), points.at(6).getY(), points.at(6).getZ());
			glTexCoord2f(scaling, scaling);
			glVertex3d(points.at(3).getX(), points.at(3).getY(), points.at(3).getZ());
		glEnd();

		firstCross = fifth - first;
		secondCross = sixth - first;

		normal = Vector3D::crossProduct(firstCross, secondCross);
		normal.normalize();

		if(textureType != objectFaceInfos.at(3)) {

			textureType = objectFaceInfos.at(3);

			if(textureType == defaultT || !mTextureImages.contains(textureType)) {
					glDisable(GL_TEXTURE_2D);
			}
			else {
				glEnable(GL_TEXTURE_2D);
				glBindTexture( GL_TEXTURE_2D, mTexture[0] );
				glTexImage2D( GL_TEXTURE_2D, 0, 3,
					mTextureImages.value(textureType).width(),
					mTextureImages.value(textureType).height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
					mTextureImages.value(textureType).bits() );
			}
		}
 		glBegin(GL_POLYGON);
			glNormal3d(normal.getX(), normal.getY(), normal.getZ());
			glTexCoord2f(scaling, 0.0);
			glVertex3d(points.at(0).getX(), points.at(0).getY(), points.at(0).getZ());
			glTexCoord2f(0.0, 0.0);
			glVertex3d(points.at(5).getX(), points.at(5).getY(), points.at(5).getZ());
			glTexCoord2f(0.0, scaling);
			glVertex3d(points.at(4).getX(), points.at(4).getY(), points.at(4).getZ());
			glTexCoord2f(scaling, scaling);
			glVertex3d(points.at(1).getX(), points.at(1).getY(), points.at(1).getZ());
		glEnd();

		firstCross = seventh - fifth;
		secondCross = sixth - fifth;

		normal = Vector3D::crossProduct(firstCross, secondCross);
		normal.normalize();

		if(textureType != objectFaceInfos.at(4)) {
			textureType = objectFaceInfos.at(4);

			if(textureType == defaultT || !mTextureImages.contains(textureType)) {
					glDisable(GL_TEXTURE_2D);
			}
			else {
				glEnable(GL_TEXTURE_2D);

				glBindTexture( GL_TEXTURE_2D, mTexture[0] );
				glTexImage2D( GL_TEXTURE_2D, 0, 3, mTextureImages.value(textureType).width(),
					mTextureImages.value(textureType).height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
					mTextureImages.value(textureType).bits() );
			}
		}

		glBegin(GL_POLYGON);
			glNormal3d(normal.getX(), normal.getY(), normal.getZ());
			glTexCoord2f(scaling, scaling);
			glVertex3d(points.at(4).getX(), points.at(4).getY(), points.at(4).getZ());
			glTexCoord2f(scaling, 0.0);
			glVertex3d(points.at(5).getX(), points.at(5).getY(), points.at(5).getZ());
			glTexCoord2f(0.0, 0.0);
			glVertex3d(points.at(6).getX(), points.at(6).getY(), points.at(6).getZ());
			glTexCoord2f(0.0, scaling);
			glVertex3d(points.at(7).getX(), points.at(7).getY(), points.at(7).getZ());
		glEnd();

		firstCross = eighth - second;
		secondCross = fifth - second;

		normal = Vector3D::crossProduct(firstCross, secondCross);
		normal.normalize();

		if(textureType != objectFaceInfos.at(5)) {
			textureType = objectFaceInfos.at(5);

			if(textureType == defaultT || !mTextureImages.contains(textureType)) {
				glDisable(GL_TEXTURE_2D);
			}
			else {
				glEnable(GL_TEXTURE_2D);

				glBindTexture( GL_TEXTURE_2D, mTexture[0] );
				glTexImage2D( GL_TEXTURE_2D, 0, 3, mTextureImages.value(textureType).width(),
					mTextureImages.value(textureType).height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
					mTextureImages.value(textureType).bits() );
			}
		}

		glBegin(GL_POLYGON);
			glNormal3d(normal.getX(), normal.getY(), normal.getZ());
			glTexCoord2f(0.0, 0.0);
			glVertex3d(points.at(1).getX(), points.at(1).getY(), points.at(1).getZ());
			glTexCoord2f(0.0, scaling);
			glVertex3d(points.at(4).getX(), points.at(4).getY(), points.at(4).getZ());
			glTexCoord2f(scaling, scaling);
			glVertex3d(points.at(7).getX(), points.at(7).getY(), points.at(7).getZ());
			glTexCoord2f(scaling, 0.0);
			glVertex3d(points.at(2).getX(), points.at(2).getY(), points.at(2).getZ());
		glEnd();
	}
	else {
// 		glDisable(GL_TEXTURE_2D);
		QVector<Vector3D> points = boxGeom->getPoints();

		firstCross = third - first;
		secondCross = second - first;
		normal = Vector3D::crossProduct(firstCross, secondCross);
		normal.normalize();

 		glBegin(GL_POLYGON);
			glNormal3d(normal.getX(), normal.getY(), normal.getZ());
			glVertex3d(points.at(0).getX(), points.at(0).getY(), points.at(0).getZ());
			glVertex3d(points.at(1).getX(), points.at(1).getY(), points.at(1).getZ());
			glVertex3d(points.at(2).getX(), points.at(2).getY(), points.at(2).getZ());
			glVertex3d(points.at(3).getX(), points.at(3).getY(), points.at(3).getZ());
		glEnd();

		firstCross = seventh - third;
		secondCross = eighth - third;

		normal = Vector3D::crossProduct(firstCross, secondCross);
		normal.normalize();

		glBegin(GL_POLYGON);
			glNormal3d(normal.getX(), normal.getY(), normal.getZ());
			glVertex3d(points.at(2).getX(), points.at(2).getY(), points.at(2).getZ());
			glVertex3d(points.at(7).getX(), points.at(7).getY(), points.at(7).getZ());
			glVertex3d(points.at(6).getX(), points.at(6).getY(), points.at(6).getZ());
			glVertex3d(points.at(3).getX(), points.at(3).getY(), points.at(3).getZ());
		glEnd();

		firstCross = sixth - first;
		secondCross = seventh - first;

		normal = Vector3D::crossProduct(firstCross, secondCross);
		normal.normalize();

		glBegin(GL_POLYGON);
			glNormal3d(normal.getX(), normal.getY(), normal.getZ());
			glVertex3d(points.at(0).getX(), points.at(0).getY(), points.at(0).getZ());
			glVertex3d(points.at(5).getX(), points.at(5).getY(), points.at(5).getZ());
			glVertex3d(points.at(6).getX(), points.at(6).getY(), points.at(6).getZ());
			glVertex3d(points.at(3).getX(), points.at(3).getY(), points.at(3).getZ());
		glEnd();

		firstCross = fifth - first;
		secondCross = sixth - first;

		normal = Vector3D::crossProduct(firstCross, secondCross);
		normal.normalize();

		glBegin(GL_POLYGON);
			glNormal3d(normal.getX(), normal.getY(), normal.getZ());
			glVertex3d(points.at(0).getX(), points.at(0).getY(), points.at(0).getZ());
			glVertex3d(points.at(5).getX(), points.at(5).getY(), points.at(5).getZ());
			glVertex3d(points.at(4).getX(), points.at(4).getY(), points.at(4).getZ());
			glVertex3d(points.at(1).getX(), points.at(1).getY(), points.at(1).getZ());
		glEnd();

		firstCross = seventh - fifth;
		secondCross = sixth - fifth;

		normal = Vector3D::crossProduct(firstCross, secondCross);
		normal.normalize();

		glBegin(GL_POLYGON);
			glNormal3d(normal.getX(), normal.getY(), normal.getZ());
			glVertex3d(points.at(4).getX(), points.at(4).getY(), points.at(4).getZ());
			glVertex3d(points.at(5).getX(), points.at(5).getY(), points.at(5).getZ());
			glVertex3d(points.at(6).getX(), points.at(6).getY(), points.at(6).getZ());
			glVertex3d(points.at(7).getX(), points.at(7).getY(), points.at(7).getZ());
		glEnd();

		firstCross = eighth - second;
		secondCross = fifth - second;

		normal = Vector3D::crossProduct(firstCross, secondCross);
		normal.normalize();

		glBegin(GL_POLYGON);
			glNormal3d(normal.getX(), normal.getY(), normal.getZ());
			glVertex3d(points.at(1).getX(), points.at(1).getY(), points.at(1).getZ());
			glVertex3d(points.at(4).getX(), points.at(4).getY(), points.at(4).getZ());
			glVertex3d(points.at(7).getX(), points.at(7).getY(), points.at(7).getZ());
			glVertex3d(points.at(2).getX(), points.at(2).getY(), points.at(2).getZ());
		glEnd();
	}
}

void OpenGLVisualization::drawTriangleGeom(CollisionObject *currentCollisionObject, TriangleGeom *triangleGeom, bool hasATexture) {
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
	glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, mShininess->get());
	QVector<Triangle> triangles = triangleGeom->getTriangles();
	QVector<Vector3D> trianglePoints = triangleGeom->getPoints();

	if(hasATexture) {
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}

	int textureType = -1;
	QList<int> objectFaceInfos = currentCollisionObject->getTextureTypeInformation();
	for(int i = 0; i < objectFaceInfos.size() && i < triangles.size(); i++) {

		//cerr << "Size: " << triangles.size() << " " << objectFaceInfos.size() << " " << i << endl;

		if(objectFaceInfos.at(i) != textureType) {
			textureType = objectFaceInfos.at(i);
			glBindTexture( GL_TEXTURE_2D, mTexture[0] );
			glTexImage2D( GL_TEXTURE_2D, 0, 3, mTextureImages.value(textureType).width(),
				mTextureImages.value(textureType).height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
				mTextureImages.value(textureType).bits() );
		}
		Triangle current = triangleGeom->getTriangle(i);
		Vector3D normal;
		Vector3D first = triangleGeom->getPoint(current.mEdge[0]);
		Vector3D second = triangleGeom->getPoint(current.mEdge[1]);
		Vector3D third = triangleGeom->getPoint(current.mEdge[2]);

		Vector3D firstCross = second - first;
		Vector3D secondCross = third - first;

		normal = Vector3D::crossProduct(firstCross, secondCross);
		normal.normalize();

	if(hasATexture) {
		double scaling = 10.0;

  	glBegin(GL_POLYGON);
			glNormal3d(normal.getX(), normal.getY(), normal.getZ());
			glTexCoord2f(0.0, 0.0); glVertex3d(trianglePoints.at(current.mEdge[0]).getX(),
				trianglePoints.at(current.mEdge[0]).getY(),
				trianglePoints.at(current.mEdge[0]).getZ());
			glTexCoord2f(scaling, 0.0);
			glVertex3d(trianglePoints.at(current.mEdge[1]).getX(),
				 trianglePoints.at(current.mEdge[1]).getY(),
					trianglePoints.at(current.mEdge[1]).getZ());
			glTexCoord2f(scaling, scaling);
			glVertex3d(trianglePoints.at(current.mEdge[2]).getX(),
				trianglePoints.at(current.mEdge[2]).getY(),
				trianglePoints.at(current.mEdge[2]).getZ());
		glEnd();
	}
	else {
		glBegin(GL_POLYGON);
			glNormal3d(normal.getX(), normal.getY(), normal.getZ());
			glVertex3d(triangleGeom->getPoint(current.mEdge[0]).getX(),
				triangleGeom->getPoint(current.mEdge[0]).getY(),
				triangleGeom->getPoint(current.mEdge[0]).getZ());
			glVertex3d(triangleGeom->getPoint(current.mEdge[1]).getX(),
				triangleGeom->getPoint(current.mEdge[1]).getY(),
				triangleGeom->getPoint(current.mEdge[1]).getZ());
			glVertex3d(triangleGeom->getPoint(current.mEdge[2]).getX(),
				triangleGeom->getPoint(current.mEdge[2]).getY(),
				triangleGeom->getPoint(current.mEdge[2]).getZ());
		glEnd();
	}
	}
}


void OpenGLVisualization::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	if(value == mCurrentPosition) {
		mX = mCurrentPosition->getX();
		mY = mCurrentPosition->getY();
		mZ = mCurrentPosition->getZ();
	}
	else if(value == mReferenceBodyName) {

		if(mReferenceBody != 0) {
			mReferenceBody = 0;
			mReferenceBodyPosition = 0;
			mReferenceBodyOrientation = 0;
		}
		else {
			if(mReferenceBodyPosition != 0) {
				delete mReferenceBodyPosition;
				mReferenceBodyPosition = 0;
			}
			if(mReferenceBodyOrientation != 0) {
				delete mReferenceBodyOrientation;
				mReferenceBodyOrientation = 0;
			}
		}
		
		mReferenceBody = Physics::getPhysicsManager()->getSimBody(mReferenceBodyName->get());
		
		if(mReferenceBody != 0) {
			mReferenceBodyPosition = mReferenceBody->getPositionValue();
			mReferenceBodyOrientation = mReferenceBody->getQuaternionOrientationValue();
		}
		else {
			cerr << "check" << endl;
			if(mReferenceBodyName->get().trimmed() != "") {
				Core::log("OpenGLVisualization: Could not find reference body with name ["
						  + mReferenceBodyName->get() + "]. Ignoring", true);
			}
			
			mReferenceBodyPosition = new Vector3DValue(0.0, 0.0, 0.0);
			mReferenceBodyOrientation = new QuaternionValue();
		}
	}
	else if(value == mCurrentOrientation) {
		alpha = mCurrentOrientation->getX();
		beta = mCurrentOrientation->getY();
		gamma = mCurrentOrientation->getZ();
	}
	else if(value == mAmbientLight
			|| value == mDiffuseLight
			|| value == mSpecularLight
			|| value == mPosition
			|| value == mShininess)
	{
		mLightChanged = true;
	}
	else if(value == mPaintUpdateTimerInterval) {
		mUpdateInterval = mPaintUpdateTimerInterval->get();
		mTimerChanged = true;
	}
	else if(value == mOpeningAngleValue
		|| value == mMinDistanceCutoffValue
		|| value == mMaxDistanceCutoffValue)
	{
		if(value == mOpeningAngleValue
			&& (mOpeningAngleValue->get() > 180 || mOpeningAngleValue->get() < 0))
		{
			Core::getInstance()->scheduleTask(new ChangeDoubleValueTask(
				mOpeningAngleValue, mOpeningAngle));
		}
		if(value == mMinDistanceCutoffValue) {
			if(mMinDistanceCutoffValue->get() < 0) {
				Core::getInstance()->scheduleTask(new ChangeDoubleValueTask(
					mMinDistanceCutoffValue, mMinDistanceCutoff));
			}
		}
		if(value == mMaxDistanceCutoffValue) {
			if(mMaxDistanceCutoffValue->get() < 0) {
				Core::getInstance()->scheduleTask(new ChangeDoubleValueTask(
					mMaxDistanceCutoffValue, mMaxDistanceCutoff));
			}
		}
		if(mMinDistanceCutoffValue->get() > mMaxDistanceCutoffValue->get()) {
			if(value == mMinDistanceCutoffValue) {
				Core::getInstance()->scheduleTask(new ChangeDoubleValueTask(
					mMinDistanceCutoffValue, mMinDistanceCutoff));

			}
			else {
				Core::getInstance()->scheduleTask(new ChangeDoubleValueTask(
					mMaxDistanceCutoffValue, mMaxDistanceCutoff));
			}
		}
		mOpeningAngle = mOpeningAngleValue->get();
		mMinDistanceCutoff = mMinDistanceCutoffValue->get();
		mMaxDistanceCutoff = mMaxDistanceCutoffValue->get();
		mViewChanged = true;
	}
	else if(value == mRunInPerformanceMode || value == mPauseValue) {
		if((mRunInPerformanceMode == 0 || !mRunInPerformanceMode->get())
			&& (mPauseValue == 0 || !mPauseValue->get()))
		{
			emit startVisualizationTimer();
		}
	}
	else {
		return;
	}

	//make sure that changes of values can be applied in the visualization immediately.
	//if paused or in performance mode the paintGL method will deactivate the timer after
	//the first time, so this is save to do here.
	if(!mVisualizationTimer->isActive()) {
		emit startVisualizationTimer();
	}
}

void OpenGLVisualization::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	if(mPauseValue != 0 && mPauseValue->get()) {
		if(mTriggerPaintEvents.contains(event)) {
			if(!mVisualizationTimer->isActive()) {
				emit startVisualizationTimer();
			}
		}
	}
}

void OpenGLVisualization::changeView() {
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity ();

		gluPerspective((GLfloat) mOpeningAngleValue->get(), mWindowRation,
			mMinDistanceCutoffValue->get(), mMaxDistanceCutoffValue->get());

		glMatrixMode (GL_MODELVIEW);
		mViewChanged = false;
}

void OpenGLVisualization::mousePressEvent(QMouseEvent *e) {
	if(mIsManipulatable->get() && (mRunInPerformanceMode == 0 || !mRunInPerformanceMode->get())) {
		mLastMousePosition->setX(e->x());
		mLastMousePosition->setY(e->y());
		mLastMousePosition->setZ(0);

		if((e->button() & Qt::LeftButton) && !mLeftButtonPress) {
			mLeftButtonPress = true;

			mMouseClickPosition.set(e->x(), e->y(), 0.0);
			mMouseCurrentPosition.set(mMouseClickPosition);

			if(!mChangeViewportTimer->isActive()) {
				mChangeViewportTimer->start(mViewPortUpdateTimerInterval->get());
			}

		}
		if(e->button() & Qt::RightButton) {
			mRightButtonPress = true;
		}
		if(e->button() & Qt::MidButton) {
			mMiddleButtonPress = true;
		}
		if((mRunInPerformanceMode == 0 || !mRunInPerformanceMode->get())
			&& !mVisualizationTimer->isActive())
		{
			//make sure that the visualization remains reactive while a mouse button is pressed.
			emit startVisualizationTimer();
		}
	}
}

void OpenGLVisualization::mouseReleaseEvent(QMouseEvent *e) {
	if(mIsManipulatable->get()
		&& (mRunInPerformanceMode == 0 || !mRunInPerformanceMode->get()))
	{

		ChangeVectorValueTask *adaptOrientationTask =
				new ChangeVectorValueTask(mCurrentOrientation, alpha, beta, gamma);
		Core::getInstance()->scheduleTask(adaptOrientationTask);

		ChangeVectorValueTask *adaptPositionTask =
				new ChangeVectorValueTask(mCurrentPosition, mX, mY, mZ);
		Core::getInstance()->scheduleTask(adaptPositionTask);

		if(e->button() & Qt::LeftButton) {
			mLeftButtonPress = false;
			if(mPressedMovementKeys == 0) {
				mChangeViewportTimer->stop();
			}
		}
		if(e->button() & Qt::RightButton) {
			mRightButtonPress = false;
		}
		if(e->button() & Qt::MidButton) {
			mMiddleButtonPress = false;
		}
	}
	if((mPauseValue != 0 && mPauseValue->get())
		|| (mRunInPerformanceMode != 0 && mRunInPerformanceMode->get()))
	{
		//stop visualization timer if it was only active during mouse manipulations.
		emit stopVisualizationTimer();
	}
}

void OpenGLVisualization::mouseMoveEvent(QMouseEvent *e) {
	if(mIsManipulatable->get()) {

		if(mRunInPerformanceMode == 0 || !mRunInPerformanceMode->get()) {
			double factor = 0.0;
			if(mShiftHold) {
				factor = mMaxMoveStepSize->get();
			}
			else {
				factor = mMinMoveStepSize->get();
			}
			factor /= 10.0;
			double offsetY = -factor * (e->y() - mLastMousePosition->getY());
			double offsetX = -factor * (e->x() - mLastMousePosition->getX());

			if(mRightButtonPress) {
				Quaternion rot;
				rot.setFromAngles(mCurrentOrientation->getX(),
									mCurrentOrientation->getY(),
									mCurrentOrientation->getZ());
				Quaternion inverse;
				inverse = rot.getInverse();
				Quaternion startPoint(0.0, -offsetX, -offsetY, 0.0);
				if(mSwitchYZAxes == 0 || mSwitchYZAxes->get()) {
					startPoint = Quaternion(0.0, -offsetX, 0.0, -offsetY);
				}
				Quaternion rotated = rot * startPoint * inverse;

				mX += rotated.getX();
				mY += rotated.getY();
				mZ += rotated.getZ();
			}

			if(mMiddleButtonPress) {
				Quaternion rot;
				rot.setFromAngles(mCurrentOrientation->getX(),
									mCurrentOrientation->getY(),
									mCurrentOrientation->getZ());
				Quaternion inverse;
				inverse = rot.getInverse();
				Quaternion startPoint(0.0, 0.0, offsetY, 0.0);
				Quaternion rotated = rot * startPoint * inverse;

				mX += rotated.getX();
				mY += rotated.getY();
				mZ += rotated.getZ();
			}
			if(mLeftButtonPress) {
				mMouseCurrentPosition.set(e->x(), e->y(), 0.0);
			}
		}

		mLastMousePosition->set(e->x(), e->y(), 0);
	}
}

void OpenGLVisualization::keyReleaseEvent(QKeyEvent *e) {
	if(mIsManipulatable->get()){
		if(e->isAutoRepeat()) {
			return;
		}
		if(e->key() == Qt::Key_Shift) {
			mShiftHold = false;
		}
		else if(e->key() == Qt::Key_W || e->key() == Qt::Key_Up) {
			mPressedMovementKeys = mPressedMovementKeys & ~KEY_FWD;
		}
		else if(e->key() == Qt::Key_S || e->key() == Qt::Key_Down) {
			mPressedMovementKeys = mPressedMovementKeys & ~KEY_BWD;
		}
		else if(e->key() == Qt::Key_A || e->key() == Qt::Key_Left) {
			mPressedMovementKeys = mPressedMovementKeys & ~KEY_LEFT;
		}
		else if(e->key() == Qt::Key_D || e->key() == Qt::Key_Right) {
			mPressedMovementKeys = mPressedMovementKeys & ~KEY_RIGHT;
		}
		else if(e->key() == Qt::Key_E || e->key() == Qt::Key_PageUp) {
			mPressedMovementKeys = mPressedMovementKeys & ~KEY_UP;
		}
		else if(e->key() == Qt::Key_Q || e->key() == Qt::Key_PageDown) {
			mPressedMovementKeys = mPressedMovementKeys & ~KEY_DOWN;
		}
		if(mPressedMovementKeys == 0 && !mLeftButtonPress) {
			mChangeViewportTimer->stop();
		}
	}
}


void OpenGLVisualization::moveViewPoint() {
	if(mIsManipulatable->get()){
		//consider rotation
		if(mLeftButtonPress) {

			alpha += -(-mMouseClickPosition.getY() + mMouseCurrentPosition.getY())
						* mMouseRotationStepSize->get();
			if(mSwitchYZAxes == 0 || mSwitchYZAxes->get()) {
				beta += -(-mMouseClickPosition.getX() + mMouseCurrentPosition.getX())
						* mMouseRotationStepSize->get();
			}
			else {
				gamma += -(-mMouseClickPosition.getX() + mMouseCurrentPosition.getX())
					* mMouseRotationStepSize->get();
			}

			//switch between helicopter mode.
			mMouseClickPosition.set(mMouseCurrentPosition);
		}

		//consider translation
		double a = 0.0;
		double b = 0.0;
		double c = 0.0;
		double d = 0.0;

		double moveIncrement = mMinMoveStepSize->get();
		double sideIncrement = mMinSideStepSize->get();

		if(mShiftHold) {
			moveIncrement = mMaxMoveStepSize->get();
			sideIncrement = mMaxSideStepSize->get();
		}

		if((mPressedMovementKeys & KEY_FWD) == KEY_FWD) {
			d -= moveIncrement;
		}
		if((mPressedMovementKeys & KEY_BWD) == KEY_BWD) {
			d += moveIncrement;
		}
		if((mPressedMovementKeys & KEY_LEFT) == KEY_LEFT) {
			b -= sideIncrement;
		}
		if((mPressedMovementKeys & KEY_RIGHT) == KEY_RIGHT) {
			b += sideIncrement;
		}
		if((mPressedMovementKeys & KEY_UP) == KEY_UP) {
			c += sideIncrement;
		}
		if((mPressedMovementKeys & KEY_DOWN) == KEY_DOWN) {
			c -= sideIncrement;		}

		Quaternion rot;
		rot.setFromAngles(alpha, beta, gamma);
		Quaternion inverse;
		inverse = rot.getInverse();
		Quaternion startPoint(a, b, c, d);
		Quaternion rotated = rot * startPoint * inverse;

		mX += rotated.getX();
		mY += rotated.getY();
		mZ += rotated.getZ();

		ChangeVectorValueTask *adaptPositionTask =
				new ChangeVectorValueTask(mCurrentPosition, mX, mY, mZ);
		Core::getInstance()->scheduleTask(adaptPositionTask);
	}
}

void OpenGLVisualization::wheelEvent(QWheelEvent *e) {
	if(e == 0) {
		return;
	}
	if(e->delta() > 0) {
		unsigned int oldKeyState = mPressedMovementKeys;
		mPressedMovementKeys = mPressedMovementKeys | KEY_FWD;
		moveViewPoint();
		mPressedMovementKeys = oldKeyState;
	}
	else if(e->delta() < 0) {
		unsigned int oldKeyState = mPressedMovementKeys;
		mPressedMovementKeys = mPressedMovementKeys | KEY_BWD;
		moveViewPoint();
		mPressedMovementKeys = oldKeyState;
	}
}


/**
 * Enables the manipulation of the simulation by the user via keyboard. If the OpenGLVisualization is started with mCanManipulate being false, the manipulation is disabled.
 * @param e
 */
void OpenGLVisualization::keyPressEvent(QKeyEvent *e) {
	if(mIsManipulatable->get()) {
		if(e->key() == Qt::Key_Shift) {
			mShiftHold = true;
		}
			// switch between paused/running
			// changing to e->key() == Qt::Key_P will result in a case insensitve version.
		else if(e->text() == "+") {
			mMaxMoveStepSize->set(mMaxMoveStepSize->get() + mStepSizeDistance);
		}
		else if(e->text() == "-") {
			if(mMaxMoveStepSize->get() > mStepSizeDistance) {
				mMaxMoveStepSize->set(mMaxMoveStepSize->get() - mStepSizeDistance);
			}
		}
		else if(e->key() == Qt::Key_X && (e->modifiers() & Qt::ControlModifier)){
				mUseTexturesValue->set(!mUseTexturesValue->get());
		}
		else if(e->key() == Qt::Key_T && (e->modifiers() & Qt::ControlModifier)){
				mDisplaySimulationTime->set(!mDisplaySimulationTime->get());
		}
		else if(e->key() == Qt::Key_A && (e->modifiers() & Qt::ControlModifier)){
				mDrawCoordinateAxes->set(!mDrawCoordinateAxes->get());
		}
		else if(e->key() == Qt::Key_V && (e->modifiers() & Qt::ControlModifier)&& !(e->modifiers() & Qt::ShiftModifier)) {
			resetViewport();
		}
		else if(e->key() == Qt::Key_V) {
			if((e->modifiers() & Qt::ControlModifier) && (e->modifiers() & Qt::ShiftModifier))
			{
				mStartPosition->set(mCurrentPosition->get());
				mStartOrientation->set(mCurrentOrientation->get());
			}
		}
		else if(e->key() == Qt::Key_G && (e->modifiers() & Qt::ControlModifier)) {
			mShowCoordinateSystemLines->set(!mShowCoordinateSystemLines->get());
		}
		
		//TODO moveOffset and sideOffset seem to be relicts from previous refactorings. Check and remove!
// 		double moveOffset = mMaxMoveStepSize->get();
// 		double sideOffset = mMaxSideStepSize->get();
		if(mShiftHold == true) {
// 			moveOffset = mMinMoveStepSize->get();
// 			sideOffset = mMinSideStepSize->get();
		}
		else if(e->key() == Qt::Key_F
				&& (e->modifiers() & Qt::ControlModifier)
				&& (e->modifiers() & Qt::AltModifier)){
				//switch framegrabber mode.
				mUseAsFrameGrabber->set(!mUseAsFrameGrabber->get());
		}

		int timerInterval = mViewPortUpdateTimerInterval->get();
		bool viewChanged = false;

		if(e->text() == "d" ||e->text() == "D" || e->key() == Qt::Key_Right){
			mPressedMovementKeys = mPressedMovementKeys | KEY_RIGHT;
			if(!mChangeViewportTimer->isActive()) {
				mChangeViewportTimer->start(timerInterval);
			}

		}
		if(e->text() == "w" || e->text() == "W" || e->key() == Qt::Key_Up){
			mPressedMovementKeys = mPressedMovementKeys | KEY_FWD;
			if(!mChangeViewportTimer->isActive()) {
				mChangeViewportTimer->start(timerInterval);
			}
		}
		if(e->text() == "a" || e->text() == "A" || e->key() == Qt::Key_Left){
			mPressedMovementKeys = mPressedMovementKeys | KEY_LEFT;
			if(!mChangeViewportTimer->isActive()) {
				mChangeViewportTimer->start(timerInterval);
			}
		}
		if(e->text() == "s" || e->text() == "S" || e->key() == Qt::Key_Down){
			mPressedMovementKeys = mPressedMovementKeys | KEY_BWD;
			if(!mChangeViewportTimer->isActive()) {
				mChangeViewportTimer->start(timerInterval);
			}
		}
		if(e->text() == "e" || e->text() == "E" || e->key() == Qt::Key_PageUp){
			mPressedMovementKeys = mPressedMovementKeys | KEY_UP;
			if(!mChangeViewportTimer->isActive()) {
				mChangeViewportTimer->start(timerInterval);
			}
		}
		if(e->text() == "q" || e->text() == "Q" || e->key() == Qt::Key_PageDown){
			mPressedMovementKeys = mPressedMovementKeys | KEY_DOWN;
			if(!mChangeViewportTimer->isActive()) {
				mChangeViewportTimer->start(timerInterval);
			}
		}

		if(viewChanged) {
			ChangeVectorValueTask *adaptPositionTask =
					new ChangeVectorValueTask(mCurrentPosition, mX, mY, mZ);
			Core::getInstance()->scheduleTask(adaptPositionTask);
		}

		if(!mVisualizationTimer->isActive()) {
			emit startVisualizationTimer();
		}
	}
}

void OpenGLVisualization::changeVisibility() {

	if(isHidden()) {
		move(mWindowPosX, mWindowPosY);
		show();
	}
	else {
		hide();
		mWindowPosX = x();
		mWindowPosY = y();
	}

}

void OpenGLVisualization::drawSky()
{
		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, mSkyTexture[1]);
		glTexImage2D( GL_TEXTURE_2D, 0, 3, mSkyTextureImage.width(), mSkyTextureImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, mSkyTextureImage.bits());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_S);
		glTexGenf( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGenf( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

// 		static float offset = 0.0f;
//
// 		float x = SCALE_SKY;
//
// 		float z = mCurrentPosition->getY() + HEIGHT_SKY;
// 		if(mCurrentPosition->getY() < 0)
// 		{
// 			z = HEIGHT_SKY;
// 		}
// 		glMatrixMode(GL_TEXTURE);
// 		glTranslatef(off,0,0);
// 		off += 0.00000005;
// 		if( off > 1) {
// 			off = -1;
// 		}
		gluSphere(mGluObject, 50, 48, 48);
// 		glBegin (GL_QUADS);
// 		glNormal3f (0,0,-1);
// 		glTexCoord2f (-x+offset,-x+offset);
// 		glVertex3f (-SKY_SIZE + mCurrentPosition->getX(),z,-SKY_SIZE + mCurrentPosition->getZ());
// 		glTexCoord2f (-x+offset,x+offset);
// 		glVertex3f (-SKY_SIZE+ mCurrentPosition->getX(),z,SKY_SIZE + mCurrentPosition->getZ());
// 		glTexCoord2f (x+offset,x+offset);
// 		glVertex3f (SKY_SIZE+mCurrentPosition->getX(),z,SKY_SIZE + mCurrentPosition->getZ());
// 		glTexCoord2f (x+offset,-x+offset);
// 		glVertex3f (SKY_SIZE+ mCurrentPosition->getX(),z,-SKY_SIZE + mCurrentPosition->getZ());
// 		glEnd();

		//Tempo of the sky
// 		offset = offset + 0.00025f;
// 		if (offset > 1) offset -= 1;

		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_S);
		glMatrixMode(GL_MODELVIEW);
		glDisable(GL_TEXTURE_2D);
// 	}
}

double OpenGLVisualization::getDefaultOpeningAngle() {
	return 30.0;
}

double OpenGLVisualization::getDefaultMinCutoff() {
	return 0.1;
}

double OpenGLVisualization::getDefaultMaxCutoff() {
	return 200.0;
}



void OpenGLVisualization::drawCylinder(CollisionObject *currentCollisionObject, bool hasATexture) {
	glPushMatrix();

	CylinderGeom *cylinderGeom = dynamic_cast<CylinderGeom*>(currentCollisionObject->getGeometry());


	Vector3D pos = cylinderGeom->getLocalPosition();
	glTranslated(pos.getX(), pos.getY(), pos.getZ());

	Quaternion quaternion = cylinderGeom->getLocalOrientation();
	double angle = 2 * acos(quaternion.getW());
	double scale = sqrt(quaternion.getX() * quaternion.getX()
							+ quaternion.getY() * quaternion.getY()
							+ quaternion.getZ() * quaternion.getZ());
	double x  = 0.0;
	double y = 0.0;
	double z = 0.0;

	if(scale != 0) {
		x = quaternion.getX() / scale;
		y = quaternion.getY() / scale;
		z = quaternion.getZ() / scale;
	}
	angle = angle*180.0/Math::PI;
	glRotated(angle, x, y, z);

	double radius = cylinderGeom->getRadius();
	double height = cylinderGeom->getLength();

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
	glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, mShininess->get());


	if(hasATexture) {
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		int textureType = -1;
		QList<int> objectFaceInfos = currentCollisionObject->getTextureTypeInformation();

		int defaultT  = Physics::getCollisionManager()->getMaterialProperties()->getTextureIndex("Default");

		textureType = objectFaceInfos.at(0);

		if(textureType == defaultT || !mTextureImages.contains(textureType)) {
			glDisable(GL_TEXTURE_2D);
		}
		glBindTexture( GL_TEXTURE_2D, mTexture[0] );
		glTexImage2D( GL_TEXTURE_2D, 0, 3, mTextureImages.value(textureType).width(), mTextureImages.value(textureType).height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 	mTextureImages.value(textureType).bits());
	}
	else {
		glDisable(GL_TEXTURE_2D);
	}

	double zoffset = 0.0;
	double l = height;
	double r = radius;
 	int i;

	//Taken from drawstuff.cpp (ODE)
  float tmp,ny,nz,a,ca,sa;
  const int n = 24;	// number of sides to the cylinder (divisible by 4)

  l *= 0.5;
  a = float(M_PI*2.0)/float(n);
  sa = (float) sin(a);
  ca = (float) cos(a);

  // draw cylinder body
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_STRIP);
  for (i=0; i<=n; i++) {
    glNormal3d (ny,nz,0);
    glVertex3d (ny*r,nz*r,l+zoffset);
    glNormal3d (ny,nz,0);
    glVertex3d (ny*r,nz*r,-l+zoffset);
    // rotate ny,nz
    tmp = ca*ny - sa*nz;
    nz = sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();

  // draw top cap
  glShadeModel (GL_FLAT);
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_FAN);
  glNormal3d (0,0,1);
  glVertex3d (0,0,l+zoffset);
  for (i=0; i<=n; i++) {
    glNormal3d (0,0,1);
    glVertex3d (ny*r,nz*r,l+zoffset);

    // rotate ny,nz
    tmp = ca*ny - sa*nz;
    nz = sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();

  // draw bottom cap
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_FAN);
  glNormal3d (0,0,-1);
  glVertex3d (0,0,-l+zoffset);
  for (i=0; i<=n; i++) {
    glNormal3d (0,0,-1);
    glVertex3d (ny*r,nz*r,-l+zoffset);

    // rotate ny,nz
    tmp = ca*ny + sa*nz;
    nz = -sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();	glPopMatrix();
}


void OpenGLVisualization::drawRay(CollisionObject *currentCollisionObject) {
/*
	RayGeom *ray = dynamic_cast<RayGeom*>(currentCollisionObject->getGeometry());
	if(ray == 0) {
		return;
	}
	double length = ray->getVisibleLength();
	Vector3D startPoint = ray->getLocalPosition();
	Vector3D endPoint = startPoint;
	endPoint.setZ(endPoint.getZ() + length);
	
	Vector3D pos = ray->getLocalPosition();
	
	Quaternion localRayOrientation = ray->getLocalOrientation();
	
	Vector3D newPoint;
	Quaternion back(localRayOrientation);
	back.setW(-back.getW());
	Quaternion inv = back.getInverse();
	Quaternion old(0, 0, 0, 0);
	
	Quaternion inverse = localRayOrientation.getInverse();
	
	old.set(0, startPoint.getX(), startPoint.getY(), startPoint.getZ());
	Quaternion newPointQ = localRayOrientation * old * inverse;
	startPoint.set(newPointQ.getX(), newPointQ.getY(), newPointQ.getZ());
	
	old.set(0, endPoint.getX(), endPoint.getY(), endPoint.getZ());
	newPointQ = localRayOrientation * old * inverse;
	endPoint.set(newPointQ.getX(), newPointQ.getY(), newPointQ.getZ());

	
	glPushMatrix();
	glShadeModel(GL_FLAT);
	glTranslated(pos.getX(), pos.getY(), pos.getZ());
	//glRotated(angle, x, y, z);
	
	
	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex3d(startPoint.getX(), startPoint.getY(), startPoint.getZ());
	glVertex3d(endPoint.getX(), endPoint.getY(), endPoint.getZ());
	glEnd();
	glPopMatrix();
*/	
	
	 //TODO the above changes are only testwise!
	 
	 RayGeom *ray = dynamic_cast<RayGeom*>(currentCollisionObject->getGeometry());
	 if(ray == 0) {
		 return;
	 }
	 double length = ray->getVisibleLength();
	 Vector3D startPoint = ray->getLocalPosition();
	 Vector3D endPoint = startPoint;
	 endPoint.setZ(endPoint.getZ() + length);
	 
	 Vector3D pos = ray->getLocalPosition();
	 
	 Quaternion quaternion = ray->getLocalOrientation();
	 double angle = 2 * acos(quaternion.getW());
	 double scale = sqrt(quaternion.getX() * quaternion.getX() + quaternion.getY()
						* quaternion.getY() + quaternion.getZ() * quaternion.getZ());
	 double x = 0.0;
	 double y = 0.0;
	 double z = 0.0;
	 
	 if(scale != 0.0) {
		 x = quaternion.getX() / scale;
		 y = quaternion.getY() / scale;
		 z = quaternion.getZ() / scale;
	 }
	 angle = angle * 180.0 / Math::PI;
	 
	 glPushMatrix();
	 glShadeModel(GL_FLAT);
	 glTranslated(pos.getX(), pos.getY(), pos.getZ());
	 glRotated(angle, x, y, z);
	 
	 
	 
	 glLineWidth(1);
	 glBegin(GL_LINES);
	 glVertex3d(startPoint.getX(), startPoint.getY(), startPoint.getZ());
	 glVertex3d(endPoint.getX(), endPoint.getY(), endPoint.getZ());
	 glEnd();
	 glPopMatrix();
	 
// 	RayGeom *ray = dynamic_cast<RayGeom*>(currentCollisionObject->getGeometry());
// 	if(ray == 0) {
// 		return;
// 	}
// 	double length = ray->getVisibleLength();
// 	Vector3D startPoint = ray->getLocalPosition();
// 	Vector3D endPoint = startPoint;
// 	endPoint.setZ(endPoint.getZ() + length);
// 
// 	Vector3D pos = ray->getLocalPosition();
// 
// 	Quaternion quaternion = ray->getLocalOrientation();
// 	double angle = 2 * acos(quaternion.getW());
// 	double scale = sqrt(quaternion.getX() * quaternion.getX() + quaternion.getY()
// 		* quaternion.getY() + quaternion.getZ() * quaternion.getZ());
// 	double x = 0.0;
// 	double y = 0.0;
// 	double z = 0.0;
// 
// 	if(scale != 0.0) {
// 		x = quaternion.getX() / scale;
// 		y = quaternion.getY() / scale;
// 		z = quaternion.getZ() / scale;
// 	}
// 	angle = angle * 180.0 / Math::PI;
// 
// 	glPushMatrix();
// 	glShadeModel(GL_FLAT);
// 	glTranslated(pos.getX(), pos.getY(), pos.getZ());
// 	glRotated(angle, x, y, z);
// 	
// 
// 	glLineWidth(1);
// 	glBegin(GL_LINES);
// 		glVertex3d(startPoint.getX(), startPoint.getY(), startPoint.getZ());
// 		glVertex3d(endPoint.getX(), endPoint.getY(), endPoint.getZ());
// 	glEnd();
// 	glPopMatrix();
	
	
}


void OpenGLVisualization::drawSphere(CollisionObject *currentCollisionObject, bool hasATexture) {
	if(hasATexture) {
		int type = currentCollisionObject->getTextureTypeInformation().at(0);
		glBindTexture( GL_TEXTURE_2D, mTexture[0]);
		glTexImage2D( GL_TEXTURE_2D, 0, 3, mTextureImages.value(type).width(),
			mTextureImages.value(type).height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
			mTextureImages.value(type).bits());
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
	SphereGeom *sphereGeom = dynamic_cast<SphereGeom*>(currentCollisionObject->getGeometry());
	if(sphereGeom != 0) {
		Vector3D pos = sphereGeom->getLocalPosition();
		glPushMatrix();
		glTranslated(pos.getX(), pos.getY(), pos.getZ());
		gluSphere(mGluObject, sphereGeom->getRadius(), 48, 48);
		glPopMatrix();
	}
}


void OpenGLVisualization::drawCapsule(CollisionObject *currentCollisionObject, bool hasATexture) {

	glPushMatrix();

	CapsuleGeom *capsuleGeom = dynamic_cast<CapsuleGeom*>(currentCollisionObject->getGeometry());


	Vector3D pos = capsuleGeom->getLocalPosition();
	glTranslated(pos.getX(), pos.getY(), pos.getZ());

	Quaternion quaternion = capsuleGeom->getLocalOrientation();
	double angle = 2 * acos(quaternion.getW());
	double scale = sqrt(quaternion.getX() * quaternion.getX()
							+ quaternion.getY() * quaternion.getY()
							+ quaternion.getZ() * quaternion.getZ());
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;

	if(scale != 0) {
		x = quaternion.getX() / scale;
		y = quaternion.getY() / scale;
		z = quaternion.getZ() / scale;
	}
	angle = angle * 180.0 / Math::PI;
	glRotated(angle, x, y, z);

	double radius = capsuleGeom->getRadius();
	double height = capsuleGeom->getLength();

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
	glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, mShininess->get());


	if(hasATexture) {
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		int textureType = -1;
		QList<int> objectFaceInfos = currentCollisionObject->getTextureTypeInformation();

		int defaultT  = Physics::getCollisionManager()->getMaterialProperties()->getTextureIndex("Default");

		textureType = objectFaceInfos.at(0);

		if(textureType == defaultT || !mTextureImages.contains(textureType)) {
			glDisable(GL_TEXTURE_2D);
		}
		glBindTexture( GL_TEXTURE_2D, mTexture[0] );
		glTexImage2D( GL_TEXTURE_2D, 0, 3, mTextureImages.value(textureType).width(), mTextureImages.value(textureType).height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 	mTextureImages.value(textureType).bits());
	}
	else {
		glDisable(GL_TEXTURE_2D);
	}

	//double zoffset = 0.0; //TODO what was this for?
	double l = height;
	double r = radius;

	//Rest taken from drawstuff.cpp (ODE)
	//******************************

	int i,j;
	float tmp,nx,ny,nz,start_nx,start_ny,a,ca,sa;
	// number of sides to the cylinder (divisible by 4):
	const int n = 24;

	l *= 0.5;
	a = float(M_PI*2.0)/float(n);
	sa = (float) sin(a);
	ca = (float) cos(a);

	// draw cylinder body
	ny=1; nz=0;		  // normal vector = (0,ny,nz)
	glBegin (GL_TRIANGLE_STRIP);
	for (i=0; i<=n; i++) {
		glNormal3d (ny,nz,0);
		glVertex3d (ny*r,nz*r,l);
		glNormal3d (ny,nz,0);
		glVertex3d (ny*r,nz*r,-l);
		// rotate ny,nz
		tmp = ca*ny - sa*nz;
		nz = sa*ny + ca*nz;
		ny = tmp;
	}
	glEnd();

	// draw first cylinder cap
	start_nx = 0;
	start_ny = 1;
	for (j=0; j<(n/4); j++) {
		// get start_n2 = rotated start_n
		float start_nx2 =  ca*start_nx + sa*start_ny;
		float start_ny2 = -sa*start_nx + ca*start_ny;
		// get n=start_n and n2=start_n2
		nx = start_nx; ny = start_ny; nz = 0;
		float nx2 = start_nx2, ny2 = start_ny2, nz2 = 0;
		glBegin (GL_TRIANGLE_STRIP);
		for (i=0; i<=n; i++) {
			glNormal3d (ny2,nz2,nx2);
			glVertex3d (ny2*r,nz2*r,l+nx2*r);
			glNormal3d (ny,nz,nx);
			glVertex3d (ny*r,nz*r,l+nx*r);
			// rotate n,n2
			tmp = ca*ny - sa*nz;
			nz = sa*ny + ca*nz;
			ny = tmp;
			tmp = ca*ny2- sa*nz2;
			nz2 = sa*ny2 + ca*nz2;
			ny2 = tmp;
		}
		glEnd();
		start_nx = start_nx2;
		start_ny = start_ny2;
	}

	// draw second cylinder cap
	start_nx = 0;
	start_ny = 1;
	for (j=0; j<(n/4); j++) {
		// get start_n2 = rotated start_n
		float start_nx2 = ca*start_nx - sa*start_ny;
		float start_ny2 = sa*start_nx + ca*start_ny;
		// get n=start_n and n2=start_n2
		nx = start_nx; ny = start_ny; nz = 0;
		float nx2 = start_nx2, ny2 = start_ny2, nz2 = 0;
		glBegin (GL_TRIANGLE_STRIP);
		for (i=0; i<=n; i++) {
			glNormal3d (ny,nz,nx);
			glVertex3d (ny*r,nz*r,-l+nx*r);
			glNormal3d (ny2,nz2,nx2);
			glVertex3d (ny2*r,nz2*r,-l+nx2*r);
			// rotate n,n2
			tmp = ca*ny - sa*nz;
			nz = sa*ny + ca*nz;
			ny = tmp;
			tmp = ca*ny2- sa*nz2;
			nz2 = sa*ny2 + ca*nz2;
			ny2 = tmp;
		}
		glEnd();
		start_nx = start_nx2;
		start_ny = start_ny2;
	}

	glPopMatrix();
}

}
