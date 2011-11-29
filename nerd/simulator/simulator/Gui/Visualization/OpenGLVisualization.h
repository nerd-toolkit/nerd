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
 *   Copyright (C) 2008 - 2010 by the Neurocybernetics Groups Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work is funded by EU-Project Number ICT 214856                   *
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



#ifndef OpenGLVisualization_H_
#define OpenGLVisualization_H_
#include <QGLWidget>
#include <QWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include "Event/EventListener.h"
#include "Value/ValueChangedListener.h"
#include "Math/Quaternion.h"
#include "Gui/ControlWindow.h"
#include <QFocusEvent>
#include "Core/ParameterizedObject.h"
#include "Math/Vector3D.h"
#include "Event/EventListener.h"
#include "Event/Event.h"
#include <QImage>
#include <QGLFramebufferObject>
#include "PlugIns/CommandLineArgument.h"


namespace nerd{

class SimBody;
class BoolValue;
class Vector3DValue;
class TriangleGeom;
class ColorValue;
class SimJoint;
class CollisionManager;
class QuaternionValue;
class PlaneBody;
class CollisionObject;

/**
* OpenGLVisualization
**/
class OpenGLVisualization : public QGLWidget, public ParameterizedObject,
							public virtual EventListener
{
	
	Q_OBJECT

	public:
		OpenGLVisualization(bool isManipulatable, SimBody *referenceBody, 
					const QString &name, bool publishValues=false, QWidget *w = 0);
		virtual ~OpenGLVisualization();
		void init();
		virtual void valueChanged(Value *value);
		virtual void eventOccured(Event *event);
		virtual QString getName() const;
		
		static double getDefaultOpeningAngle();
		static double getDefaultMinCutoff();
		static double getDefaultMaxCutoff();

	signals:
		void deleting(QString name);
		void startVisualizationTimer();
		void stopVisualizationTimer();

	public slots:
		void keyPressEvent(QKeyEvent *e);	
		void keyReleaseEvent(QKeyEvent *e);
		void resetViewport();
		void changeVisibility();
		void closeEvent(QCloseEvent *e);

	protected slots:
		void mousePressEvent(QMouseEvent *e);
		void mouseReleaseEvent(QMouseEvent *e);
		void mouseMoveEvent(QMouseEvent *e);
		void moveViewPoint();
	
	private slots:
		void updateGL();
	
	protected:
		void paintGL();
		void initializeGL();
		void resizeGL(int width, int height);
		void focusOutEvent(QFocusEvent *event);
	
	private:
		void updateVisualization();
		void createEnvironmentConditions();
		void drawTriangleGeom(CollisionObject *currentCollisionObject, TriangleGeom *triangleGeom, bool hasATexture);
		void drawBox(CollisionObject *currentCollisionObject, bool hasATexture);
		void drawCylinder(CollisionObject *currentCollisionObject, bool hasATexture);
		void drawRay(CollisionObject *currentCollisionObject);
		void drawSphere(CollisionObject *currentCollisionObject, bool hasATexture);
		void drawCapsule(CollisionObject *currentCollisionObject, bool hasATexture);
		void drawAxis();
		void setUp();
		void changeLighting();
		void drawSky();
		void drawPlane(PlaneBody *body);
		void changeView();

		void loadTextures();

	private:
		double mWindowRation;
		double off;
		bool mLightChanged;
		bool mPublishValues;
		bool mInitSuccessful;	
		bool mShiftHold;	
		bool mRightButtonPress;
		bool mLeftButtonPress;
		bool mMiddleButtonPress;
		bool mShowAxisAndCollisionObjects; 

		bool mTimerChanged;
		bool mViewChanged;
		
		int mWindowPosX;
		int mWindowPosY;
		int mUpdateInterval;
		unsigned int mPressedMovementKeys;

		double alpha;
		double beta;
		double gamma;
		double mX;
		double mY;
		double mZ;
		double mStepSizeDistance;
		
		SimBody *mReferenceBody;
		StringValue *mReferenceBodyName;

		Vector3D *mLastMousePosition;
		Vector3DValue *mPosition;
		Vector3DValue *mDiffuseLight;
		Vector3DValue *mSpecularLight;
		Vector3DValue *mAmbientLight;
		Vector3DValue *mStartPosition;
		Vector3DValue *mStartOrientation;
		Vector3DValue *mCurrentPosition;
		Vector3DValue *mCurrentOrientation;
		Vector3DValue* mReferenceBodyPosition;
		
		QuaternionValue *mReferenceBodyOrientation;
		
		IntValue *mViewPortUpdateTimerInterval;
		IntValue *mPaintUpdateTimerInterval;
	
		DoubleValue *mShininess;
		DoubleValue *mMinMoveStepSize;
		DoubleValue *mMaxMoveStepSize;
		DoubleValue *mMinSideStepSize;
		DoubleValue *mMaxSideStepSize;
		DoubleValue *mMouseRotationStepSize;	
		DoubleValue *mSimulationTime;
		DoubleValue *mRealTime;
		IntValue *mTimeDisplaySize;
		DoubleValue *mOpeningAngleValue;

		DoubleValue *mMinDistanceCutoffValue;
		DoubleValue *mMaxDistanceCutoffValue;
		double mMinDistanceCutoff;
		double mMaxDistanceCutoff;
		double mOpeningAngle;

		ColorValue *mClearColorValue;
		ColorValue *mTimeTextColorValue;
			
		BoolValue *mDrawCoordinateAxes;
		BoolValue *mIsManipulatable;
		BoolValue *mPauseValue;	
		BoolValue *mUseTexturesValue;
		BoolValue *mDisplaySimulationTime;
		BoolValue *mShowCoordinateSystemLines;
		BoolValue *mShowStepsPerSecond;
		BoolValue *mRealTimeRecorderRunning;

		BoolValue *mIsTranslationalValue;
		BoolValue *mIgnoreTranslationalYAxis;

		QString mVisualizationName;

		QTimer *mChangeViewportTimer;	
		QTimer *mVisualizationTimer;
	
		Vector3D mMouseClickPosition;
		Vector3D mMouseCurrentPosition;

		GLUquadricObj *mGluObject;		
		GLuint mTexture[1];
		GLuint	mSkyTexture[1];

		QHash<int, QImage> mTextureImages;
		QImage mSkyTextureImage;
		QVector<SimBody*> mSimBodies;
		QVector<SimJoint*> mSimJoints;

		IntValue *mStepsPerSecondValue;
		BoolValue *mRunInPerformanceMode;
		bool mGlIsUpdating;

		QList<Event*> mTriggerPaintEvents;
		QGLFramebufferObject *mFrameBuffer;
		BoolValue *mUseAsFrameGrabber;

		CommandLineArgument *mDisableTexturesArg;
};

}
#endif

